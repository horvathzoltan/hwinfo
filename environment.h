#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>

class Environment
{
public:
    static QString Host();
    static QString UserAtHost();
    static QString User();
};

#endif // ENVIRONMENT_H
