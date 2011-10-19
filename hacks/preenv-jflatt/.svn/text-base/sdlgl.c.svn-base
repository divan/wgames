#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_gles.h>
#include <GLES/gl.h>
#include <EGL/egl.h>

#include "config.h"

#include "SDLGL.h"

#include "override.h"
#include "private.h"
#include "debug.h"

#define DEBUG_DOMAIN "SDLGL"

static SDL_GLES_Version desired_version = SDL_GLES_VERSION_NONE;
static SDL_bool gles_inited = SDL_FALSE;
static SDL_GLES_Context *context = NULL;

static SDL_Surface *v_screen = NULL;

static SDL_bool app_active = SDL_FALSE;
static SDL_EventFilter app_filter = NULL;

static int gles_init()
{
	if (!gles_inited) {
		int res = SDL_GLES_Init(desired_version);
		if (res != 0) return res;
		TRACE("SDL_GLES initialized (res=%d)", res);
		gles_inited = SDL_TRUE;

		/* Palm SDL has a 16 bit depth buffer by default,
		 * but I'm going to set it to 8 by default,
		 * because it's faster on a N900. */
		res = SDL_GLES_SetAttribute(SDL_GLES_DEPTH_SIZE, 8);
		if (res != 0) return res;
	}
	return 0;
}

static void gles_quit()
{
	if (gles_inited) {
		if (context) {
			SDL_GLES_DeleteContext(context);
			context = NULL;
		}
		SDL_GLES_Quit();
		gles_inited = SDL_FALSE;
	}
}

static inline int mini(int i1, int i2)
{
	if (i1 < i2) return i1;
	else return i2;
}

static inline int absi(int i)
{
	if (i < 0) return -i;
	else return i;
}

static void make_box(struct box *b, const struct point *p1, const struct point *p2)
{
	b->x = mini(p1->x, p2->x);
	b->y = mini(p1->y, p2->y);
	b->w = absi(p1->x - p2->x);
	b->h = absi(p1->y - p2->y);
}

static void p_scale(struct point *p) 
{
	const int ox = p->x, oy = p->y;
	switch (s_rotate) {
		case PDL_ORIENTATION_0:
			p->x = ox * s_scale.x;
			p->y = oy * s_scale.y;
			break;
		case PDL_ORIENTATION_90:
			p->x = oy * s_scale.x;
			p->y = r_size.h - ox * s_scale.y;
			break;
		case PDL_ORIENTATION_180:
			p->x = r_size.w - ox * s_scale.x;
			p->y = r_size.h - oy * s_scale.y;
			break;
		case PDL_ORIENTATION_270:
			p->x = r_size.w - oy * s_scale.x;
			p->y = ox * s_scale.y;
			break;
		default:
			break;
	}
}

static void p_unscale(struct point *p) 
{
	const int ox = p->x, oy = p->y;
	switch (s_rotate) {
		case PDL_ORIENTATION_0:
			p->x = ox / s_scale.x;
			p->y = oy / s_scale.y;
			break;
		case PDL_ORIENTATION_90:
			p->y = v_size.h - ox / s_scale.x;
			p->x = oy / s_scale.y;
			break;
		case PDL_ORIENTATION_180:
			p->x = v_size.w - ox / s_scale.x;
			p->y = v_size.h - oy / s_scale.y;
			break;
		case PDL_ORIENTATION_270:
			p->y = ox / s_scale.x;
			p->x = v_size.w - oy / s_scale.y;
			break;
		default:
			break;
	}
}

void SDLPRE_PushEvent(SDL_Event *event) {
	/* Here's to hoping the app_filter function is thread-safe. */
	if (app_filter && !app_filter(event)) {
		/* Application's event filter handled this event. */
		return;
	}
	SDL_PushEvent(event);
}

static int filter_active_event(const SDL_ActiveEvent *event) {
	SDL_Event fakeevent;
	SDL_ActiveEvent *fake = &fakeevent.active;
	fake->type = SDL_ACTIVEEVENT;
	fake->gain = event->gain;
	fake->state = 0;
	if (event->state & SDL_APPINPUTFOCUS) {
		TRACE("Sending %s focus event", fake->gain ? "gain" : "lose");
		PDL_NotifyFocus(fake->gain);
		app_active = fake->gain;
		fake->state = SDL_APPACTIVE;
	}
	if (fake->state) {
		SDLPRE_PushEvent(&fakeevent);
	}
	return 0;
}

