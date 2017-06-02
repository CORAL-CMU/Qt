#-------------------------------------------------
#
# Project created by QtCreator 2016-09-30T19:15:59
#
#-------------------------------------------------

QT       += core gui positioning websockets webchannel webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NMEA
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

unix{
    MOC_DIR = $$(HOME)/APP/$${TARGET}
    OBJECTS_DIR = $$(HOME)/APP/$${TARGET}
    UI_DIR = $$(HOME)/APP/$${TARGET}
    RCC_DIR = $$(HOME)/APP/$${TARGET}
    DESTDIR = $$(HOME)/APP/$${TARGET}

    INCLUDEPATH += $$(HOME)/SDK/Lib/Static/QGMapInterface
    debug:LIBS += -L$$(HOME)/SDK/Lib/Static/QGMapInterface -lQGMapInterface_Debug
    release:LIBS += -L$$(HOME)/SDK/Lib/Static/QGMapInterface -lQGMapInterface_Release

    INCLUDEPATH += $$(HOME)/SDK/Lib/Static/QJSInterface
    debug:LIBS += -L$$(HOME)/SDK/Lib/Static/QJSInterface -lQJSInterface_Debug
    release:LIBS += -L$$(HOME)/SDK/Lib/Static/QJSInterface -lQJSInterface_Release
}

win32{
    INCLUDEPATH += C:\SDK\Lib\Static\QGMapInterface
    INCLUDEPATH += C:\SDK\Lib\Static\QJSInterface

    CONFIG(debug,debug|release){
        LIBS += C:\SDK\Lib\Static\QGMapInterface\QGMapInterface_Debug.lib
        LIBS += C:\SDK\Lib\Static\QJSInterface\QJSInterface_Debug.lib
    }else{
        LIBS += C:\SDK\Lib\Static\QGMapInterface\QGMapInterface_Release.lib
        LIBS += C:\SDK\Lib\Static\QJSInterface\QJSInterface_Release.lib
    }
}

RESOURCES += \
    gmap.qrc
