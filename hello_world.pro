#-------------------------------------------------
#
# Project created by QtCreator 2019-07-15T22:29:02
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hello_world
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.s
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    SerialPort/serial_handle.cpp

HEADERS += \
        mainwindow.h \
    SerialPort/serial_handle.h \
    HIKVision_SDK/head/HCNetSDK.h \
    HIKVision_SDK/head/DataType.h \
    HIKVision_SDK/head/DecodeCardSdk.h \
    HIKVision_SDK/head/plaympeg4.h \
    types.h

FORMS += \
        mainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/HIKVision_SDK/lib/ -lHCNetSDK
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/HIKVision_SDK/lib/ -lHCNetSDK
else:unix: LIBS += -L$$PWD/HIKVision_SDK/lib/ -lHCNetSDK

INCLUDEPATH += $$PWD/HIKVision_SDK
DEPENDPATH += $$PWD/HIKVision_SDK

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/HIKVision_SDK/lib/ -lHCCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/HIKVision_SDK/lib/ -lHCCore
else:unix: LIBS += -L$$PWD/HIKVision_SDK/lib/ -lHCCore

INCLUDEPATH += $$PWD/HIKVision_SDK
DEPENDPATH += $$PWD/HIKVision_SDK

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/HIKVision_SDK/lib/HCNetSDKCom/ -lHCGeneralCfgMgr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/HIKVision_SDK/lib/HCNetSDKCom/ -lHCGeneralCfgMgr
else:unix: LIBS += -L$$PWD/HIKVision_SDK/lib/HCNetSDKCom/ -lHCGeneralCfgMgr

INCLUDEPATH += $$PWD/HIKVision_SDK/lib
DEPENDPATH += $$PWD/HIKVision_SDK/lib