static int filter_mouse_motion(const SDL_MouseMotionEvent *event) {
	/*
	TRACE("mouse %d motion %d %dx%d %dx%d", event->which, event->state, event->x, event->y, event->xrel, event->yrel);
	*/
	SDL_Event fakeevent;
	SDL_MouseMotionEvent *fake = &fakeevent.motion;
	*fake = *event;
	struct point p = { event->x, event->y };
	struct point prel = { event->xrel, event->yrel };
	p_unscale(&p);
	p_unscale(&prel);
	fake->x = p.x;
	fake->y = p.y;
	fake->xrel = prel.x;
	fake->yrel = prel.y;

	SDLPRE_PushEvent(&fakeevent);
	return 0;
}

static int filter_mouse_button(const SDL_MouseButtonEvent *event) {
	/*
	TRACE("mouse %d button %d %dx%d", event->which, event->state, event->x, event->y);
	*/
	SDL_Event fakeevent;
	SDL_MouseButtonEvent *fake = &fakeevent.button;
	*fake = *event;
	struct point p = { event->x, event->y };
	p_unscale(&p);
	fake->x = p.x;
	fake->y = p.y;

	SDLPRE_PushEvent(&fakeevent);
	return 0;
}

static int filter_event(const SDL_Event *event)
{
	switch (event->type) {
	case SDL_VIDEORESIZE:
		TRACE("Deleted a video resize event");
		return 0;
	case SDL_ACTIVEEVENT:
		return filter_active_event(&event->active);
	case SDL_MOUSEMOTION:
		return filter_mouse_motion(&event->motion);
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		return filter_mouse_button(&event->button);
	case SDL_JOYAXISMOTION:
		{
		axis_mapping map = keymap.axis[event->jaxis.axis];

		/* eliminate false touchscreen events */
		if (map.mouse_which == 0)
			return 0;

		/* just a hack for now */
		int xaxis = 0;
		int yaxis = 0;
		if (map.axis == 0 || map.axis == 1) {
			xaxis = 0;
			yaxis = 1;
		} else if (map.axis == 2 || map.axis == 3) {
			xaxis = 2;
			yaxis = 3;
		} else if (map.axis == 12 || map.axis == 13) {
			xaxis = map.axis;
			yaxis = map.axis;
		}
		/* should only be 5 Pre mouse max, try allowing more */
		static point last_mouse[10];

		/* fake a mouse button */
		if (SDL_JoystickGetAxis(real_joystick, xaxis) == 0 &&
				SDL_JoystickGetAxis(real_joystick, yaxis) == 0) {
			SDL_MouseButtonEvent fakeButton;
			fakeButton.button = SDL_BUTTON_LEFT;
			fakeButton.which = map.mouse_which;
			fakeButton.type = SDL_MOUSEBUTTONUP;
			fakeButton.state = SDL_RELEASED;
			fakeButton.x = keymap.axis[xaxis].center;
			fakeButton.y = keymap.axis[yaxis].center;
			//hack
			if (xaxis == yaxis)
				fakeButton.x = 720;
			filter_mouse_button(&fakeButton);
		}

		if (event->jaxis.axis == map.axis) {
			SDL_MouseMotionEvent fakeMotion;
			fakeMotion.type = SDL_MOUSEMOTION;
			fakeMotion.which = map.mouse_which;
			fakeMotion.state = SDL_BUTTON(1);

			if (map.type == 0) {
				/* X-Axis */
				int x = 0;
				if (event->jaxis.value < 0) {
					x = (map.center - map.min) * (event->jaxis.value / 32767.0f);
				} else if (event->jaxis.value > 0) {
					x = (map.max - map.center) * (event->jaxis.value / 32767.0f);
				}

				int y = 0;
				int value = SDL_JoystickGetAxis(real_joystick, yaxis);
				if (value < 0) {
					y = (keymap.axis[yaxis].center - keymap.axis[yaxis].min) * (value / 32767.0f);
				} else if (value > 0) {
					y = (keymap.axis[yaxis].max - keymap.axis[yaxis].center) * (value / 32767.0f);
				}
				fakeMotion.x = map.center + x;
				fakeMotion.y = keymap.axis[yaxis].center + y;
			} else if (map.type == 1) {
				/* Y-Axis */
				int x = 0;
				int value = SDL_JoystickGetAxis(real_joystick, xaxis);
				if (value < 0) {
					x = (keymap.axis[xaxis].center - keymap.axis[xaxis].min) * (value / 32767.0f);
				} else if (value > 0) {
					x = (keymap.axis[xaxis].max - keymap.axis[xaxis].center) * (value / 32767.0f);
				}

				int y = 0;
				if (event->jaxis.value < 0) {
					y = (map.center - map.min) * (event->jaxis.value / 32767.0f);
				} else if (event->jaxis.value > 0) {
					y = (map.max - map.center) * (event->jaxis.value / 32767.0f);
				}
				fakeMotion.x = keymap.axis[xaxis].center + x;
				fakeMotion.y = map.center + y;
			}
			//hack
			if (xaxis == yaxis)
				fakeMotion.x = 720;

			fakeMotion.xrel = fakeMotion.x - last_mouse[map.mouse_which].x;
			fakeMotion.yrel = fakeMotion.y - last_mouse[map.mouse_which].y;

			if ((last_mouse[map.mouse_which].x == keymap.axis[xaxis].center &&
					last_mouse[map.mouse_which].y == keymap.axis[yaxis].center) ||
					(xaxis == yaxis &&
					 last_mouse[map.mouse_which].y == keymap.axis[yaxis].center)) {
				SDL_MouseButtonEvent fakeButton;
				fakeButton.button = SDL_BUTTON_LEFT;
				fakeButton.which = map.mouse_which;
				fakeButton.type = SDL_MOUSEBUTTONDOWN;
				fakeButton.state = SDL_PRESSED;
				fakeButton.x = last_mouse[map.mouse_which].x;
				fakeButton.y = last_mouse[map.mouse_which].y;
				//hack
				if (xaxis == yaxis)
					fakeButton.x = 720;
				filter_mouse_button(&fakeButton);
			}
			filter_mouse_motion(&fakeMotion);

			last_mouse[map.mouse_which].x = fakeMotion.x;
			last_mouse[map.mouse_which].y = fakeMotion.y;
			return 0;
		}
		}
		return 0;
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		{
		SDL_MouseButtonEvent fakeButton;
		fakeButton.type = (event->type == SDL_JOYBUTTONDOWN ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP);
		fakeButton.button = SDL_BUTTON_LEFT;
		key_mapping key = keymap.jbuttons[event->jbutton.button];
		if (event->jbutton.button == key.keycode) {
			fakeButton.which = key.mouse_which;
			fakeButton.x = key.mouse_press.x;
			fakeButton.y = key.mouse_press.y;
			return filter_mouse_button(&fakeButton);
		}		
		}
		return 0;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		{
		/* prevent backspace button */
		if (event->key.keysym.sym == SDLK_BACKSPACE) {
			return 1;
		}

		/* five simultaneous touches can exist */

		SDL_MouseButtonEvent fakeButton;
		fakeButton.type = (event->type == SDL_KEYDOWN ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP);
		fakeButton.state = (event->type == SDL_KEYDOWN ? SDL_PRESSED : SDL_RELEASED);
		fakeButton.button = SDL_BUTTON_LEFT;

		key_mapping key = keymap.keys[event->key.keysym.sym];
		if (event->key.keysym.sym == key.keycode) {
			fakeButton.which = key.mouse_which;
			fakeButton.x = key.mouse_press.x;
			fakeButton.y = key.mouse_press.y;
			return filter_mouse_button(&fakeButton);
		}

		if (keymap.enable_dpad) {
			/* send the first keypress */
			fakeButton.which = keymap.dpad_center.mouse_which;
			fakeButton.x = keymap.dpad_center.mouse_press.x;
			fakeButton.y = keymap.dpad_center.mouse_press.y;

			int swipeX = 0;
			int swipeY = 0;
			Uint8 *keystate = SDL_GetKeyState(NULL);

			/* keys go clockwise, starting with 0 == north == w */
			if (event->key.keysym.sym == keymap.dpad_keys[0].keycode &&
					!keystate[keymap.dpad_keys[6].keycode] &&
					!keystate[keymap.dpad_keys[7].keycode] &&
					!keystate[keymap.dpad_keys[1].keycode] &&
					!keystate[keymap.dpad_keys[2].keycode]) {
				/* up only */
				swipeX = keymap.dpad_keys[0].mouse_press.x;
				swipeY = keymap.dpad_keys[0].mouse_press.y;
			}
			else if ((event->key.keysym.sym == keymap.dpad_keys[1].keycode) ||
					(event->key.keysym.sym == keymap.dpad_keys[0].keycode &&
							keystate[keymap.dpad_keys[2].keycode]) ||
					(event->key.keysym.sym == keymap.dpad_keys[2].keycode &&
							keystate[keymap.dpad_keys[0].keycode])) {
				/* up and right */
				swipeX = keymap.dpad_keys[1].mouse_press.x;
				swipeY = keymap.dpad_keys[1].mouse_press.y;
			}
			else if (event->key.keysym.sym == keymap.dpad_keys[2].keycode &&
					!keystate[keymap.dpad_keys[0].keycode] &&
					!keystate[keymap.dpad_keys[1].keycode] &&
					!keystate[keymap.dpad_keys[3].keycode] &&
					!keystate[keymap.dpad_keys[4].keycode]) {
				/* right only */
				swipeX = keymap.dpad_keys[2].mouse_press.x;
				swipeY = keymap.dpad_keys[2].mouse_press.y;
			}
			else if ((event->key.keysym.sym == keymap.dpad_keys[3].keycode) ||
					(event->key.keysym.sym == keymap.dpad_keys[2].keycode &&
						 	keystate[keymap.dpad_keys[4].keycode]) ||
					(event->key.keysym.sym == keymap.dpad_keys[4].keycode &&
							keystate[keymap.dpad_keys[2].keycode])) {
				/* down and right */
				swipeX = keymap.dpad_keys[3].mouse_press.x;
				swipeY = keymap.dpad_keys[3].mouse_press.y;
			}
			else if (event->key.keysym.sym == keymap.dpad_keys[4].keycode &&
					!keystate[keymap.dpad_keys[6].keycode] &&
					!keystate[keymap.dpad_keys[2].keycode] &&
					!keystate[keymap.dpad_keys[5].keycode] &&
					!keystate[keymap.dpad_keys[3].keycode]) {
				/* down only */
				swipeX = keymap.dpad_keys[4].mouse_press.x;
				swipeY = keymap.dpad_keys[4].mouse_press.y;
			}
			else if ((event->key.keysym.sym == keymap.dpad_keys[5].keycode) ||
					(event->key.keysym.sym == keymap.dpad_keys[6].keycode &&
							keystate[keymap.dpad_keys[4].keycode]) ||
					(event->key.keysym.sym == keymap.dpad_keys[4].keycode &&
							keystate[keymap.dpad_keys[6].keycode])) {
				/* down and left */
				swipeX = keymap.dpad_keys[5].mouse_press.x;
				swipeY = keymap.dpad_keys[5].mouse_press.y;
			}
			else if (event->key.keysym.sym == keymap.dpad_keys[6].keycode &&
					!keystate[keymap.dpad_keys[7].keycode] &&
					!keystate[keymap.dpad_keys[0].keycode] &&
					!keystate[keymap.dpad_keys[5].keycode] &&
					!keystate[keymap.dpad_keys[4].keycode]) {
				/* left only */
				swipeX = keymap.dpad_keys[6].mouse_press.x;
				swipeY = keymap.dpad_keys[6].mouse_press.y;
			}
			else if ((event->key.keysym.sym == keymap.dpad_keys[7].keycode) ||
					(event->key.keysym.sym == keymap.dpad_keys[0].keycode &&
							keystate[keymap.dpad_keys[6].keycode]) ||
					(event->key.keysym.sym == keymap.dpad_keys[6].keycode &&
							keystate[keymap.dpad_keys[0].keycode])) {
				/* up and left */
				swipeX = keymap.dpad_keys[7].mouse_press.x;
				swipeY = keymap.dpad_keys[7].mouse_press.y;
			}

			filter_mouse_button(&fakeButton);

			/* check if valid button was set */
			/* really asking for 0 is probably unlikely */
			if (swipeX > 0 && swipeY > 0) {
				SDL_MouseMotionEvent fakeMotion;
				fakeMotion.type = SDL_MOUSEMOTION;
				fakeMotion.which = fakeButton.which;
				fakeMotion.state = SDL_BUTTON(1);
				fakeMotion.x = swipeX;
				fakeMotion.y = swipeY;
				fakeMotion.xrel = swipeX - fakeButton.x;
				fakeMotion.yrel = swipeY - fakeButton.y;
				return filter_mouse_motion(&fakeMotion);
			}
		}
		return 1;
	}
	default:
		if (app_filter) return app_filter(event);
	}
	return 1;
}

