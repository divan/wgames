#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <glib.h>
#include <SDL.h>

#include "config.h"

#include "override.h"
#include "private.h"
#include "debug.h"

#define DEBUG_DOMAIN    "ACCEL"
#define CAL_FILE        "/etc/preenv/cal"
#define ACCEL_FILE      "/sys/class/i2c-adapter/i2c-3/3-001d/coord"
#define ACCEL_BUF_SIZE  256

#define EVENT_INTERVAL  100

static struct _SDL_Joystick
{
	int fd;
	Sint16 x, y, z;
	char opened;
} accel = { 0 };

static SDL_bool poll = SDL_TRUE;
static guint timer = 0;

#define SINT16_MAX 32767
#define SINT16_MIN -32768

static int cal_offset[3] = { 0, 0, 0 };
static int cal_divider[3] = { 1000, 1000, 1000 };

static void read_calibration()
{
	static char buffer[ACCEL_BUF_SIZE+1];
	int n, fd = open(CAL_FILE, O_RDONLY);
	if (fd < 0) {
		TRACE("Using default calibration values");
		return;
	}

	if ((n = read(fd, &buffer, ACCEL_BUF_SIZE)) <= 0) {
		WARN("Failed to read calibration file");
		close(fd);
		return;
	}
	buffer[n] = '\0';

	if (sscanf(buffer, "%i %i %i %i %i %i",
		&cal_offset[0], &cal_offset[1], &cal_offset[2],
		&cal_divider[0], &cal_divider[1], &cal_divider[2]) == 6) {
		TRACE("Using calibration values: %i %i %i %i %i %i",
			cal_offset[0], cal_offset[1], cal_offset[2],
			cal_divider[0], cal_divider[1], cal_divider[2]);
	} else {
		WARN("Failed to parse calibration file");
	}

	close(fd);
}

static inline Sint16 scale(Uint8 axis, long v)
{
	long s = ((v + cal_offset[axis]) * SINT16_MAX) / cal_divider[axis];
	/* Clamp value */
	if (s > SINT16_MAX) return SINT16_MAX;
	if (s < SINT16_MIN) return SINT16_MIN;
	return s;
}

static inline void maybe_update(Uint8 axis, Sint16 * value, int new)
{
	Sint16 scaled = scale(axis, new);
	if (*value != scaled) {
		if (poll) {
			SDL_Event fakeevent;
			SDL_JoyAxisEvent *fake = &fakeevent.jaxis;
			fake->type = SDL_JOYAXISMOTION;
			fake->which = 0;
			fake->axis = axis;
			fake->value = scaled;
			SDLPRE_PushEvent(&fakeevent);
		}
		*value = scaled;
	}
}

static inline int parse_values(const char *buf, int *x, int *y, int *z)
{
	return sscanf(buf, "%i %i %i", x, y, z) == 3;
}

static int accel_update()
{
	static char buffer[ACCEL_BUF_SIZE+1];
	ssize_t r;
	int x, y, z;

	if (!accel.opened) {
		/*
		TRACE("Timer called while accelerometer closed");
		*/
		return 0;
	}

	if (lseek(accel.fd, 0, SEEK_SET) != 0) {
		WARN("fseek failed");
		return 0;
	}
	if ((r = read(accel.fd, &buffer, ACCEL_BUF_SIZE)) <= 0) {
		WARN("failed to read accel file");
		return 0;
	}
	buffer[r] = '\0';
	if (!parse_values(buffer, &x, &y, &z)) {
		WARN("failed to parse values");
		return 0;
	}

	/* Mind the rotation */
	maybe_update(0, &accel.x, y);
	maybe_update(1, &accel.y, -x);
	maybe_update(2, &accel.z, z);

	return 1;
}

static gboolean accel_poll(gpointer data)
{
	if (!accel_update()) return FALSE;
	return TRUE;
}

