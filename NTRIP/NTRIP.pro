#-------------------------------------------------
#
# Project created by QtCreator 2017-01-04T13:07:24
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NTRIP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ntrip.cpp \
    venus8.cpp

HEADERS  += mainwindow.h \
    ntrip.h \
    venus8.h

FORMS    += mainwindow.ui

CONFIG(debug,debug|release){
    DESTDIR = ../build/$$TARGET/debug
    MOC_DIR = ../build/$$TARGET/debug
    OBJECTS_DIR = ../build/$$TARGET/debug

    target.path = ../app/$$TARGET/debug
    INSTALLS += target

    INCLUDEPATH += ../lib/NmeaLib/include
    win32:LIBS += ../lib/NmeaLib/debug/NmeaLib.lib
    unix:LIBS += -L../lib/NmeaLib/debug -lNmeaLib
}else{
    DESTDIR = ../build/$$TARGET/release
    MOC_DIR = ../build/$$TARGET/release
    OBJECTS_DIR = ../build/$$TARGET/release

    target.path = ../app/$$TARGET/release
    INSTALLS += target

    INCLUDEPATH += ../lib/NmeaLib/include
    win32:LIBS += ../lib/NmeaLib/release/NmeaLib.lib
    unix:LIBS += -L../lib/NmeaLib/release -lNmeaLib
}


