#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T14:16:11
#
#-------------------------------------------------

QT       -= gui

TARGET = NmeaLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    context.c \
    generate.c \
    generator.c \
    gmath.c \
    info.c \
    parse.c \
    parser.c \
    sentence.c \
    time.c \
    tok.c

HEADERS += \
    nmea/config.h \
    nmea/context.h \
    nmea/generate.h \
    nmea/generator.h \
    nmea/gmath.h \
    nmea/info.h \
    nmea/nmea.h \
    nmea/parse.h \
    nmea/parser.h \
    nmea/sentence.h \
    nmea/time.h \
    nmea/tok.h \
    nmea/units.h

CONFIG(debug,debug|release){
    DESTDIR = ../build/$$TARGET/debug
    MOC_DIR = ../build/$$TARGET/debug
    OBJECTS_DIR = ../build/$$TARGET/debug

    target.path = ../lib/$$TARGET/debug
    headers.path = ../lib/$$TARGET/include
}else{
    DESTDIR = ../build/$$TARGET/release
    MOC_DIR = ../build/$$TARGET/release
    OBJECTS_DIR = ../build/$$TARGET/release

    target.path = ../lib/$$TARGET/release
    headers.path = ../lib/$$TARGET/include
}

INSTALLS += target
headers.files = $$HEADERS
INSTALLS += headers

