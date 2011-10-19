#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <QString>
#include <QStringList>
#include <QDate>

class GameInfo
{
public:
    GameInfo();
    GameInfo(QString filename);

    bool loadFromFile(QString filename);

    QString Title() const { return _title; }
    QString Description() const { return _description; }
    QString Genre() const { return _genre; }
    QString URL() const { return _url; }
    QString PreviewImage() const { return _preview_image; }
    QString IPKFile() const { return _ipk_name; }
    qint32 SpaceRequired() const { return _space_required; }
    QString ID() const { return _id; }
    QString InstallNotes() const { return _install_notes; }
    QString PostInstall() const { return _postinstall; }
    QString Preload() const { return _preload; }
    QStringList SaveFiles() const { return _save_files; }
    QString Env() const { return _env; }

    QString JsonTitle() const { return _json_title; }
    QString JsonBinary() const { return _json_binary; }
    QString JsonVendor() const { return _json_vendor; }
    QString JsonVersion() const { return _json_version; }
    QString JsonIcon() const { return _json_icon; }
    QString JsonID() const { return _json_id; }

    bool Installed() const { return _is_installed; }
    void SetInstalled(bool value) { _is_installed = value; }
    QString IPKPath() const { return _ipk_path; }
    void SetIPKPath(QString path) { _ipk_path = path; }
    QString BinPath() const { return _bin_path; }
    void SetBinPath(QString path) { _bin_path = path; }
    QString DataPath() const { return _data_path; }
    void SetDataPath(QString path) { _data_path = path; }

    void SetSaveTimestamp(QDate date) { _save_timestamp = date.toString();}
    QDate GetSaveTimestamp() const { return QDate::fromString(_save_timestamp); }

    bool ParseJson(QString result);
    void SaveStatus();

    QString Exec();
    QString DBusName();

private:
    // Info provided by game supporters (static)
    QString _title; // may differ from stored in .ipk
    QString _description;
    QString _genre;
    QString _url;
    QString _ipk_name; // mask for filename // TODO: add mask support
    QString _preview_image; // path to preview image
    qint32 _space_required; // how much space in MBytes
    QString _id; // ID string to compare with extracted from appinfo.json
    QString _install_notes; // some info displayed at the end of installation
    QString _postinstall; // postinstall script
    QString _preload;
    QStringList _save_files;
    QString _env;

    // Dynamically changed
    bool _is_installed;
    QString _ipk_path; // full path to .ipk file during install
    QString _bin_path; // path to binary dir
    QString _data_path; // path to data dir
    QString _save_timestamp;

    // Json data
    QString _json_title;
    QString _json_binary; // 'main' property
    QString _json_vendor;
    QString _json_version;
    QString _json_icon; // relative path to Icon
    QString _json_id; // id string

    QString ParsePreload(QString);
};

#endif // GAMEINFO_H
