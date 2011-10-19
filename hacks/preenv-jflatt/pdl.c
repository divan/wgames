#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "config.h"
#include <gconf/gconf-client.h>

#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib.h>
#include <libosso.h>
#include <hildon-mime.h>

#if USE_LIBLOCATION
#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#endif

#include "PDL.h"
#include "private.h"
#include "debug.h"

#define DEBUG_DOMAIN "PDL"

#define SCREEN_TIMEOUT_TIME 45

#define BT_SERVICE				"org.bluez"
#define BT_DEF_ADAPTER_IF		"org.bluez.Manager"
#define BT_DEF_ADAPTER_PATH		"/"
#define BT_DEF_ADAPTER_METHOD	"DefaultAdapter"
#define BT_ADAPTER_IF			"org.bluez.Adapter"
#define BT_ADAPTER_GETPROPS		"GetProperties"

#define UNIQUE_ID_FILE			"/home/user/.preenv-uid"
#define RANDOM_SRC_FILE			"/dev/random"
#define UNIQUE_ID_RND_SIZE		24

static gboolean basic_init_done = FALSE;
static GMainContext *main_context = NULL;
static GMainLoop *main_loop = NULL;
static osso_context_t *osso_context = NULL;
static GThread *main_thread = NULL;
static gchar * exepath = NULL;
static gchar * servicename = NULL;

#if USE_LIBLOCATION
static LocationGPSDControl *location = NULL;
static LocationGPSDevice *loc_device = NULL;
#endif

/* Current status */
static gboolean dimming_prevention_enabled = FALSE;
static gboolean system_pause_ui_enabled = FALSE;

static gchar * last_error = NULL;
static guint dimming_timer_id = 0;

static PDL_Location last_loc = { -1 };
static PDL_Err last_loc_error = PDL_NOERROR;

static gchar* get_exe_path()
{
	pid_t pid = getpid();
	gchar * linkname = g_strdup_printf("/proc/%i/exe", pid);
	gchar * link = g_file_read_link(linkname, NULL);
	g_free(linkname);
	return link;
}

static gchar* get_device_name()
{
	GError *error = NULL;
	gchar *name = NULL;
	DBusGConnection *bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (!bus) {
		WARN("Failed to get system bus: %s", error->message);
		g_error_free(error);
		return NULL;
	}

	DBusGProxy* mgr_proxy = dbus_g_proxy_new_for_name(bus,
		BT_SERVICE, BT_DEF_ADAPTER_PATH, BT_DEF_ADAPTER_IF);
	DBusGProxy* adp_proxy = NULL;
	if (!dbus_g_proxy_call(mgr_proxy, BT_DEF_ADAPTER_METHOD, &error,
		G_TYPE_INVALID, DBUS_TYPE_G_PROXY, &adp_proxy, G_TYPE_INVALID)) {
		/* needed for raging thunder 2 */
		if (error != NULL) {
			WARN("Failed to get Bluez manager: %s", error->message);
			g_error_free(error);
		} else {
			WARN("Failed to get Bluez manager");
		}
		goto err_clean_mgr;
	}

	GHashTable *props = NULL;
	dbus_g_proxy_set_interface(adp_proxy, BT_ADAPTER_IF);
	if (!dbus_g_proxy_call(adp_proxy, BT_ADAPTER_GETPROPS, &error,
		G_TYPE_INVALID,
		dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE),
		&props, G_TYPE_INVALID)) {
		if (error != NULL) {
			WARN("Failed to get Bluez properties: %s", error->message);
		} else {
			WARN("Failed to get Bluez properties");
		}
		g_error_free(error);
		goto err_clean_adp;
	}

	GValue *value = g_hash_table_lookup(props, "Name");
	if (!value) {
		WARN("Failed to get Bluez property 'Name'");
		goto err_clean_ht;
	}

	name = g_value_dup_string(value);

err_clean_ht:
	g_hash_table_destroy(props);
err_clean_adp:
	g_object_unref(adp_proxy);
err_clean_mgr:
	g_object_unref(mgr_proxy);
	return name;
}

static void set_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	last_error = g_strdup_vprintf(format, args);
	va_end(args);
}

