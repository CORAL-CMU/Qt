#-------------------------------------------------
#
# Project created by QtCreator 2017-01-04T13:07:24
#
#-------------------------------------------------

QT       += core gui network serialport network websockets webchannel webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NTRIP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ntrip.cpp \
    venus8.cpp \
    rtcm.cpp

HEADERS  += mainwindow.h \
    ntrip.h \
    venus8.h \
    rtcm.h

FORMS    += mainwindow.ui

CONFIG(debug,debug|release){
    DESTDIR = ../build/$$TARGET/debug
    MOC_DIR = ../build/$$TARGET/debug
    OBJECTS_DIR = ../build/$$TARGET/debug

    target.path = ../app/$$TARGET/debug
    INSTALLS += target

    win32:INCLUDEPATH += C:/SDK/Lib/Static/NmeaLib
    win32:LIBS += C:/SDK/Lib/Static/NmeaLib/NmeaLib_Debug.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/Lib/Static/NmeaLib
    unix:LIBS += -L$$(HOME)/SDK/Lib/Static/NmeaLib -lNmeaLib_Debug

    win32:INCLUDEPATH += C:/SDK/Lib/Static/QGMapInterface
    win32:INCLUDEPATH += C:/SDK/Lib/Static/QJSInterface
    win32:LIBS += C:/SDK/Lib/Static/QGMapInterface/QGMapInterface_Debug.lib
    win32:LIBS += C:/SDK/Lib/Static/QJSInterface/QJSInterface_Debug.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/Lib/Static/QGMapInterface
    unix:INCLUDEPATH += $$(HOME)/SDK/Lib/Static/QJSInterface
    unix:LIBS += -L$$(HOME)/SDK/Lib/Static/QGMapInterface -lQGMapInterface_Debug
    unix:LIBS += -L$$(HOME)/SDK/Lib/Static/QJSInterface -lQJSInterface_Debug
}else{
    DESTDIR = ../build/$$TARGET/release
    MOC_DIR = ../build/$$TARGET/release
    OBJECTS_DIR = ../build/$$TARGET/release

    target.path = ../app/$$TARGET/release
    INSTALLS += target

    win32:INCLUDEPATH += C:/SDK/Lib/Static/NmeaLib
    win32:LIBS += C:/SDK/Lib/Static/NmeaLib/NmeaLib_Release.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/Lib/Static/NmeaLib
    unix:LIBS += -L$$(HOME)/SDK/Lib/Static/NmeaLib -lNmeaLib_Release

    win32:INCLUDEPATH += C:/SDK/Lib/Static/QGMapInterface
    win32:INCLUDEPATH += C:/SDK/Lib/Static/QJSInterface
    win32:LIBS += C:/SDK/Lib/Static/QGMapInterface/QGMapInterface_Release.lib
    win32:LIBS += C:/SDK/Lib/Static/QJSInterface/QJSInterface_Release.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/Lib/Static/QGMapInterface
    unix:INCLUDEPATH += $$(HOME)/SDK/Lib/Static/QJSInterface
    unix:LIBS += -L$$(HOME)/SDK/Lib/Static/QGMapInterface -lQGMapInterface_Release
    unix:LIBS += -L$$(HOME)/SDK/Lib/Static/QJSInterface -lQJSInterface_Release
}