/** Recalculate scaling multipliers */
void SDLPRE_RefreshScale()
{
	static const float EPSILON = 0.01;
	switch (s_rotate) {
		case PDL_ORIENTATION_0:
		case PDL_ORIENTATION_180:
			s_scale.x = (float)r_size.w / v_size.w + EPSILON;
			s_scale.y = (float)r_size.h / v_size.h + EPSILON;
			break;
		case PDL_ORIENTATION_90:
		case PDL_ORIENTATION_270:
			s_scale.x = (float)r_size.w / v_size.h + EPSILON;
			s_scale.y = (float)r_size.h / v_size.w + EPSILON;
			break;
	}
}

int SDL_Init(Uint32 flags)
{
	OVERRIDES(SDL_Init, (Uint32 flags), int);
	TRACE("Called sdl_init with flags = 0x%x", flags);

	flags |= SDL_INIT_JOYSTICK; /* We always want to try to get joysticks */

	int res = SUPER(flags);

	TRACE("Number of joysticks %d", SDL_NumJoysticks());
	int x;
	for (x = 0; x < SDL_NumJoysticks(); x++) {
		TRACE("Joystick %d named %s", x, SDL_JoystickName(x));
	}
	if (SDL_NumJoysticks() > 1) {
		real_joystick = SDL_JoystickOpen(1);
		SDL_JoystickEventState(SDL_ENABLE);
	}

	if (res == 0) {
		/* Success, hook some stuff. */
		SDL_ShowCursor(SDL_DISABLE); /* Pre disables cursor by default. */
		SDL_SetEventFilter(&filter_event);
		if (flags & SDL_INIT_VIDEO) {
			X11_Init();
		}
		/* necessary for ancient frog */
		PDL_EnsureInit();
	}

	return res;
}

