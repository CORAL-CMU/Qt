#-------------------------------------------------
#
# Project created by QtCreator 2017-02-10T08:58:10
#
#-------------------------------------------------

QT       += core charts gui network websockets webchannel webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NmeaViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32{
    INCLUDEPATH += C:\SDK\Lib\Static\QGMapInterface
    INCLUDEPATH += C:\SDK\Lib\Static\QJSInterface
    INCLUDEPATH += C:\SDK\Lib\Static\NmeaLib

    CONFIG(debug,debug|release){
        LIBS += C:\SDK\Lib\Static\QGMapInterface\QGMapInterface_Debug.lib
        LIBS += C:\SDK\Lib\Static\QJSInterface\QJSInterface_Debug.lib
        LIBS += C:\SDK\Lib\Static\NmeaLib\NmeaLib_Debug.lib
    }else{
        LIBS += C:\SDK\Lib\Static\QGMapInterface\QGMapInterface_Release.lib
        LIBS += C:\SDK\Lib\Static\QJSInterface\QJSInterface_Release.lib
        LIBS += C:\SDK\Lib\Static\NmeaLib\NmeaLib_Release.lib
    }
}