static PDL_Err copy_to_user(const char *data, char *buffer, int bufferLen)
{
	if (!buffer || bufferLen <= 0) {
		set_error("buffer is NULL or bufferLen is 0");
		return PDL_INVALIDINPUT;
	}
	
	if (data && bufferLen < (strlen(data) + 1)) {
		set_error("bufferLen not enough to hold %d characters", strlen(data));
		return PDL_STRINGTOOSMALL;
	}

	strncpy(buffer, data, bufferLen);
	return PDL_NOERROR;
}

#if USE_LIBLOCATION
static void loc_error_cb(LocationGPSDControl *control, LocationGPSDControlError error, gpointer user_data)
{
	SDL_Event event;
	switch (error) {
	case LOCATION_ERROR_USER_REJECTED_DIALOG:
		WARN("GPS: User didn't enable requested methods");
		last_loc_error = PDL_NOTALLOWED;
	break;
	case LOCATION_ERROR_USER_REJECTED_SETTINGS:
		WARN("GPS: User changed settings, which disabled location");
		last_loc_error = PDL_NOTALLOWED;
	break;
	case LOCATION_ERROR_BT_GPS_NOT_AVAILABLE:
		WARN("GPS: Problems with BT GPS");
		last_loc_error = PDL_ECONNECTION;
	break;
	case LOCATION_ERROR_METHOD_NOT_ALLOWED_IN_OFFLINE_MODE:
		WARN("GPS: Requested method is not allowed in offline mode");
		last_loc_error = PDL_NOTALLOWED;
	break;
	case LOCATION_ERROR_SYSTEM:
		WARN("GPS: System error");
		last_loc_error = PDL_EOTHER;
	break;
	default:
		WARN("GPS: Unknown error");
		last_loc_error = PDL_EOTHER;
	break;
	}

	event.type = SDL_USEREVENT;
	event.user.code = PDL_GPS_FAILURE;
	event.user.data1 = &last_loc_error;
	SDLPRE_PushEvent(&event);
}

static void loc_changed_cb(LocationGPSDevice *device, gpointer user_data)
{
	SDL_Event event;
	if (!device) return;
	if (!device->fix) return;

	TRACE("Got location event");
	if (device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET) {
		last_loc.latitude = device->fix->latitude;
		last_loc.longitude = device->fix->longitude;
		last_loc.horizontalAccuracy = device->fix->eph / 100;
	} else {
		last_loc.latitude = -1.0;
		last_loc.longitude = -1.0;
		last_loc.horizontalAccuracy = -1.0;
	}
	if (device->fix->fields & LOCATION_GPS_DEVICE_ALTITUDE_SET) {
		last_loc.altitude = device->fix->altitude;
		last_loc.verticalAccuracy = device->fix->epv;
	} else {
		last_loc.altitude = -1.0;
		last_loc.verticalAccuracy = -1.0;
	}
	if (device->fix->fields & LOCATION_GPS_DEVICE_TRACK_SET) {
		last_loc.heading = device->fix->track;
	} else {
		last_loc.heading = -1.0;
	}
	if (device->fix->fields & LOCATION_GPS_DEVICE_SPEED_SET) {
		last_loc.velocity = device->fix->speed / 3.6;
	} else {
		last_loc.velocity = -1.0;
	}
	event.type = SDL_USEREVENT;
	event.user.code = PDL_GPS_UPDATE;
	event.user.data1 = &last_loc;
	SDLPRE_PushEvent(&event);
}
#endif

/* Running the glib mainloop continually into a specific thread makes us a
 * better citizen (re dbus, etc.). Note that this means signals, etc. might be
 * called by a thread different from the SDL one. SDL's only thread-safe func.
 * is SDL_PushEvent.
 */
static gpointer main_thread_func(gpointer data)
{
	GMainLoop *loop = (GMainLoop*)data;
	g_main_loop_run(loop);
	return GINT_TO_POINTER(1);
}

int confGetInt(GConfClient *gcClient, const gchar *exename, const gchar *key, gint *number, const int dflt)
{
	GConfValue *val = NULL;
	gboolean hasChanged = FALSE;
	gchar *path = g_strconcat("/apps/preenv/", exename, "/", key, NULL);
	val = gconf_client_get_without_default(gcClient, path, NULL);
	g_free(path);
	if (val == NULL) {
		*number = dflt;
		return FALSE;
	}
	if (val->type == GCONF_VALUE_INT) {
		*number = gconf_value_get_int(val);
		hasChanged = TRUE;
	}
	gconf_value_free(val);
	val = NULL;
	return hasChanged;
}