void SDL_Quit()
{
	OVERRIDES(SDL_Quit, (), void);
	TRACE("Called SDL_Quit");
	if (real_joystick && SDL_JoystickOpened(1)) {
		SDL_JoystickClose(real_joystick);
	}
	gles_quit();
	v_screen = NULL;
	app_active = SDL_FALSE;
	app_filter = NULL;
	screen = NULL;
	SUPER();
}

int SDL_GL_SetAttribute(SDL_GLattr attr, int value)
{
	gles_init();
	switch (attr) {
		case SDL_GL_CONTEXT_MAJOR_VERSION:
			TRACE("Requested GL major version %d", value);
			switch (value) {
				case 1:
					desired_version = SDL_GLES_VERSION_1_1;
					return 0;
				case 2:
					desired_version = SDL_GLES_VERSION_2_0;
					return 0;
				default:
					SDL_SetError("Invalid GLES major version: %d", value);
					return -1;
			}
			break;
		case SDL_GL_CONTEXT_MINOR_VERSION:
			TRACE("Requested GL minor version %d (ignored)", value);
			return 0;
		case SDL_GL_RED_SIZE:
			TRACE("Ignoring color buffer red size %d", value);
			return 0;
		case SDL_GL_GREEN_SIZE:
			TRACE("Ignoring color buffer green size %d", value);
			return 0;
		case SDL_GL_BLUE_SIZE:
			TRACE("Ignoring color buffer blue size %d", value);
			return 0;
		case SDL_GL_ALPHA_SIZE:
			TRACE("Ignoring color buffer alpha size %d", value);
			return 0;
		case SDL_GL_DOUBLEBUFFER:
			TRACE("Ignoring doublebuffer preference %d", value);
			/* I wouldn't know how to handle it either way. */
			return 0;
		case SDL_GL_DEPTH_SIZE:
			TRACE("Requested depth buffer size %d", value);
			SDL_GLES_SetAttribute(SDL_GLES_DEPTH_SIZE, value);
			return 0;
		case SDL_GL_STENCIL_SIZE:
			TRACE("Requested stencil buffer size %d", value);
			SDL_GLES_SetAttribute(SDL_GLES_STENCIL_SIZE, value);
			return 0;
		default:
			WARN("Application asks for unknown GL attribute %u, value %d", attr, value);
			SDL_SetError("Unknown GL attribute");
			return 0;
	}
}

