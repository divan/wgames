#ifndef CONFIG_H
#define CONFIG_H

// Application related defines
#ifdef Q_WS_MAEMO_5
    #define BASE_DIR "/usr/share/wgames"
    #define TAR_APP "/usr/bin/gnu/tar"
#else
    #define BASE_DIR "/tmp/wgames"
    #define TAR_APP "/bin/tar"
#endif

#define GRAPHICS_DIR BASE_DIR"/graphics/"
#define GAMES_DATA_DIR BASE_DIR"/gamesinfo/ini/"
#define GAMES_IMAGES_DIR BASE_DIR"/gamesinfo/images/"
#define GAMES_HACKS_DIR BASE_DIR"/gamesinfo/hacks/"
#define GAMES_LIB_DIR BASE_DIR"/lib/"

// Game installing defines
#ifdef Q_WS_MAEMO_5
    #define USER_DIR "/home/user"
#else
    #define USER_DIR "/tmp/wgames/home"
#endif

#define MYDOCS_DIR USER_DIR"/MyDocs"

#define DATA_DIR_ROOT MYDOCS_DIR"/Games"
#define BIN_DIR_ROOT USER_DIR"/Games"
#define SAVES_DIR_ROOT USER_DIR"/Games/Saves"

#define DESKTOP_ENTRY_PATH USER_DIR"/.local/share/applications/hildon/"
#define ICONS_PATH USER_DIR"/.local/share/icons/hicolor/scalable/hildon/"
#define DBUS_PATH USER_DIR"/.local/share/dbus-1/services/"
#define DOWNLOADS_DIR MYDOCS_DIR"/Downloads/"

#define PREVIEW_WIDTH 450
#define PREVIEW_HEIGHT 150

#define ORGANIZATION_NAME "PalmPre"
#define APPLICATION_NAME "Manager"

enum UserRoles {
    UserRoleName = Qt::UserRole,
    UserRoleIndex, // index of _games array
    UserRoleTitle,
    UserRoleDescription,
    UserRoleGenre,
    UserRoleURL,
    UserRolePreviewImage,
    UserRoleStatus,
    UserRoleSpace,
    UserRoleSaveTimestamp,
};

#ifdef Q_WS_MAEMO_5
#define TRACKER_PATH "/home/user/.config/tracker/tracker.cfg"
#else
#define TRACKER_PATH "/tmp/tracker.cfg"
#endif

#endif /* CONFIG_H */
