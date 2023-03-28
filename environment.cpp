#include "environment.h"
#include "helpers/processhelper.h"

QString Environment::User()
{
    auto l = QString(qgetenv("USER"));
    if (!l.isEmpty()) return l;
    l = qgetenv("USERNAME");
    if (!l.isEmpty()) return l;
#ifdef Q_OS_UNIX
    auto cmd = "whoami";
    auto r = ProcessHelper::Execute(cmd);
    if(!r.stdErr.isEmpty()) return QString();
    l = r.stdOut.trimmed();
#endif
    return l;
}

QString Environment::Host(){ return QSysInfo::machineHostName(); }

QString Environment::UserAtHost(){
    auto u = User();
    auto h = Host();
    return (u.isEmpty())?h:u+'@'+h;
}
