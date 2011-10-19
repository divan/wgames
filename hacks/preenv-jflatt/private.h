#ifndef _PRIVATE_H_
#define _PRIVATE_H_

#include <SDL.h>
#include <PDL.h>

struct size {
	unsigned int w, h;
};

struct point {
	unsigned int x, y;
};

struct box {
	unsigned int x, y;
	unsigned int w, h;
};

struct pointf {
	float x, y;
};

#pragma GCC visibility push(hidden)
/** The real SDL screen surface. */
extern SDL_Surface *screen;

/** Real screen size. */
extern struct size r_size;

/** Real screen bpp. */
extern unsigned int r_bpp;

/** Virtual screen size, used as base for scaling. */
extern struct size v_size;

/** Rotation to apply while scaling. */
extern PDL_Orientation s_rotate;

/** Calculated scaling.
    Use SDLPRE_RefreshScale() to recalculate if changing v_size, r_size or
    s_rotate. */
extern struct pointf s_scale;

/** Recalculate s_scale. */
void SDLPRE_RefreshScale();

/** Push a SDL event but calling the application's event filter. */
void SDLPRE_PushEvent(SDL_Event *event);

PDL_Err PDL_EnsureInit();
void PDL_NotifyFocus(SDL_bool focus);

void X11_Init();
void X11_SetDoNotDisturb(SDL_bool set);

extern SDL_Joystick *real_joystick;
#pragma GCC visibility pop

#endif
