#include <QTextStream>
#include <QByteArray>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>

#include "trackercfg.h"
#include "config.h"

TrackerCfg::TrackerCfg(QWidget *parent,
                       QString newDir,
                       QString oldDir) :
    QWidget(parent),
    _unchanged(true)
{
    _oldDir = Sanitize(oldDir);
    _newDir = Sanitize(newDir);
}

QString TrackerCfg::Sanitize(QString dir)
{
    QString ret(dir);

    // We need to update tracker only for
    // MyDocs subdir
    if (!dir.contains("MyDocs"))
        return "";

    if (!dir.endsWith("/"))
        ret.append("/");
    if (!dir.contains("$HOME") && dir.contains("/home/user"))
        ret.replace("/home/user", "$HOME");

    return ret;
}

bool TrackerCfg::Update()
{
    if (_newDir.isEmpty())
        return true;

    QFile config(TRACKER_PATH);
    if (!config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
            qWarning() << "Cannot open tracker config ("TRACKER_PATH")!";
            return false;
    }
    QTextStream stream(&config);
    QByteArray buffer;
    QTextStream out(&buffer, QIODevice::WriteOnly);

    do
    {
            QString str = stream.readLine();
            if (str.contains("NoWatchDirectory"))
                    str = ReplaceDirs(str);
            out << str << "\n";
    }
    while (!stream.atEnd());

    out.flush();
    config.close();

    if (_unchanged)
        return true;

    if (QMessageBox::Yes == QMessageBox::question(this, tr("Tracker Config"),
                             tr("The tracker configuration must be "
                                "changed to avoid messing up media cache "
                                "with media from the games' folders.<br />"
                                "The tracker also need to restart and rebuild "
                                "databases.<br /><br />"
                                "Do you want to change config and restart "
                                "tracker now?"),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes))
    {
        if (!config.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            qWarning() << "Cannot open tracker config for writing!";
            return false;
        }
        config.write(buffer);
        config.close();

        QProcess proc;
        proc.start("/usr/bin/tracker-processes -r");
        proc.waitForFinished();
    }

    return true;
}

QString TrackerCfg::ReplaceDirs(QString string)
{
        QString key = string.section('=', 0, 0);
        QStringList list = string.section('=', -1, -1).trimmed()
                .split(';', QString::SkipEmptyParts);
        int ind_prev(0);
        for (int i = 0; i < list.size(); ++i)
        {
                if (list[i].contains(_oldDir))
                {
                        ind_prev = i;
                        break;
                }
        }

        if (ind_prev)
        {
            if (_oldDir != _newDir)
            {
                _unchanged = false;
                list[ind_prev] = _newDir;
            }
        }
        else
        {
            _unchanged = false;
            list << _newDir;
        }

        return QString("%1= %2").arg(key).arg(list.join(";"));
}
