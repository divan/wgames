#ifndef CONFIG_H

#define LIB_VERSION			"0.1.6"

#define USE_MCE_VIBRA       1

#define USE_LIBLOCATION     1

#define DEACTIVATED_FPS_CAP	2

#define MAX_KEYS 306
#define MAX_JOYBUTTONS 17
#define MAX_AXIS 20

typedef struct {
	int x;
	int y;
} point;

typedef struct {
	int keycode;
	int mouse_which;
	point mouse_press;
} key_mapping;

typedef struct {
	int axis;
	int mouse_which;
	int type; /* 0 for X, 1 for Y */
	int center;
	int min;
	int max;
} axis_mapping;

struct {
	int enable_dpad;
	key_mapping dpad_center;
	key_mapping dpad_keys[8];
	key_mapping keys[MAX_KEYS];
	key_mapping jbuttons[MAX_JOYBUTTONS];
	axis_mapping axis[MAX_AXIS];
} keymap;

/*
There are probably better ways to do this, but I figure this would
take lookups less time at the expense of memory wasted.  Create a huge
array of keys to fit the range of keys available
This is my keycode list, other locales may be different:
313 - blue fn arrow, highest value
306 - ctrl
304 - shift
276 - left arrow
275 - right arrow
274 - down arrow
273 - up arrow
271 - return
122 - z, ordered alphabetically from here
...
97 - a
46 - period, colon
44 - comma, semicolon
32 - space bar, use this for lowest
8 - backspace, will need disabled
*/

#endif
