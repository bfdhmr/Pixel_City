QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ckernel.cpp \
    fiveinlinezone.cpp \
    ld_userinfo.cpp \
    leaderboard_show.cpp \
    logindialog.cpp \
    main.cpp \
    maindialog.cpp \
    roomdialog.cpp \
    roomitem.cpp

HEADERS += \
    ckernel.h \
    fiveinlinezone.h \
    ld_userinfo.h \
    leaderboard_show.h \
    logindialog.h \
    maindialog.h \
    roomdialog.h \
    roomitem.h

FORMS += \
    fiveinlinezone.ui \
    ld_userinfo.ui \
    leaderboard_show.ui \
    logindialog.ui \
    maindialog.ui \
    roomdialog.ui \
    roomitem.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(./netapi/netapi.pri)
include(./md5/md5.pri)
include(./FiveInLine/FiveInLine.pri)

INCLUDEPATH += ./netapi/net
INCLUDEPATH += ./netapi/mediator
INCLUDEPATH += ./FiveInLine/

RESOURCES += \
    Resource.qrc
