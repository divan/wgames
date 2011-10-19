#ifndef TRACKERCFG_H
#define TRACKERCFG_H

#include <QWidget>
#include <QString>

class TrackerCfg : public QWidget
{
    Q_OBJECT
public:
    explicit TrackerCfg(QWidget *parent = 0,
                        QString newDir = "",
                        QString oldDir = "");

    bool Update();
private:
    QString _oldDir;
    QString _newDir;
    bool _unchanged;

    QString Sanitize(QString dir);
    QString ReplaceDirs(QString string);

signals:

public slots:

};

#endif // TRACKERCFG_H
