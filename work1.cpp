#include "work1.h"
//#include "helpers/logger.h"
//#include "helpers/textfilehelper.h"
#include "helpers/processhelper.h"
#include "helpers/sqlhelper.h"
#include "settings.h"
#include "environment.h"
#include <QVariant>
#include <iostream>

extern Settings _settings;
extern Environment _environment;

Work1Params Work1::_params;

int Work1::doWork()
{
    QString mac=_params.mac;
    if(mac.isEmpty() && !_params.ip.isEmpty()){
        mac = GetMac();
    }
    if(mac.isEmpty()) return 4;

    SQLHelper sqlh;
    static const QString CONN = QStringLiteral("conn1");
    auto db = sqlh.Connect(_settings._sql_settings, CONN);
    auto hwdata = sqlh.GetHwData(db, mac); //b8:27:eb:e3:cc:41
    if(!hwdata.isValid()) return 5;
    std::cout << (mac+';'+hwdata.ToString()+'\n').toStdString();
    return 0;
}

QString Work1::GetMac(){
    if(_params.ip.isEmpty()) return {};

    auto out = ProcessHelper::Execute("ping", {"-c5","-W1",_params.ip});
    if(out.exitCode) return {};
    out = ProcessHelper::Execute("arp", {"-a"});

    QString mac = "";
    auto lines = out.stdOut.split('\n');
    for(auto &l: lines){
        if(l.isEmpty()) continue;
        auto fields = l.split(' ', Qt::SplitBehaviorFlags::SkipEmptyParts);
        if(fields.count()<3) continue;
        if(fields[1]!='('+_params.ip+')') continue;
        mac = fields[3];
        break;
    }
    return mac;
}
