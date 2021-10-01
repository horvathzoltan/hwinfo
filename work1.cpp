#include "work1.h"
#include "common/logger/log.h"
#include "common/helper/textfilehelper/textfilehelper.h"
#include "common/helper/ProcessHelper/processhelper.h"
#include "sqlhelper.h"
#include "settings.h"
#include "environment.h"
#include <QVariant>
#include <iostream>

extern Settings _settings;
extern Environment _environment;

Work1Params Work1::params;

Work1::Work1() = default;

auto Work1::doWork() -> int
{    
    if(params.ip.isEmpty()) return 1;
    //zTrace();
    SQLHelper sqlh;
    static const QString CONN = QStringLiteral("conn1");

    QString cmd = QStringLiteral("ping %1 -w 1 -c 1").arg(params.ip);
    auto out = com::helper::ProcessHelper::Execute(cmd);
    if(out.exitCode) return 2;
    cmd = QStringLiteral("arp -a");
    out = com::helper::ProcessHelper::Execute(cmd);

    QString mac = "";
    auto lines = out.stdOut.split('\n');
    for(auto &l: lines){
        if(l.isEmpty()) continue;
        auto fields = l.split(' ', Qt::SplitBehaviorFlags::SkipEmptyParts);
        if(fields.count()<3) continue;

        if(fields[1]!='('+params.ip+')') continue;
        mac = fields[3];
    }

    if(mac.isEmpty()) return 4;

    auto db = sqlh.Connect(_settings._sql_settings, CONN);
    auto hwdata = sqlh.GetHwData(db, mac);
    if(!hwdata.isValid()) return 5;
    zInfo(mac+';'+hwdata.ToString());
    return 0;
}


