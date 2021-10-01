#ifndef SQLHELPER_H
#define SQLHELPER_H

#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QVariant>

class SQLHelper
{
public:
    struct HostPort
    {
        QString host;// = "172.16.1.5";
        int port;
    };

    struct SQLSettings
    {
        QString driver;// = "QODBC";
        QString dbname;// = "BuildInfoFlex";
        QVector<HostPort> hosts;
        QString user;// = "sa";
        QString password;//= "Gtr7jv8fh2";
    };

    SQLHelper(){

    }
    QSqlDatabase Connect(const SQLSettings& s, const QString &name);

    static QFileInfo GetMostRecent(const QString &path, const QString &pattern);

    QVariant GetProjId(QSqlDatabase &db, const QString& project_name);
    int GetBuildNum(QSqlDatabase &db, int project);
    bool SetBuildNum(QSqlDatabase &db, int project, const QString &user, int buildnumber, const QString &project_name);
    static QString GetDriverName();

    struct HwData{
        QVariant serial;
        QVariant board_rev;

        bool isValid(){return serial.isValid()&&board_rev.isValid();}
        QString ToString(){return serial.toString()+';'+board_rev.toString();}
    };
    static HwData GetHwData(QSqlDatabase &db, const QString &project_name);
};

#endif // SQLHELPER_H