void gconf_settings_get(gchar *exename)
{
	GConfClient *gcClient = NULL;
	gcClient = gconf_client_get_default();
	g_assert(GCONF_IS_CLIENT(gcClient));
	confGetInt(gcClient, exename, "screen_width",      (gint *)&r_size.w,		r_size.w);
	confGetInt(gcClient, exename, "screen_height",     (gint *)&r_size.h,		r_size.h);
	confGetInt(gcClient, exename, "screen_rotation",   (gint *)&s_rotate,		s_rotate);
	confGetInt(gcClient, exename, "enable_dpad",       &keymap.enable_dpad,                0);
	confGetInt(gcClient, exename, "dpad_center_mouse", &keymap.dpad_center.mouse_which,    0);
	confGetInt(gcClient, exename, "dpad_center_x",     &keymap.dpad_center.mouse_press.x,  0);
	confGetInt(gcClient, exename, "dpad_center_y",     &keymap.dpad_center.mouse_press.y,  0);
	/* compass directions */
	confGetInt(gcClient, exename, "dpad_key_n_key",    &keymap.dpad_keys[0].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_n_mouse",  &keymap.dpad_keys[0].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_n_x",      &keymap.dpad_keys[0].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_n_y",      &keymap.dpad_keys[0].mouse_press.y, 0);
	confGetInt(gcClient, exename, "dpad_key_ne_key",   &keymap.dpad_keys[1].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_ne_mouse", &keymap.dpad_keys[1].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_ne_x",     &keymap.dpad_keys[1].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_ne_y",     &keymap.dpad_keys[1].mouse_press.y, 0);
	confGetInt(gcClient, exename, "dpad_key_e_key",    &keymap.dpad_keys[2].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_e_mouse",  &keymap.dpad_keys[2].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_e_x",      &keymap.dpad_keys[2].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_e_y",      &keymap.dpad_keys[2].mouse_press.y, 0);
	confGetInt(gcClient, exename, "dpad_key_se_key",   &keymap.dpad_keys[3].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_se_mouse", &keymap.dpad_keys[3].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_se_x",     &keymap.dpad_keys[3].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_se_y",     &keymap.dpad_keys[3].mouse_press.y, 0);
	confGetInt(gcClient, exename, "dpad_key_s_key",    &keymap.dpad_keys[4].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_s_mouse",  &keymap.dpad_keys[4].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_s_x",      &keymap.dpad_keys[4].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_s_y",      &keymap.dpad_keys[4].mouse_press.y, 0);
	confGetInt(gcClient, exename, "dpad_key_sw_key",   &keymap.dpad_keys[5].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_sw_mouse", &keymap.dpad_keys[5].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_sw_x",     &keymap.dpad_keys[5].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_sw_y",     &keymap.dpad_keys[5].mouse_press.y, 0);
	confGetInt(gcClient, exename, "dpad_key_w_key",    &keymap.dpad_keys[6].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_w_mouse",  &keymap.dpad_keys[6].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_w_x",      &keymap.dpad_keys[6].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_w_y",      &keymap.dpad_keys[6].mouse_press.y, 0);
	confGetInt(gcClient, exename, "dpad_key_nw_key",   &keymap.dpad_keys[7].keycode,       0);
	confGetInt(gcClient, exename, "dpad_key_nw_mouse", &keymap.dpad_keys[7].mouse_which,   0);
	confGetInt(gcClient, exename, "dpad_key_nw_x",     &keymap.dpad_keys[7].mouse_press.x, 0);
	confGetInt(gcClient, exename, "dpad_key_nw_y",     &keymap.dpad_keys[7].mouse_press.y, 0);

	int i;
	for (i = 0; i < MAX_KEYS; i++) {
		gchar keyname[14];
		g_snprintf(keyname, sizeof(keyname), "key_%d_key", i);
		confGetInt(gcClient, exename, keyname, &keymap.keys[i].keycode, 0);
		g_snprintf(keyname, sizeof(keyname), "key_%d_mouse", i);
		confGetInt(gcClient, exename, keyname, &keymap.keys[i].mouse_which, 0);
		g_snprintf(keyname, sizeof(keyname), "key_%d_x", i);
		confGetInt(gcClient, exename, keyname, &keymap.keys[i].mouse_press.x, 0);
		g_snprintf(keyname, sizeof(keyname), "key_%d_y", i);
		confGetInt(gcClient, exename, keyname, &keymap.keys[i].mouse_press.y, 0);
	}

	int j;
	for (j = 0; j < MAX_JOYBUTTONS; j++) {
		gchar keyname[13];
		g_snprintf(keyname, sizeof(keyname), "joy_%d_key", j);
		confGetInt(gcClient, exename, keyname, &keymap.jbuttons[j].keycode, 0);
		g_snprintf(keyname, sizeof(keyname), "joy_%d_mouse", j);
		confGetInt(gcClient, exename, keyname, &keymap.jbuttons[j].mouse_which, 0);
		g_snprintf(keyname, sizeof(keyname), "joy_%d_x", j);
		confGetInt(gcClient, exename, keyname, &keymap.jbuttons[j].mouse_press.x, 0);
		g_snprintf(keyname, sizeof(keyname), "joy_%d_y", j);
		confGetInt(gcClient, exename, keyname, &keymap.jbuttons[j].mouse_press.y, 0);
	}

	int k;
	for (k = 0; k < MAX_AXIS; k++) {
		gchar keyname[15];
		g_snprintf(keyname, sizeof(keyname), "axis_%d_axis", k);
		confGetInt(gcClient, exename, keyname, &keymap.axis[k].axis, 0);
		g_snprintf(keyname, sizeof(keyname), "axis_%d_mouse", k);
		confGetInt(gcClient, exename, keyname, &keymap.axis[k].mouse_which, 0);
		g_snprintf(keyname, sizeof(keyname), "axis_%d_type", k);
		confGetInt(gcClient, exename, keyname, &keymap.axis[k].type, 0);
		g_snprintf(keyname, sizeof(keyname), "axis_%d_center", k);
		confGetInt(gcClient, exename, keyname, &keymap.axis[k].center, 0);
		g_snprintf(keyname, sizeof(keyname), "axis_%d_min", k);
		confGetInt(gcClient, exename, keyname, &keymap.axis[k].min, 0);
		g_snprintf(keyname, sizeof(keyname), "axis_%d_max", k);
		confGetInt(gcClient, exename, keyname, &keymap.axis[k].max, 0);
	}
	g_object_unref(gcClient);
	gcClient = NULL;
}

PDL_Err PDL_Init(unsigned int flags)
{
	/* Initialize GLib */
	if (!basic_init_done) {
		/* Those functions need to be called only once */
		g_thread_init(NULL);
		g_type_init();

		dbus_g_thread_init();
		basic_init_done = TRUE;
	}

	/* Check against stupid games calling PDL_Init() twice */
	if (osso_context) {
		WARN("Game called PDL_Init twice");
		return PDL_NOERROR;
	}

	TRACE("Init: Preenv %s", LIB_VERSION);

	/* Get a proper prgname for glib */
	exepath = get_exe_path();
	if (!exepath) {
		/* Some applications also need /proc and fail earlier,
		   so we might never reach here */
		WARN("Is /proc mounted?");
		set_error("Failed to get executable path");
		return PDL_SYSTEMERROR_FILE;
	}

	gchar * exename = g_path_get_basename(exepath);
	g_set_prgname(exename);

	gconf_settings_get(exename);

	main_context = g_main_context_default();
	main_loop = g_main_loop_new(NULL, FALSE);

	/* Get D-BUS service name for this application */
	const gchar *srv = g_getenv("PREENV_APPID");
	if (srv && strlen(srv) > 0) {
		servicename = g_strdup(srv);
	} else {
		/* Use the executable name as D-BUS service name, properly sanitized. */
		g_strcanon(exename,
			G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "_", '_');
		servicename = g_strdup_printf("com.javispedro.preenv.%s", exename);
	}
	g_free(exename);

	TRACE("Init: Service name is %s", servicename);

	/* Root check. */
	if (geteuid() == 0) {
		WARN("You are root. Don't do this unless you know what you're doing!");
	}

	/* Prepare libosso. This also sets up the D-BUS connections. */
	osso_context = osso_initialize(servicename, "0.1", TRUE, NULL);
	if (!osso_context) {
		WARN("Failed to initialize libosso");
		set_error("Failed to connect to services");
		return PDL_ECONNECTION;
	}

#if USE_LIBLOCATION
	/* Prepare liblocation */
	if (!location) {
		location = location_gpsd_control_get_default();
		if (!location) {
			WARN("Failed to initialize liblocation");
			set_error("Failed to connect to services");
			return PDL_ECONNECTION;
		}
		g_signal_connect(G_OBJECT(location), "error-verbose",
		                 G_CALLBACK(loc_error_cb), NULL);
	}
	if (!loc_device) {
		loc_device = g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);
		if (!loc_device) {
			set_error("Out of memory");
			return PDL_EMEMORY;
		}
		g_signal_connect(G_OBJECT(loc_device),  "changed",
		                 G_CALLBACK(loc_changed_cb), NULL);
	}
#endif

	main_thread = g_thread_create(main_thread_func, main_loop, TRUE, NULL);
	if (!main_thread) {
		WARN("Cannot create main thread");
		return PDL_EMEMORY;
	}

	TRACE("Main thread created");

	return PDL_NOERROR;
}

void PDL_Quit()
{
	if (!osso_context) {
		WARN("Game called PDL_Quit without calling PDL_Init first");
	}

	if (dimming_timer_id) {
		g_source_remove(dimming_timer_id);
		dimming_timer_id = 0;
	}
	dimming_prevention_enabled = FALSE;
	system_pause_ui_enabled = FALSE;

	TRACE("Waiting for main loop thread to die");
	g_main_loop_quit(main_loop);
	g_thread_join(main_thread);
	main_thread = NULL;
	TRACE("Main loop thread died");

	g_free(exepath);
	exepath = NULL;
	g_free(servicename);
	servicename = NULL;

	osso_deinitialize(osso_context);
	osso_context = NULL;
	g_main_loop_unref(main_loop);
	main_loop = NULL;
	g_main_context_unref(main_context);
	main_context = NULL;
	g_free(last_error);
	last_error = NULL;
}

PDL_Err PDL_EnsureInit()
{
	if (!osso_context) {
		return PDL_Init(0);
	}
	return PDL_NOERROR;
}

/* Stupid games not calling PDL_Init() */
#define CHECK_IMPLICIT_PDL_INIT() \
	if (!osso_context) { \
		PDL_Err err = PDL_Init(0); \
		if (err) return err; \
	}

PDL_Err PDL_BannerMessagesEnable(PDL_bool enable)
{
	CHECK_IMPLICIT_PDL_INIT();
	TRACE("Set do_not_disturb flag to %s",
		enable ? "false" : "true");
	/* I am not sure if this is a good idea after all */
	X11_SetDoNotDisturb(enable ? SDL_FALSE : SDL_TRUE);
	return PDL_NOERROR;
}

PDL_Err PDL_CustomPauseUiEnable(PDL_bool enable)
{
	if (enable) {
		TRACE("Application handles pause events");
		system_pause_ui_enabled = FALSE;
	} else {
		TRACE("I should provide pause UI, but I won't!");
		system_pause_ui_enabled = TRUE;
	}

	return PDL_NOERROR;
}

PDL_Err PDL_EnableLocationTracking(PDL_bool activate)
{
#if USE_LIBLOCATION
	if (activate) {
		TRACE("Application is enabling the GPS");
		location_gpsd_control_start(location);
	} else {
		TRACE("Application is disabling the GPS");
		location_gpsd_control_stop(location);
	}
#else
	WARN("No Location tracking implementation");
#endif
	return PDL_NOERROR;
}

PDL_Err PDL_GesturesEnable(PDL_bool enable)
{
	TRACE("Application tries to enable gesture area");
	/* No gesture area! */
	return PDL_NOERROR;
}

PDL_Err PDL_GetCallingPath(char *buffer, int bufferLen)
{
	TRACE("Application asked for calling path, giving './'");
	return copy_to_user("./", buffer, bufferLen);
}

PDL_Err PDL_GetDeviceName(char *buffer, int bufferLen)
{
	CHECK_IMPLICIT_PDL_INIT();
	PDL_Err err;
	gchar *name = get_device_name();
	if (!name) {
		WARN("Unable to get device name");
		set_error("Unable to get device name");
		return PDL_EOTHER;
	}

	TRACE("Application asked for device name, giving '%s'", name);

	err = copy_to_user(name, buffer, bufferLen);
	g_free(name);
	return err;
}

const char *PDL_GetError(void)
{
	if (last_error) {
		return last_error;
	} else {
		return "No error";
	}
}

const char *PDL_GetHardware(void)
{
	return "pre";
}

PDL_Err PDL_GetLanguage(char *buffer, int bufferLen)
{
	const char *lang = getenv("LANG");
	if (!lang || strlen(lang) == 0) {
		lang = "en_US";
	} else if (strcasecmp(lang, "en_UK") == 0) {
		lang = "en_GB";
	} /* TODO: Check if more special cases are needed. */

	TRACE("Application asked for lang, giving %s", lang);

	return copy_to_user(lang, buffer, bufferLen);
}

PDL_Err PDL_GetLocation(PDL_Location *location)
{
	TRACE("Application is requesting current location");
#if USE_LIBLOCATION
	if (!loc_device->online) {
		WARN("Called %s without a connection to GPS", __func__);
		return PDL_UNINIT;
	}
#else
	WARN("No Location tracking implementation");
#endif
	*location = last_loc;
	return PDL_NOERROR;
}

PDL_Err PDL_GetNetInfo(const char *interfaceName, PDL_NetInfo * interfaceInfo)
{
	if (!interfaceName || !interfaceInfo) {
		set_error("interfaceName or interfaceInfo is NULL");
		return PDL_INVALIDINPUT;
	}

	/* Seems that most applications will say interfaceName=eth0 here */
	if (g_ascii_strcasecmp(interfaceName, "eth0") == 0) {
		int fd;
		struct ifreq ifr;
		TRACE("Getting info from wlan0");
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);

		if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
			WARN("Failed to get interface address");
			set_error("Failed to get interface address");
			return PDL_SYSTEMERROR_NET;
		}
		interfaceInfo->ipaddress = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
		TRACE("Found IP address 0x%x = %s", interfaceInfo->ipaddress,
			inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

		if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0) {
			WARN("Failed to get interface netmask");
			set_error("Failed to get interface netmask");
			return PDL_SYSTEMERROR_NET;
		}
		interfaceInfo->netmask = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
		TRACE("Found Netmask 0x%x = %s", interfaceInfo->netmask,
			inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

		if (ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0) {
			WARN("Failed to get interface broadcast address");
			set_error("Failed to get interface broadcast address");
			return PDL_SYSTEMERROR_NET;
		}
		interfaceInfo->broadcast = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
		TRACE("Found Broadcast address 0x%x = %s", interfaceInfo->broadcast,
			inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

		return PDL_NOERROR;
	} else {
		WARN("I do not know how to translate interface '%s'", interfaceName);
		set_error("Unknown interface '%s'", interfaceName);
		return PDL_INVALIDINPUT;
	}
}

PDL_Err PDLNet_Get_Info(const char *interfaceName, PDL_NetInfo * interfaceInfo)
{
	return PDL_GetNetInfo(interfaceName, interfaceInfo);
}

PDL_Err PDL_GetOSVersion(PDL_OSVersion *version)
{
	TRACE("Application asks for OS version");
	version->majorVersion = 1;
	version->minorVersion = 4;
	version->revision = 5;
	version->versionStr = "Preenv " LIB_VERSION;
	return PDL_NOERROR;
}

PDL_Err PDL_GetScreenMetrics(PDL_ScreenMetrics* metrics)
{
	TRACE("Application asks for screen metrics; giving true ones");
	metrics->horizontalPixels = 480;
	metrics->verticalPixels = 800;
	metrics->horizontalDPI = 213;
	metrics->verticalDPI = 183;
	metrics->aspectRatio = 1.164;
	return PDL_NOERROR;
}

PDL_Err PDL_GetUniqueID(char *buffer, int bufferLen)
{
	TRACE("Called GetUniqueID");
	PDL_Err pdl_err;
	gchar *contents = NULL;
	if (!g_file_get_contents(UNIQUE_ID_FILE, &contents, NULL, NULL)) {
		static guchar buffer[UNIQUE_ID_RND_SIZE];
		GError *error = NULL;
		int fd = open(RANDOM_SRC_FILE, O_RDONLY);
		ssize_t nread = 0;

		TRACE("Initializing device unique ID");

		if (fd == -1) {
			WARN("Couldn't open %s\n", RANDOM_SRC_FILE);
			goto err_return;
		}
		while (nread < UNIQUE_ID_RND_SIZE) {
			nread = read(fd, &buffer[nread], UNIQUE_ID_RND_SIZE - nread);
			if (nread <= 0) {
				WARN("Failed to read %d bytes of randomness\n",
					UNIQUE_ID_RND_SIZE);
				close(fd);
				goto err_return;
			}
		}
		close(fd);

		contents = g_compute_checksum_for_data(G_CHECKSUM_SHA1, buffer,
			UNIQUE_ID_RND_SIZE);

		if (!g_file_set_contents(UNIQUE_ID_FILE, contents, -1, &error)) {
			WARN("Failed to write your generated ID to %s: %s",
				UNIQUE_ID_FILE, error->message);
			/* Proceed either way */
			g_error_free(error);
		} else {
			TRACE("Your new device ID is %s", contents);
		}
	}

	TRACE("Application asked for device ID");
	pdl_err = copy_to_user(contents, buffer, bufferLen);
	g_free(contents);
	return pdl_err;

err_return:
	TRACE("Application asked for device ID, but I couldn't get one, so 0000 is it");
	return copy_to_user("0000", buffer, bufferLen);
}

PDL_bool PDL_IsPlugin()
{
	/* No support for hybrid applications. */
	return PDL_FALSE;
}

PDL_Err PDL_LaunchBrowser(const char* url)
{
	CHECK_IMPLICIT_PDL_INIT();
	TRACE("Launch browser, url=\"%s\"", url);
	GError *error = NULL;
	if (hildon_uri_open(url, NULL, &error)) {
		return PDL_NOERROR;
	} else {
		WARN("Failed to open browser: %s", error->message);
		set_error("Failed to open browser: %s", error->message);
		g_error_free(error);
		return PDL_ECONNECTION;
	}
}

PDL_Err PDL_NotifyMusicPlaying(PDL_bool musicPlaying)
{
	TRACE("Playing music notification: %s", musicPlaying ? "Playing" : "No music");

	return PDL_NOERROR;
}

static gboolean screen_timeout_cb(gpointer data)
{
	osso_return_t res = osso_display_blanking_pause(osso_context);
	if (res == OSSO_OK) {
		return TRUE;
	} else {
		dimming_timer_id = 0;
		return FALSE;
	}
}

PDL_Err PDL_ScreenTimeoutEnable(PDL_bool enable)
{
	CHECK_IMPLICIT_PDL_INIT();
	if (dimming_timer_id) {
		g_source_remove(dimming_timer_id);
		dimming_timer_id = 0;
	}
	if (!enable) {
		dimming_prevention_enabled = TRUE;
		/* Do an initial blanking pause. */
		osso_return_t res = osso_display_blanking_pause(osso_context);
		if (res != OSSO_OK) {
			set_error("Cannot connect to services");
			return PDL_ECONNECTION;
		}
		/* Schedule next one */
		dimming_timer_id = g_timeout_add_seconds(SCREEN_TIMEOUT_TIME,
			screen_timeout_cb, NULL);
	} else {
		dimming_prevention_enabled = FALSE;
	}

	TRACE("Screen dimming prevention %s",
		dimming_prevention_enabled ? "ON" : "OFF");

	return PDL_NOERROR;
}

PDL_Err PDL_SetFirewallPortStatus(int port, PDL_bool open)
{
	/* Maemo has the best firewall of them all: none. */
	TRACE("%s network port %d", open ? "Open" : "Close", port);
	return PDL_NOERROR;
}

PDL_Err PDL_SetOrientation(PDL_Orientation orientation)
{
	TRACE("Switch UI orientation to %u°", orientation * 90);
	/* TODO */
	return PDL_NOERROR;
}

void PDL_NotifyFocus(SDL_bool focus)
{
	if (dimming_prevention_enabled) {
		if (focus) {
			/* Gaining focus; add timer. */
			if (!dimming_timer_id) {
				dimming_timer_id = g_timeout_add_seconds(SCREEN_TIMEOUT_TIME,
					screen_timeout_cb, NULL);
			}
		} else {
			/* Losing focus, kill timer. */
			if (dimming_timer_id) {
				g_source_remove(dimming_timer_id);
				dimming_timer_id = 0;
			}
		}
		TRACE("Readjusting dimming prevention timer");
	}
}

PDL_Err PDL_SetTouchAggression(PDL_TouchAggression aggression)
{
	TRACE("Application tries to set 'touch aggression'");
	/* But we have no multitouch input here */
	return PDL_NOERROR;
}

int PDL_isAppLicensedForDevice(const char *appid)
{
	WARN("Application is copy-protected. Sorry! (%s:%d)", __FILE__, __LINE__);
	return 0;
}

