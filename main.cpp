#include <QCoreApplication>
#include "helpers/logger.h"
#include "helpers/signalhelper.h"
#include "helpers/commandlineparserhelper.h"
#include "helpers/coreappworker.h"

#include "settings.h"
#include "environment.h"
#include "work1.h"

// projects/run/CommandLineArguments
// -m b8:27:eb:e3:cc:41
// vagy
// -i 10.10.10.102

Settings _settings(
    {
        "QODBC",
        "BuildInfoFlex",
            {{"office.logcontrol.hu", 9876}, {"172.16.1.5", 1433}},
        "sa",
        "Gtr7jv8fh2"
    } );

Environment _environment;

auto main(int argc, char *argv[]) -> int
{
    SignalHelper::setShutDownSignal(SignalHelper::SIGINT_); // shut down on ctrl-c
    SignalHelper::setShutDownSignal(SignalHelper::SIGTERM_); // shut down on killall
    Logger::Init(Logger::ErrLevel::INFO, Logger::DbgLevel::TRACE, true, true);


    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("hwinfo"));

    QCommandLineParser parser;

    parser.setApplicationDescription(QStringLiteral("command line hwinfo app."));
    parser.addHelpOption();
    parser.addVersionOption();

//    const QString OPTION_TMP = QStringLiteral("template");
//    const QString OPTION_OUT = QStringLiteral("output");
    const QString OPTION_IP = QStringLiteral("ip");
    const QString OPTION_MAC = QStringLiteral("mac");

//    com::helper::CommandLineParserHelper::addOption(&parser, OPTION_TMP, QStringLiteral("template file"));
//    com::helper::CommandLineParserHelper::addOption(&parser, OPTION_OUT, QStringLiteral("file as output"));
    CommandLineParserHelper::addOption(&parser, OPTION_IP, QStringLiteral("ip address"));
    CommandLineParserHelper::addOption(&parser, OPTION_MAC, QStringLiteral("mac address"));

    parser.process(a);

//    // statikus, számítunk arra, hogy van
//    Work1::params.tmpfile = parser.value(OPTION_TMP);
//    Work1::params.ofile = parser.value(OPTION_OUT);
    Work1::_params.ip = parser.value(OPTION_IP);
    Work1::_params.mac = parser.value(OPTION_MAC);

    //TODO a parser is nem kell, a paraméterek kellenek
    CoreAppWorker c(Work1::doWork, &a, &parser);

    volatile auto errcode = c.run();

    switch(errcode)
    {
        case 1: zInfo("no project name use option -p name"); break;
        case 2: zInfo("ping error"); break;
        case 3: zInfo("hwid is empty"); break;
        case 4: zInfo("mac is empty"); break;
        case 5: zInfo("hwdata not found"); break;
    }

    auto e = QCoreApplication::exec();    
    return e;
}
