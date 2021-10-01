#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>



class Environment
{
public:
    Environment();

    static QString user_at_host;

    static QString Host();
    static QString UserAtHost();
    static QString User();
};

#endif // ENVIRONMENT_H