int SDL_GL_GetAttribute(SDL_GLattr attr, int *value)
{
	gles_init();
	if (!value) {
		SDL_SetError("Null value");
		return -1;
	}
	switch (attr) {
		case SDL_GL_CONTEXT_MAJOR_VERSION:
			switch (desired_version) {
				case SDL_GLES_VERSION_1_1: *value = 1; break;
				case SDL_GLES_VERSION_2_0: *value = 2; break;
				default:                   *value = 0; break;
			}
			return 0;
		case SDL_GL_CONTEXT_MINOR_VERSION:
			switch (desired_version) {
				case SDL_GLES_VERSION_1_1: *value = 1; break;
				case SDL_GLES_VERSION_2_0: *value = 0; break;
				default:                   *value = 0; break;
			}
			return 0;
		case SDL_GL_RED_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_RED_SIZE, value);
		case SDL_GL_GREEN_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_GREEN_SIZE, value);
		case SDL_GL_BLUE_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_BLUE_SIZE, value);
		case SDL_GL_ALPHA_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_ALPHA_SIZE, value);
		case SDL_GL_DOUBLEBUFFER:
			*value = 0;
			return 0;
		case SDL_GL_DEPTH_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_DEPTH_SIZE, value);
		case SDL_GL_STENCIL_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_STENCIL_SIZE, value);
		default:
			WARN("Application asks for value of unknown GL attribute %u", attr);
			SDL_SetError("Unknown GL attribute");
			return 0;
	}
}

SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
	OVERRIDES(SDL_SetVideoMode, (int width, int height, int bpp, Uint32 flags), SDL_Surface*);

	TRACE("Called SetVideoMode(%d, %d, %d, 0x%x)", width, height, bpp, flags);

	if (flags & SDL_OPENGL) {
		TRACE("Requested OpenGL video mode");
		flags &= ~SDL_OPENGL; /* Our platform SDL doesn't handle it. */

		int res = gles_init();
		if (res != 0 ) return NULL;

		if (!context) {
			context = SDL_GLES_CreateContext();
			if (!context) return NULL;
		}

		/* Always create a GL surface the size of the native screen, as per the Pre. */
		screen = SUPER(r_size.w, r_size.h, r_bpp, flags | SDL_FULLSCREEN);
		if (!screen) return NULL;

		SDLPRE_RefreshScale();

		res = SDL_GLES_MakeCurrent(context);
		if (res != 0) return NULL;

		app_active = SDL_TRUE;

		if (r_size.w != v_size.w || r_size.h != v_size.h) {
			TRACE("Creating shadow surface (w=%d, h=%d)", v_size.w, v_size.h);
			/* Assuming v_size <= r_size */
			v_screen = SDL_ConvertSurface(screen, screen->format, screen->flags);
			v_screen->w = v_size.w;
			v_screen->h = v_size.h;
		} else {
			v_screen = screen;
		}

		return v_screen;
	} else {
		if (gles_inited) {
			gles_quit();
		}

		screen = SUPER(width, height, bpp, flags);

		WARN("Cannot yet scale non OpenGL applications");

		app_active = SDL_TRUE;

		return screen;
	}
}

int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags)
{
	OVERRIDES(SDL_VideoModeOK, (int width, int height, int bpp, Uint32 flags), int);
	TRACE("Asked if video mode %dx%dx%d (flags=0x%x) is ok",
		width, height, bpp, flags);
	flags &= ~SDL_OPENGL;
	int res = SUPER(width, height, bpp, flags);
	if (res == 0) {
		WARN("It isn't");
	}
	return res;
}

const SDL_VideoInfo* SDL_GetVideoInfo(void)
{
	OVERRIDES(SDL_GetVideoInfo, (void), const SDL_VideoInfo*);
	static SDL_VideoInfo info;
	const SDL_VideoInfo* sdl_info = SUPER();
	info = *sdl_info;

	/* Lie. */
	info.current_w = v_size.w;
	info.current_h = v_size.h;
	TRACE("Reporting video information width=%u, height=%u",
		info.current_w, info.current_h);

	return &info;
}

