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

    win32:INCLUDEPATH += C:/SDK/CMU_LIBS/3rdLibs/NmeaLib
    win32:LIBS += C:/SDK/CMU_LIBS/3rdLibs/NmeaLib/NmeaLib_Debug.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/CMU_LIBS/3rdLibs/NmeaLib
    unix:LIBS += -L$$(HOME)/SDK/CMU_LIBS/3rdLibs/NmeaLib -lNmeaLib_Debug

    win32:INCLUDEPATH += C:/SDK/CMU_LIBS/StaticLibs/QGMapInterface
    win32:INCLUDEPATH += C:/SDK/CMU_LIBS/StaticLibs/QJSInterface
    win32:LIBS += C:/SDK/CMU_LIBS/StaticLibs/QGMapInterface/QGMapInterface_Debug.lib
    win32:LIBS += C:/SDK/CMU_LIBS/StaticLibs/QJSInterface/QJSInterface_Debug.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/CMU_LIBS/StaticLibs/QGMapInterface
    unix:INCLUDEPATH += $$(HOME)/SDK/CMU_LIBS/StaticLibs/QJSInterface
    unix:LIBS += -L$$(HOME)/SDK/CMU_LIBS/StaticLibs/QGMapInterface -lQGMapInterface_Debug
    unix:LIBS += -L$$(HOME)/SDK/CMU_LIBS/StaticLibs/QJSInterface -lQJSInterface_Debug
}else{
    DESTDIR = ../build/$$TARGET/release
    MOC_DIR = ../build/$$TARGET/release
    OBJECTS_DIR = ../build/$$TARGET/release

    target.path = ../app/$$TARGET/release
    INSTALLS += target

    win32:INCLUDEPATH += C:/SDK/CMU_LIBS/3rdLibs/NmeaLib
    win32:LIBS += C:/SDK/CMU_LIBS/3rdLibs/NmeaLib/NmeaLib_Release.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/CMU_LIBS/3rdLibs/NmeaLib
    unix:LIBS += -L$$(HOME)/SDK/CMU_LIBS/3rdLibs/NmeaLib -lNmeaLib_Release

    win32:INCLUDEPATH += C:/SDK/CMU_LIBS/StaticLibs/QGMapInterface
    win32:INCLUDEPATH += C:/SDK/CMU_LIBS/StaticLibs/QJSInterface
    win32:LIBS += C:/SDK/CMU_LIBS/StaticLibs/QGMapInterface/QGMapInterface_Release.lib
    win32:LIBS += C:/SDK/CMU_LIBS/StaticLibs/QJSInterface/QJSInterface_Release.lib
    unix:INCLUDEPATH += $$(HOME)/SDK/CMU_LIBS/StaticLibs/QGMapInterface
    unix:INCLUDEPATH += $$(HOME)/SDK/CMU_LIBS/StaticLibs/QJSInterface
    unix:LIBS += -L$$(HOME)/SDK/CMU_LIBS/StaticLibs/QGMapInterface -lQGMapInterface_Release
    unix:LIBS += -L$$(HOME)/SDK/CMU_LIBS/StaticLibs/QJSInterface -lQJSInterface_Release
}


