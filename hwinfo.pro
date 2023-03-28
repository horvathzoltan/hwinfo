QT += network sql
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix:HOME = $$system(echo $HOME)
win32:HOME = $$system(echo %userprofile%)

CONFIG(debug, debug|release){
BUILD = debug
} else {
BUILD = release
}

contains(QMAKESPEC,.*linux-rasp-pi\d*-.*){
    message(rpi)
    CONFIG += rpi
}

SOURCES += \
        environment.cpp \
        helpers/commandlineparserhelper.cpp \
        helpers/coreappworker.cpp \
        helpers/logger.cpp \
        helpers/processhelper.cpp \
        helpers/signalhelper.cpp \
        helpers/sqlhelper.cpp \
        main.cpp \
        settings.cpp \
        work1.cpp

HEADERS += \
    environment.h \
    helpers/commandlineparserhelper.h \
    helpers/coreappworker.h \
    helpers/logger.h \
    helpers/processhelper.h \
    helpers/signalhelper.h \
    settings.h \
    helpers/sqlhelper.h \
    work1.h

unix:rpi:{
message(LIBS added for raspberry_pi)
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


