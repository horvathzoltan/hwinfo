#include "sqlhelper.h"
#include "helpers/logger.h"
#include "helpers/processhelper.h"
#include <QDirIterator>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDateTime>
#include <QTcpSocket>
#include <QRegularExpression>


auto SQLHelper::GetDriverName() -> QString{
    //opt/microsoft/msodbcsql17/lib64/libmsodbcsql-17.10.so.2.1
    auto driverdir = QStringLiteral("/opt/microsoft/msodbcsql17/lib64");
    auto driverpattern = QStringLiteral("^.*libmsodbcsql-?[0-9.so]*$");
    auto driverfi = GetMostRecent(driverdir, driverpattern);
    if(!driverfi.isFile()) return QString();
    return driverfi.absoluteFilePath();
}

auto SQLHelper::Ping(const QString& ip) -> bool
{
    auto out = ProcessHelper::Execute("ping", {"-c5","-W1",ip});
    bool ok = !out.exitCode;
    return ok;
}

//https://docs.microsoft.com/en-us/sql/linux/sql-server-linux-setup-tools?view=sql-server-ver15#ubuntu
QSqlDatabase SQLHelper::Connect(const SQLSettings& s, const QString& name)
{
    QSqlDatabase db;
       const HostPort* h=nullptr;
       for(auto&i:s.hosts)
       {
           //zInfo("host: "+i.host+":"+QString::number(i.port));
           if(Ping(i.host)) {
               zInfo("reachable: "+i.host+":"+QString::number(i.port));
               QTcpSocket s;
               s.connectToHost(i.host, i.port);
               auto isok = s.waitForConnected(1000);
               if(isok){
                   s.disconnectFromHost();
                   if (s.state() != QAbstractSocket::UnconnectedState) s.waitForDisconnected();
                   h=&(i);
                   zInfo("socket ok");
                   break;
               }
               else{
                   zInfo("socket err");
               }
           }
           else{
               zInfo("unreachable:"+i.host);
           }
       }

       if(h)
       {
           zInfo("available host found: "+h->host+":"+QString::number(h->port));
           db = QSqlDatabase::addDatabase(s.driver, name);
           auto driverfn = GetDriverName();
           if(driverfn.isEmpty()) return db;
           auto dbname = QStringLiteral("DRIVER=%1;Server=%2,%3;Database=%4")
                   .arg(driverfn,h->host).arg(h->port).arg(s.dbname);
           db.setDatabaseName(dbname);
           db.setUserName(s.user);
           db.setPassword(s.password);
       }
       return db;
}

void Error(const QSqlError& err)
{
    if(err.isValid()) zInfo(QStringLiteral("QSqlError: %1 - %2").arg(err.type()).arg(err.text()));
}


QFileInfo SQLHelper::GetMostRecent(const QString& path, const QString& pattern)
{
    QFileInfo most_recent;
    static const QDate d1 = QDate(1980,1,1);
    QDateTime tstamp = d1.startOfDay(Qt::UTC);// ::currentDateTimeUtc().addYears(-1);//f1.lastModified();
    QRegularExpression re(pattern);

    QDirIterator it(path);
    while (it.hasNext()) {
        auto fn = it.next();
        QFileInfo fi(fn);
        if(!fi.isFile()) continue;
        auto m = re.match(fn);
        if(!m.hasMatch()) continue;

        auto ts = fi.lastModified();
        if(ts>tstamp){ tstamp=ts; most_recent = fi;}
    }
    return most_recent;
}

QVariant SQLHelper::GetProjId(QSqlDatabase &db, const QString &project_name, int* rows)
{
    if(!db.isValid()) return -1;
    QVariant project_id;

    QSqlQuery query(db);
    bool isQueryOk = false;
    bool isOpened = db.open();
    if(isOpened) {
        isQueryOk = query.exec(QStringLiteral("SELECT id FROM BuildInfoFlex.dbo.Projects WHERE Name='%1';").arg(project_name));
        if(isQueryOk)
        {
            int r = query.numRowsAffected();
            if(rows) *rows = r;
            bool a = query.isActive();
            bool s = query.isSelect();
            if(a && s)
            {
                if(r>0)
                {
                    bool q = query.first();
                    if(q)
                    {
                        project_id= query.value(0);
                    }
                }
            }
        }
    }
    if(!isOpened || !isQueryOk){
        Error(query.lastError());
        Error(db.lastError());
    }
    db.close();
    return project_id;
}



SQLHelper::HwData SQLHelper::GetHwData(QSqlDatabase &db, const QString &mac, int* rows){
    HwData hwdata;

    if(!db.isValid()) return hwdata;

    QSqlQuery query(db);
    bool isQueryOk = false;
    bool isOpened = db.open();
    if(isOpened) {
        isQueryOk = query.exec(QStringLiteral("SELECT serial,board_rev FROM BuildInfoFlex.dbo.ManufacturingInfo WHERE lower(mac)='%1';").arg(mac));
        if(isQueryOk) {
            int r = query.numRowsAffected();
            if(rows) *rows = r;
            bool a = query.isActive();
            bool s = query.isSelect();
            if (a && s)
            {
                if(r>0)
                {
                    bool q = query.first();
                    if(q)
                    {
                        hwdata.serial = query.value(0);
                        hwdata.board_rev = query.value(1);
                    }
                }
            }
        }
    }

    if(!isOpened || !isQueryOk){
        Error(query.lastError());
        Error(db.lastError());
    }
    db.close();

    return hwdata;
}

QVariant SQLHelper::GetLastSerial(QSqlDatabase &db, int* rows){
    QVariant serial;

    QSqlQuery query(db);
    bool isQueryOk = false;
    bool isOpened = db.open();
    if(isOpened) {
        isQueryOk = query.exec(QStringLiteral("SELECT max(serial) FROM BuildInfoFlex.dbo.ManufacturingInfo;"));
        if(isQueryOk) {
            int r = query.numRowsAffected();
            if(rows) *rows = r;
            bool a = query.isActive();
            bool s = query.isSelect();
            if (a && s)
            {
                if(r>0)
                {
                    bool q = query.first();
                    if(q)
                    {
                        serial = query.value(0);
                    }
                }
            }
        }
    }

    if(!isOpened || !isQueryOk){
        Error(query.lastError());
        Error(db.lastError());
    }
    db.close();

    return serial;
}

//
void SQLHelper::InsertHwData(QSqlDatabase &db, const HwData &hwdata, int* rows){
    if(!db.isValid()) return;

    QSqlQuery query(db);
    bool isQueryOk = false;
    bool isOpened = db.open();
    if(isOpened) {
        QString cmd = QStringLiteral("INSERT INTO BuildInfoFlex.dbo.ManufacturingInfo (mac,serial,project,board_rev) VALUES (:mac,:serial,:project,:board_rev);");
        query.prepare(cmd);
        query.bindValue(":mac", hwdata.mac);
        query.bindValue(":serial", hwdata.serial);
        query.bindValue(":project", hwdata.project);
        query.bindValue(":board_rev", hwdata.board_rev);

        isQueryOk = query.exec();
        if(isQueryOk) {
            int r = query.numRowsAffected();
            if(rows) *rows = r;
        }
    }

    if(!isOpened || !isQueryOk){
        Error(query.lastError());
        Error(db.lastError());
    }
    db.close();

    return;
}
