#include "environment.h"
#include "common/helper/ProcessHelper/processhelper.h"

Environment::Environment()
{

}

QString Environment::user_at_host  = UserAtHost();

QString Environment::User()
{
    auto l = QString(qgetenv("USER"));
    if (!l.isEmpty()) return l;
    l = qgetenv("USERNAME");
    if (!l.isEmpty()) return l;
#ifdef Q_OS_UNIX
    auto cmd = "whoami";
    auto r = com::helper::ProcessHelper::Execute(cmd);
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
