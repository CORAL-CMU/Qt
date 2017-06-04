#-------------------------------------------------
#
# Project created by QtCreator 2017-05-22T10:12:58
#
#-------------------------------------------------

QT += positioning core gui opengl xml network widgets sensors qml quick quickwidgets

CONFIG += c++11 arcgis_runtime_qml_cpp100_0_0

TARGET = ArcGisCoralViewer
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
        mainwindow.cpp \
    trajectoryviewer.cpp \
    applanixviewer.cpp

HEADERS  += mainwindow.h \
    trajectoryviewer.h \
    applanixviewer.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/eigen3

RESOURCES += \
    arcgiscoralviewer.qrc
