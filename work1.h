#ifndef WORK1_H
#define WORK1_H


#include <QString>

struct Work1Params{
public:
    QString ip;
    QString mac;
    bool query;
};

class Work1
{
public:
    static Work1Params _params;

    static int doWork();    
    static QString GetMac();
};

#endif // WORK1_H
