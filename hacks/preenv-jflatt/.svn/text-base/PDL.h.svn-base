#ifndef _PDL_H_
#define _PDL_H_

#include <SDL_stdinc.h>
#include "SDLGL.h"

#define PDL_FALSE SDL_FALSE
#define PDL_TRUE SDL_TRUE

#define PDL_GPS_UPDATE 0xE100
#define PDL_GPS_FAILURE 0xE101

typedef enum {
	PDL_NOERROR = 0,
	PDL_EMEMORY = 1,
	PDL_ECONNECTION = 2,
	PDL_INVALIDINPUT = 3,
	PDL_EOTHER = 4,
	PDL_UNINIT = 5,
	PDL_NOTALLOWED = 6,
	PDL_LICENSEFAILURE = 7,
	PDL_STRINGTOOSMALL = 8,
	PDL_SYSTEMERROR_FILE = 9,
	PDL_SYSTEMERROR_NET = 10,
	PDL_APPINFO = 11
} PDL_Err;

typedef enum {
	PDL_ORIENTATION_0 = 0,
	PDL_ORIENTATION_90 = 1,
	PDL_ORIENTATION_180 = 2,
	PDL_ORIENTATION_270 = 3
} PDL_Orientation;

typedef enum {
	PDL_AGGRESSION_LESSTOUCHES = 0,
	PDL_AGGRESSION_MORETOUCHES = 1
} PDL_TouchAggression;

typedef SDL_bool PDL_bool;

typedef struct {
	double latitude;
	double longitude;
	double altitude;
	double horizontalAccuracy;
	double verticalAccuracy;
	double heading;
	double velocity;
} PDL_Location;

typedef struct {
	Uint32 ipaddress;
	Uint32 netmask;
	Uint32 broadcast;
} PDL_NetInfo;

typedef struct {
	int horizontalPixels;
	int verticalPixels;
	int horizontalDPI;
	int verticalDPI;
	double aspectRatio;
} PDL_ScreenMetrics;

typedef struct {
	int majorVersion;
	int minorVersion;
	int revision;
	const char * versionStr;
} PDL_OSVersion;

PDL_Err PDL_BannerMessagesEnable(PDL_bool enable);
PDL_Err PDL_CustomPauseUiEnable(PDL_bool enable);
PDL_Err PDL_EnableLocationTracking(PDL_bool activate);
PDL_Err PDL_GesturesEnable(PDL_bool enable);
PDL_Err PDL_GetCallingPath(char *buffer, int bufferLen);
PDL_Err PDL_GetDeviceName(char *buffer, int bufferLen);
const char *PDL_GetError(void);
const char *PDL_GetHardware(void);
PDL_Err PDL_GetLanguage(char *buffer, int bufferLen);
PDL_Err PDL_GetLocation(PDL_Location *location);
PDL_Err PDL_GetNetInfo(const char *interfaceName, PDL_NetInfo * interfaceInfo);
PDL_Err PDL_GetOSVersion(PDL_OSVersion *version);
PDL_Err PDL_GetScreenMetrics(PDL_ScreenMetrics* metrics);
PDL_Err PDL_GetUniqueID(char *buffer, int bufferLen);
PDL_bool PDL_IsPlugin();
PDL_Err PDL_Init(unsigned int flags);
PDL_Err PDL_LaunchBrowser(const char* url);
PDL_Err PDL_NotifyMusicPlaying(PDL_bool musicPlaying);
PDL_Err PDL_ScreenTimeoutEnable(PDL_bool enable);
PDL_Err PDL_SetFirewallPortStatus(int port, PDL_bool open);
PDL_Err PDL_SetOrientation(PDL_Orientation orientation);
PDL_Err PDL_SetTouchAggression(PDL_TouchAggression aggression);

int PDL_isAppLicensedForDevice(const char *appid);

PDL_Err PDLNet_Get_Info(const char *interfaceName, PDL_NetInfo * interfaceInfo);

#endif
