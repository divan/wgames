#include <SDL.h>
#include <glib.h>

#include "SDL_haptic.h"
#include "override.h"
#include "config.h"
#include "private.h"
#include "debug.h"

#if USE_MCE_VIBRA
#include <dbus/dbus-glib.h>
#include <mce/dbus-names.h>
#endif

#define DEBUG_DOMAIN    "VIBRA"

struct _SDL_Haptic
{
#if USE_MCE_VIBRA
	DBusGProxy *mce_proxy;
#endif
	SDL_HapticConstant effect;
    char opened;
};

static SDL_Haptic vibra = { 0 };

#if USE_MCE_VIBRA
#define SDL_LEVEL_MIN ( 32767)
#define SDL_LEVEL_MAX (-32767)
#define MCE_LEVEL_MIN (-255)
#define MCE_LEVEL_MAX ( 255)
#define MCE_LENGTH_INFINITY (0)
#define MCE_LENGTH_MAX      (32767)

static inline Sint16 scale_level(Sint16 l)
{
	Sint16 s = l / 192;
	/* Clamp value */
	if (s > MCE_LEVEL_MAX) return MCE_LEVEL_MAX;
	if (s < MCE_LEVEL_MIN) return MCE_LEVEL_MIN;
	return s;
}
#endif

int
SDL_HapticInit(void)
{
	return 0;
}

int
SDL_NumHaptics(void)
{
    return 1;
}

const char *
SDL_HapticName(int device_index)
{
	if (device_index != 0) {
		SDL_SetError("There is no such haptic device");
		return NULL;
	}

	return "Vibrator";
}

SDL_Haptic *
SDL_HapticOpen(int device_index)
{
	if (device_index == 0) {
		if (vibra.opened) {
			WARN("Vibrator is already opened");
			return &vibra;
		}
#if USE_MCE_VIBRA
		/* We need the glib thread */
		if (PDL_EnsureInit() != PDL_NOERROR) {
			return NULL;
		}
		TRACE("Opening MCE vibrator device");
		GError *error = NULL;
		DBusGConnection *bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
		if (!bus) {
			WARN("Failed to get system bus: %s", error->message);
			g_error_free(error);
			return NULL;
		}
		vibra.mce_proxy = dbus_g_proxy_new_for_name(bus,
			MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF);
#endif
		return &vibra;
	} else {
		SDL_SetError("There is no such haptic device");
		return NULL;
	}
}

int
SDL_HapticOpened(int device_index)
{
	if (device_index == 0)
		return vibra.opened;
	else
		return SDL_FALSE;
}

int
SDL_HapticIndex(SDL_Haptic * haptic)
{
	if (haptic == &vibra)
		return 0;
	else
		return -1;
}

int
SDL_MouseIsHaptic(void)
{
    return SDL_FALSE;
}

SDL_Haptic *
SDL_HapticOpenFromMouse(void)
{
	return NULL;
}

void
SDL_HapticClose(SDL_Haptic * haptic)
{
	if (haptic == &vibra) {
#if USE_MCE_VIBRA
		g_object_unref(vibra.mce_proxy);
#endif
		vibra.opened = FALSE;
	}
}

void
SDL_HapticQuit(void)
{

}

int
SDL_HapticNumEffects(SDL_Haptic * haptic)
{
	return 1;
}

int
SDL_HapticNumEffectsPlaying(SDL_Haptic * haptic)
{
	return 1;
}

unsigned int
SDL_HapticQuery(SDL_Haptic * haptic)
{
	if (haptic == &vibra)
		return SDL_HAPTIC_CONSTANT;
	else
	    return 0;
}

int
SDL_HapticNumAxes(SDL_Haptic * haptic)
{
	if (haptic == &vibra)
		return 1;
	else
	    return 0;
}

int
SDL_HapticEffectSupported(SDL_Haptic * haptic, SDL_HapticEffect * effect)
{
	SDL_HapticConstant *c = &effect->constant;
	/* TODO: Properly detect we do not support triggers, attack or fade. */
	return c->type == SDL_HAPTIC_CONSTANT;
}

