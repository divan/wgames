#include "gameinfo.h"

#include <QSettings>
#include <QScriptEngine>
#include <QDebug>
#include <QLocale>

#include "config.h"

GameInfo::GameInfo()
{
}

GameInfo::GameInfo(QString filename)
{
    loadFromFile(filename);
}

bool GameInfo::loadFromFile(QString filename)
{
    QString localeName = QLocale::system().name();
    QSettings settings(filename, QSettings::IniFormat);

    settings.setIniCodec("UTF-8");

    // Try to get localized parameter first,
    // then fallback
    _description = settings.value(
                settings.contains("Info/Description_" + localeName) ?
                    "Info/Description_" + localeName :
                    "Info/Description",
                "").toString();

    _install_notes = settings.value(
                settings.contains("Info/Notes_" + localeName) ?
                    "Info/Notes_" + localeName :
                    "Info/Notes",
                "").toString();

    _title = settings.value("Info/Title", "").toString();
    _genre = settings.value("Info/Genre", "WebOS Game").toString();
    _url = settings.value("Info/URL", "").toString();
    _preview_image = settings.value("Info/PreviewImage", "").toString();
    _ipk_name = settings.value("Info/IPK_Name", "").toString();
    _space_required = settings.value("Info/Space_Required", 0).toInt();
    _id = settings.value("Info/ID", "").toString();
    _postinstall = settings.value("Info/Postinstall", "").toString();
    _preload = settings.value("Info/Preload", "").toString();
    _save_files = settings.value("Info/SaveFiles", "")
            .toString()
            .split(',', QString::SkipEmptyParts);
    _env = settings.value("Info/Env", "").toString();

    QSettings status(ORGANIZATION_NAME, APPLICATION_NAME);
    _is_installed = status.value(_id + "/Installed", false).toBool();
    _json_title = status.value(_id + "/Title", "").toString();
    _json_version = status.value(_id + "/Version", "").toString();
    _bin_path = status.value(_id + "/BinPath", "").toString();
    _data_path = status.value(_id + "/DataPath", "").toString();
    _save_timestamp = status.value(_id + "/SaveTimestamp", "").toString();
    _json_id = status.value(_id + "/Id", "").toString();
    _json_binary = status.value(_id + "/Binary", "").toString();

    return true;
}

void GameInfo::SaveStatus()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    settings.setValue(_id + "/Installed", _is_installed);
    if (_is_installed)
    {
        settings.setValue(_id + "/Title", _json_title);
        settings.setValue(_id + "/Version", _json_version);
        settings.setValue(_id + "/BinPath", _bin_path);
        settings.setValue(_id + "/DataPath", _data_path);
        settings.setValue(_id + "/Id", _json_id);
        settings.setValue(_id + "/Binary", _json_binary);
    }
    else
    {
        settings.remove(_id + "/Title");
        settings.remove(_id + "/Version");
        settings.remove(_id + "/BinPath");
        settings.remove(_id + "/DataPath");
        settings.remove(_id + "/Binary");
    }
    if (!_save_timestamp.isEmpty())
        settings.setValue(_id + "/SaveTimestamp", _save_timestamp);
}

//
// Parse appinfo.json and extract data
//
bool GameInfo::ParseJson(QString json_data)
{
    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + json_data + ")");

    if (engine.hasUncaughtException())
    {
        qWarning() << "Error parsing Json: "
                << sc.toString();
        return false;
    }

    _json_binary = sc.property("main").toString();
    _json_title = sc.property("title").toString();
    _json_vendor = sc.property("vendor").toString();
    _json_version = sc.property("version").toString();
    _json_icon = sc.property("icon").toString();
    _json_id = sc.property("id").toString();

    return true;
}

//
// Form 'Exec' command, add LD_PRELOAD hack if needed
//
QString GameInfo::Exec()
{
    QString env;
 
    if (!_env.isEmpty() || !_preload.isEmpty())
    {
        env += "/usr/bin/env ";
    }

    if (!_env.isEmpty())
        env += QString("%1 ")
                .arg(_env);

    if (!_preload.isEmpty())
    {
        env += ParsePreload(_preload.section(';', 0, 0));
        env += ParsePreload(_preload.section(';', 1, 1));
        env += QString("LD_PRELOAD=%1 ").arg("libldhack.so");
    }

    return QString("%1\""BASE_DIR"/wrapper-wgames.sh\" "
                    "\"%2\" "
                    "\"%3\"")
            .arg(env)
            .arg(DBusName())
            .arg(_bin_path + "/" +
                 _json_binary.section('/', -1, -1));

}


QString GameInfo::DBusName()
{
    return _json_id
            .replace('-', '_')
            .remove(QRegExp("[^.a-zA-Z0-9_]"));
}

QString GameInfo::ParsePreload(QString section)
{
    if (section.isEmpty())
        return "";

    QString fromDir = section.section(':', 0, 0);
    QString toDir = section.section(':', 1, 1);
    toDir.replace("BIN_DIR", _bin_path);
    toDir.replace("DATA_DIR", _data_path);

    if (!toDir.endsWith('/'))
        toDir += '/';

    if (fromDir == "PowerVR")
        return " LD_POWERVR_DIR=\"" + toDir + "\" ";

    return QString(" LD_HACK_FROM=\"" + fromDir + "\" " +
                   "LD_HACK_TO=\"" + toDir + "\" ");
}
