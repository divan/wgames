#include <sys/utsname.h>
#include <unistd.h>
#include <glib.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "private.h"
#include "debug.h"

#define DEBUG_DOMAIN "X11"

static Display *display;
static Window wmwindow;
static Window fswindow;

#define ATOM_COUNT 3
static char *atom_names[] = {
	"_NET_WM_PID",
	"WM_DELETE_WINDOW",
	"_HILDON_DO_NOT_DISTURB",
	NULL
};
static Atom atoms[ATOM_COUNT];
#define ATOM_WM_PID atoms[0]
#define ATOM_WM_DELETE_WINDOW atoms[1]
#define ATOM_HILDON_DO_NOT_DISTURB atoms[2]

void X11_Init()
{
	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version);

	if ( SDL_GetWMInfo(&wminfo) ) {
		display = wminfo.info.x11.display;
		fswindow = wminfo.info.x11.fswindow;
		wmwindow = wminfo.info.x11.wmwindow;
	} else {
		WARN("Failed to get window ids from SDL");
		return;
	}

	XInternAtoms(display, atom_names, ATOM_COUNT, False, atoms);

	/* This will set WM_CLIENT_MACHINE and WM_LOCALE_NAME */
	XSetWMProperties(display, fswindow, NULL, NULL, NULL, 0, NULL, NULL, NULL);
	XSetWMProperties(display, wmwindow, NULL, NULL, NULL, 0, NULL, NULL, NULL);

	/* Set the pid for this window */
	unsigned long pid = getpid();
	XChangeProperty(display, fswindow, ATOM_WM_PID, XA_CARDINAL, 32,
		PropModeReplace, (unsigned char *)&pid, 1);
	XChangeProperty(display, wmwindow, ATOM_WM_PID, XA_CARDINAL, 32,
		PropModeReplace, (unsigned char *)&pid, 1);
	TRACE("PID is %lu", pid);

	Atom prots[] = { ATOM_WM_DELETE_WINDOW };
	XSetWMProtocols(display, fswindow, prots, 1);

	XClassHint classhint = { "preenv", "preenv" };
	XSetClassHint(display, fswindow, &classhint);
}

void X11_SetDoNotDisturb(SDL_bool set)
{
	if (set) {
		long state = 1;
		XChangeProperty(display, wmwindow, ATOM_HILDON_DO_NOT_DISTURB,
			XA_INTEGER, 32, PropModeReplace,
			(unsigned char *) &state, 1);
		XChangeProperty(display, fswindow, ATOM_HILDON_DO_NOT_DISTURB,
			XA_INTEGER, 32, PropModeReplace,
			(unsigned char *) &state, 1);
	} else {
		XDeleteProperty(display, wmwindow, ATOM_HILDON_DO_NOT_DISTURB);
		XDeleteProperty(display, fswindow, ATOM_HILDON_DO_NOT_DISTURB);
	}
}