int
SDL_HapticNewEffect(SDL_Haptic * haptic, SDL_HapticEffect * effect)
{
	if (haptic != &vibra) {
		SDL_SetError("Invalid haptic device");
		return -1;
	}
	if (!SDL_HapticEffectSupported(haptic, effect)) {
		SDL_SetError("Not supported");
		return -1;
	}

	vibra.effect = effect->constant;

	return 0;
}

int
SDL_HapticUpdateEffect(SDL_Haptic * haptic, int effect,
                       SDL_HapticEffect * data)
{
	if (haptic != &vibra) {
		SDL_SetError("Invalid haptic device");
		return -1;
	}
	if (effect != 0) {
		SDL_SetError("Invalid effect id %d", effect);
		return -1;
	}
	if (!SDL_HapticEffectSupported(haptic, data)) {
		SDL_SetError("Not supported");
		return -1;
	}

	vibra.effect = data->constant;

	return 0;
}

int
SDL_HapticRunEffect(SDL_Haptic * haptic, int effect, Uint32 iterations)
{
	if (haptic != &vibra) {
		SDL_SetError("Invalid haptic device");
		return -1;
	}
	if (effect != 0) {
		SDL_SetError("Invalid effect id %d", effect);
		return -1;
	}

	Sint16 level = vibra.effect.level;
	Uint32 length = vibra.effect.length;

	if (length == SDL_HAPTIC_INFINITY || iterations == SDL_HAPTIC_INFINITY) {
		length = SDL_HAPTIC_INFINITY;
	} else {
		length *= iterations;
	}

#if USE_MCE_VIBRA
	gint speed = scale_level(level);
	gint duration;
	if (length == SDL_HAPTIC_INFINITY) {
		duration = MCE_LENGTH_INFINITY;
	} else if (length > MCE_LENGTH_MAX) {
		duration = MCE_LENGTH_MAX;
	} else {
		duration = length;
	}

	dbus_g_proxy_call_no_reply(vibra.mce_proxy, MCE_START_MANUAL_VIBRATION,
		G_TYPE_INT, speed, G_TYPE_INT, duration, G_TYPE_INVALID,
		G_TYPE_INVALID);
#endif

	return 0;
}

int
SDL_HapticStopEffect(SDL_Haptic * haptic, int effect)
{
	if (haptic != &vibra) {
		SDL_SetError("Invalid haptic device");
		return -1;
	}
	if (effect != 0) {
		SDL_SetError("Invalid effect id %d", effect);
		return -1;
	}

#if USE_MCE_VIBRA
	dbus_g_proxy_call_no_reply(vibra.mce_proxy, MCE_STOP_MANUAL_VIBRATION,
		G_TYPE_INVALID, G_TYPE_INVALID);
#endif

	return 0;
}

void
SDL_HapticDestroyEffect(SDL_Haptic * haptic, int effect)
{

}

int
SDL_HapticGetEffectStatus(SDL_Haptic * haptic, int effect)
{
	return 0;
}

int
SDL_HapticSetGain(SDL_Haptic * haptic, int gain)
{
	SDL_SetError("Not supported");
	return -1;
}

int
SDL_HapticSetAutocenter(SDL_Haptic * haptic, int autocenter)
{
	SDL_SetError("Not supported");
	return -1;
}

int
SDL_HapticPause(SDL_Haptic * haptic)
{
	SDL_SetError("Not supported");
	return -1;
}

int
SDL_HapticUnpause(SDL_Haptic * haptic)
{
	SDL_SetError("Not supported");
	return -1;
}

int
SDL_HapticStopAll(SDL_Haptic * haptic)
{
    return 0;
}

int
SDL_HapticMinDuration(SDL_Haptic * haptic)
{
   return 0;
}

int
SDL_HapticMaxDuration(SDL_Haptic * haptic)
{
   return 2000;
}