SDL_Surface* SDL_GetVideoSurface(void)
{
	return v_screen;
}

void SDL_GL_SwapBuffers(void)
{
	if (!app_active) {
		/* Let's cap the framerate a bit when the application is not active */
		SDL_Delay(1000 / DEACTIVATED_FPS_CAP);
	}
	SDL_GLES_SwapBuffers();
}

void SDL_SetEventFilter(SDL_EventFilter filter)
{
	OVERRIDES(SDL_SetEventFilter, (SDL_EventFilter filter), void);
	if (filter != &filter_event) {
		TRACE("SetEventFilter(%p)", filter);
		app_filter = filter;
	}
	SUPER(filter);
}

Uint8 SDL_GetAppState(void)
{
	if (app_active)
		return SDL_APPMOUSEFOCUS | SDL_APPINPUTFOCUS | SDL_APPACTIVE;
	else
		return 0;
}

Uint8 SDL_GetRelativeMouseState(int *x, int *y)
{
	OVERRIDES(SDL_GetRelativeMouseState, (int *x, int *y), Uint8);
	int sx, sy;
	Uint8 state = SUPER(&sx, &sy);
	struct point p = { sx, sy };
	p_unscale(&p);
	if (x) *x = p.x;
	if (y) *y = p.y;
	return state;
}

Uint8 SDL_GetMouseState(int *x, int *y)
{
	OVERRIDES(SDL_GetMouseState, (int *x, int *y), Uint8);
	int sx, sy;
	Uint8 state = SUPER(&sx, &sy);
	struct point p = { sx, sy };
	p_unscale(&p);
	if (x) *x = p.x;
	if (y) *y = p.y;
	return state;
}

Uint8 SDL_GetRelativeMultiMouseState(int which, int *x, int *y)
{
	if (which == 0) return SDL_GetRelativeMouseState(x, y);
	return 0;
}

Uint8 SDL_GetMultiMouseState(int which, int *x, int *y)
{
	if (which == 0) return SDL_GetMouseState(x, y);
	return 0;
}

int SDL_UpperBlit(SDL_Surface *src, SDL_Rect *srcrect,
			 SDL_Surface *dst, SDL_Rect *dstrect)
{
	OVERRIDES(SDL_UpperBlit, (SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect), int);
	if (dst == screen || src == screen) {
		WARN("Someone is trying to blit from/to screen; this breaks the fourth wall");
	}
	return SUPER(src, srcrect, dst, dstrect);
}

void glLoadIdentity()
{
	OVERRIDES(glLoadIdentity, (void), void);
	SUPER();

	/* glGet calls might be costly. Skip if possible. */
	if (s_rotate == PDL_ORIENTATION_0) return;

	GLint mode;
	glGetIntegerv(GL_MATRIX_MODE, &mode);
	if (mode == GL_PROJECTION) {
		switch (s_rotate) {
		case PDL_ORIENTATION_0:
			break;
		case PDL_ORIENTATION_90:
			glRotatef(90.0f, 0.0, 0.0, -1.0f);
			break;
		case PDL_ORIENTATION_180:
			glRotatef(180.0f, 0.0, 0.0, -1.0f);
			break;
		case PDL_ORIENTATION_270:
			glRotatef(270.0f, 0.0, 0.0, -1.0f);
			break;
		default:
			break;
		}
	}
}

void glLoadMatrixf(const GLfloat * m)
{
	glLoadIdentity();
	glMultMatrixf(m);
}

void glLoadMatrixx(const GLfixed * m)
{
	glLoadIdentity();
	glMultMatrixx(m);
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	OVERRIDES(glViewport, (GLint x, GLint y, GLsizei width, GLsizei height), void);
/*
	TRACE("Called glViewport %dx%d", width, height);
*/
	if (width == r_size.w && height == r_size.h) {
		width = v_size.w;
		height = v_size.h;
	}
	if (width == 0 || height == 0) {
		width = v_size.w;
		height = v_size.h;
	}
	struct point p1 = { x, y };
	struct point p2 = { x + width, y + height };
	struct box b;
	p_scale(&p1);
	p_scale(&p2);
	make_box(&b, &p1, &p2);
	SUPER(b.x, b.y, b.w, b.h);
}

