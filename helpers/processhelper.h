#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QStringList>

class ProcessHelper
{
public:
    struct Output
        {
            QString stdOut;
            QString stdErr;
            int exitCode;
            QString ToString();
        };
    static Output Execute(const QString& cmd, const QStringList& args = {}, int timeout = -1);
};

#endif // PROCESSHELPER_H