static void update_polling()
{
	/* SDL normally uses event thread to poll joystick; I will set a timer to
     * get a similar behaviour. */
	if (accel.opened && poll) {
		/* Joystick is opened, and we want polling. */
		timer = g_timeout_add(EVENT_INTERVAL, accel_poll, NULL);
	} else if (timer) {
		/* Joystick is closed or polling is disabled, kill timer. */
		g_source_remove(timer);
		timer = 0;
	}
}

int SDL_NumJoysticks(void)
{
	OVERRIDES(SDL_NumJoysticks, (), int);
	return 1 + SUPER();
}

const char *SDL_JoystickName(int index)
{
	OVERRIDES(SDL_JoystickName, (int index), const char *);
	if (index == 0) {
		return "Accelerometer";
	} else {
		return SUPER(index - 1);
	}
}

SDL_Joystick *SDL_JoystickOpen(int index)
{
	OVERRIDES(SDL_JoystickOpen, (int index), SDL_Joystick *);
	TRACE("Called JoystickOpen %d", index);
	if (index == 0) {
		if (accel.opened) {
			WARN("Accelerometer is already opened");
			return &accel;
		}
		/* We need the glib thread */
		if (PDL_EnsureInit() != PDL_NOERROR) {
			return NULL;
		}
		TRACE("Opening accelerometer");
		accel.fd = open(ACCEL_FILE, O_RDONLY|O_NONBLOCK);
		if (accel.fd < 0) {
			SDL_SetError("Failure to open accelerometer");
			return NULL;
		}
		accel.opened = 1;
		read_calibration();
		update_polling();
		return &accel;
	} else {
		return SUPER(index - 1);
	}
}

void SDL_JoystickClose(SDL_Joystick *joystick)
{
	OVERRIDES(SDL_JoystickClose, (SDL_Joystick *joystick), void);
	if (joystick == &accel) {
		TRACE("Closing accelerometer");
		accel.opened = 0;
		update_polling();
		close(accel.fd);
		accel.fd = -1;
	} else {
		SUPER(joystick);
	}
}

int SDL_JoystickEventState(int state)
{
	OVERRIDES(SDL_JoystickEventState, (int state), int);
	int cur_state = poll ? SDL_ENABLE : SDL_IGNORE;
	if (state == SDL_QUERY) {
		TRACE("Application queries current joystick event state");
	} else if (state == SDL_ENABLE) {
		TRACE("Application wants us to poll accelerometer");
		poll = SDL_TRUE;
	} else if (state == SDL_IGNORE) {
		TRACE("Application will poll accelerometer by itself");
		poll = SDL_FALSE;
	}
	update_polling();
	return cur_state;
}

void SDL_JoystickUpdate()
{
	OVERRIDES(SDL_JoystickUpdate, (), void);
	accel_update();
	SUPER();
}

int SDL_JoystickOpened(int index)
{
	OVERRIDES(SDL_JoystickOpened, (int index), int);
	TRACE("Called JoystickOpened %d", index);
	if (index == 0) {
		return accel.opened;
	} else {
		return SUPER(index - 1);
	}
}

Sint16 SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis)
{
	OVERRIDES(SDL_JoystickGetAxis, (SDL_Joystick *joystick, int axis), Sint16);
	if (SDL_NumJoysticks() == 1) {
		switch (axis) {
			case 0:
				return accel.x;
			case 1:
				return accel.y;
			case 2:
				return accel.z;
			default:
				WARN("Invalid joystick axis");
				SDL_SetError("Joystick only has 3 axis");
				return 0;
		}
	} else if (keymap.enable_dpad == 0) {
		switch (axis) {
			case 0:
				return SUPER(real_joystick, 5);
			case 1:
				return SUPER(real_joystick, 4);
			case 2:
				return SUPER(real_joystick, 6);
			default:
				return 0;
		}
	} else {
		return SUPER(joystick, axis);
	}
}
