#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <SDL.h>
#include <SDL_syswm.h>

#include <glib.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#include "private.h"
#include "SDL_cinema.h"
#include "debug.h"

#define DEBUG_DOMAIN "CINEMA"

static gboolean basic_init_done = FALSE;

static Window window;
static char *media_uri = NULL;
static GstElement *pipeline = NULL;

static guint bus_watch_id = 0;

static void free_pipeline()
{
	if (bus_watch_id) {
		g_source_remove(bus_watch_id);
		bus_watch_id = 0;
	}

	if (pipeline) {
		gst_element_set_state(pipeline, GST_STATE_NULL);
		gst_object_unref(pipeline);
		pipeline = NULL;
	}
}

void CIN_DeInit()
{
	TRACE("DeInit");

	free_pipeline();

	g_free(media_uri);
	media_uri = NULL;
}

int CIN_Init()
{
	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version);

	/* We need GLib threading working, or GStreamer will quietly do it for us */
	if (PDL_EnsureInit() != PDL_NOERROR) {
		return SDL_FALSE;
	}

	if (!basic_init_done) {
		gst_init(NULL, NULL);
		basic_init_done = TRUE;
	}

	if ( SDL_GetWMInfo(&wminfo) ) {
		window = wminfo.info.x11.window;
	} else {
		return SDL_FALSE;
	}

	TRACE("Init; window xid is 0x%lx", (unsigned long) window);

	return SDL_TRUE;
}

/* Because I'm not sure wheter it is good practice to leave GST pipelines
 * floating around in paused state, I defer creation of it until CIN_Play(). */
int CIN_LoadCIN(const char *url)
{
	GError *err = NULL;

	/* "Unload" previous media. */
	free_pipeline();
	g_free(media_uri);
	media_uri = NULL;

	/* GStreamer wants an absolute URI. Games are sloppy about it. Clean mess. */
	TRACE("Passed URL=\"%s\"", url);
	char *path;
	if (g_ascii_strncasecmp("file://./", url, 8) == 0) {
		/* Malformed relative file:// uri. */
		path = realpath(&url[7], NULL);
	} else if (g_ascii_strncasecmp("file://", url, 6) == 0) {
		gchar * filename = g_filename_from_uri(url, NULL, &err);
		if (!filename) {
			WARN("Failed to parse \"file://\" URI: %s", err->message);
			return SDL_FALSE;
		}
		path = realpath(filename, NULL);
		g_free(filename);
	} else if (url[0] == '/') {
		/* Absolute path */
		path = strdup(url);
	} else {
		/* Assume it is a relative path. */
		path = realpath(url, NULL);
	}
	if (!path) {
		WARN("Failed to canonicalize path for \"%s\"", url);
		return SDL_FALSE;
	}

	media_uri = g_filename_to_uri(path, NULL, &err);
	free(path);

	if (!media_uri) {
		WARN("Failed to convert into valid URI: %s", err->message);
	}

	TRACE("Cleaned URL=\"%s\"", media_uri);
	return SDL_TRUE;
}

static gboolean bus_cb(GstBus *bus, GstMessage *msg, gpointer data)
{
	switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR: {
			SDL_Event event;
			GError *err;
			gchar *debug;

			gst_message_parse_error(msg, &err, &debug);
			WARN("GStreamer Pipeline Error: %s", err->message);

			event.type = SDL_USEREVENT;
			event.user.code = SDL_CINEMAEVENT | SDL_CINEMA_ERR;
			event.user.data1 = NULL;
			event.user.data2 = NULL;
			SDLPRE_PushEvent(&event);

			g_error_free(err);
			g_free(debug);

			bus_watch_id = 0;
			return FALSE;
		}
		break;
		case GST_MESSAGE_EOS: {
			TRACE("GStreamer Pipeline EOS");

			SDL_Event event;
			event.type = SDL_USEREVENT;
			event.user.code = SDL_CINEMAEVENT | SDL_CINEMA_EOS;
			event.user.data1 = NULL;
			event.user.data2 = NULL;
			SDLPRE_PushEvent(&event);

			bus_watch_id = 0;
			return FALSE;
		}
		break;
		default:
		break;
	}

	return TRUE;
}

void CIN_Play()
{
	TRACE("Start playing now");

	if (!pipeline) {
		GstElement *playbin, *videosink;
		GstBus *bus;

		pipeline = gst_pipeline_new("cinema");
		playbin = gst_element_factory_make("playbin2", "playbin");
		videosink = gst_element_factory_make("xvimagesink", "videosink");
		g_object_set(videosink,
			"autopaint-colorkey", FALSE,
			"colorkey", 0x000000,
			(void*) 0);
		gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(videosink), window);
		g_object_set(G_OBJECT(playbin),
			"uri", media_uri,
			"video-sink", videosink,
			NULL);
		gst_bin_add(GST_BIN(pipeline), playbin);

		bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
		bus_watch_id = gst_bus_add_watch(bus, bus_cb, NULL);
	} else {
		/* Resuming existing pipeline */
	}

	gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void CIN_Pause()
{
	TRACE("Pause");
	gst_element_set_state(pipeline, GST_STATE_PAUSED);
}

void CIN_Stop()
{
	TRACE("Stop playing now");
	free_pipeline();
}

