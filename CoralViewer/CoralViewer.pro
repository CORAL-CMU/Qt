#-------------------------------------------------
#
# Project created by QtCreator 2017-06-09T15:25:06
#
#-------------------------------------------------

QT       += core gui qml quick 3dcore

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 arcgis_runtime_qml_cpp100_0_0

TARGET = CoralViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += /home/alexanderhmw/SDK/CMU_LIBS/QStaticLibs/QCoralViewer
CONFIG(debug, debug|release){
    LIBS += -L/home/alexanderhmw/SDK/CMU_LIBS/QStaticLibs/QCoralViewer -lQCoralViewer_Debug
}else{
    LIBS += -L/home/alexanderhmw/SDK/CMU_LIBS/QStaticLibs/QCoralViewer -lQCoralViewer_Release
}

RESOURCES += \
    coralviewer.qrc
