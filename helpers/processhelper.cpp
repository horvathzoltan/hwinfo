#include "processhelper.h"
#include "helpers/logger.h"

#include <QCoreApplication>
#include <QProcess>

ProcessHelper::Output ProcessHelper::Execute(const QString& cmd, const QStringList& args, int timeout){
    QProcess process;
    static QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", "/usr/lib"); // workaround - https://bugreports.qt.io/browse/QTBUG-2284
    process.setProcessEnvironment(env);
    static auto path = QCoreApplication::applicationDirPath();
    process.setWorkingDirectory(path);

    process.start(cmd,args);

    if (!process.waitForStarted(-1))
    {
        zInfo("Could not start");
        return {"","",1};
    }

    bool isFinished = process.waitForFinished(timeout);

    if(!isFinished)
    {
        process.close();
        return {"","",1};
    }

    Output e{process.readAllStandardOutput(),
                process.readAllStandardError(),
                process.exitCode()};
    return e;
}