void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	OVERRIDES(glScissor, (GLint x, GLint y, GLsizei width, GLsizei height), void);
	struct point p1 = { x, y };
	struct point p2 = { x + width, y + height };
	struct box b;
	p_scale(&p1);
	p_scale(&p2);
	make_box(&b, &p1, &p2);
	SUPER(b.x, b.y, b.w, b.h);
}

void glTexImage2D(GLenum target, GLint level, GLint internalFormat,
	GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data)
{
	OVERRIDES(glTexImage2D, (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data), void);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	if (internalFormat != format) {
		internalFormat = format;
	}
	SUPER(target, level, internalFormat, width, height, border, format, type, data);
}
/*
void *dlopen(const char *filename, int flag)
{
	OVERRIDES(dlopen, (const char *filename, int flag), void *);

	flag = RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND;

	void *handle = SUPER(filename, flag);
	TRACE("Called dlopen %s, flag 0x%x = handle %p", filename, flag, handle);
	return handle;
}

extern void * __real_dlsym(void *, const char *);
void *__wrap_dlsym(void *handle, const char *symbol)
{
	TRACE("Called dlsym %p, %s", handle, symbol);
	return __real_dlsym(handle, symbol);
}

int dlclose(void *handle)
{
	OVERRIDES(dlclose, (void *handle), int);
	TRACE("Called dlclose on handle %p", handle);
	return SUPER(handle);
}

EGLContext eglCreateContext(EGLDisplay display, EGLConfig config, EGLContext share_context, EGLint const * attrib_list)
{
	OVERRIDES(eglCreateContext, (EGLDisplay display, EGLConfig config, EGLContext share_context, EGLint const * attrib_list), EGLContext);
	TRACE("Called eglCreateContext");
	return SUPER(display, config, share_context, attrib_list);
}

EGLSurface eglCreateWindowSurface(EGLDisplay display, EGLConfig config, NativeWindowType native_window, EGLint const * attrib_list)
{
	OVERRIDES(eglCreateWindowSurface, (EGLDisplay display, EGLConfig config, NativeWindowType native_window, EGLint const * attrib_list), EGLSurface);
	TRACE("Called eglCreateWindowSurface");
	return SUPER(display, config, native_window, attrib_list);
}

extern void gles2Rotatef(float, float, float, float);

EGLBoolean eglMakeCurrent(EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext context)
{
	OVERRIDES(eglMakeCurrent, (EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext context), EGLBoolean);
	TRACE("Called eglMakeCurrent");
	EGLBoolean retval = SUPER(display, draw, read, context);
	return retval;
}

EGLBoolean eglSwapBuffers(EGLDisplay display, EGLSurface surface)
{
	OVERRIDES(eglSwapBuffers, (EGLDisplay display, EGLSurface surface), EGLBoolean);
	EGLBoolean retval = SUPER(display, surface);
	return retval;
}

EGLSurface eglGetCurrentSurface(EGLint readdraw)
{
	OVERRIDES(eglGetCurrentSurface, (EGLint readdraw), EGLSurface);
	TRACE("Called eglGetCurrentSurface");
	return SUPER(readdraw);
}

EGLBoolean eglInitialize(EGLDisplay display, EGLint * major, EGLint * minor)
{
	OVERRIDES(eglInitialize, (EGLDisplay display, EGLint * major, EGLint * minor), EGLBoolean);
	EGLBoolean retval = SUPER(display, major, minor);
	TRACE("Called eglInitialize %d.%d", *major, *minor);
	return retval;
}

EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list)
{
	OVERRIDES(eglCreatePbufferFromClientBuffer, (EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list), EGLSurface);
	TRACE("Called eglCreatePbufferFromClientBuffer");
	return SUPER(dpy, buftype, buffer, config, attrib_list);
}

EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
	OVERRIDES(eglCreatePbufferSurface, (EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list), EGLSurface);
	TRACE("Called eglCreatePbufferSurface");
	return SUPER(dpy, config, attrib_list);
}

EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list)
{
	OVERRIDES(eglCreatePixmapSurface, (EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list), EGLSurface);
	TRACE("Called eglCreatePixmapSurface");
	return SUPER(dpy, config, pixmap, attrib_list);
}
*/
