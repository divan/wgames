#include <SDL.h>
#include "PDL.h"

#include "config.h"
#include "private.h"

/* Some defaults. */
/** Your device's real screen size. */
struct size r_size = { 800, 480 };
/** Your device's depth. */
unsigned int r_bpp = 16;
/** Emulated device resolution. */
struct size v_size = { 320, 480 };
/** Rotation to apply before scaling. */
PDL_Orientation s_rotate = PDL_ORIENTATION_270;

/* Do not change this. */
SDL_Surface *screen = NULL;
struct pointf s_scale = { 1.0f, 1.0f };

SDL_Joystick *real_joystick = NULL;
