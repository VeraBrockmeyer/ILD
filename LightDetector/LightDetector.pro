#-------------------------------------------------
#
# Project created by QtCreator 2017-06-19T19:36:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LightDetector
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
    contourcalculator.cpp \
    lightcalculator.cpp \
    vectorcalculator.cpp

HEADERS  += mainwindow.h \
    contourcalculator.h \
    lightcalculator.h \
    vectorcalculator.h

FORMS    += mainwindow.ui

#Laura
INCLUDEPATH += C:\opencv-build\install\include
LIBS += -LC:\opencv-build\install\x86\mingw\lib \
    -lopencv_core320.dll \
    -lopencv_highgui320.dll \
    -lopencv_imgcodecs320.dll \
    -lopencv_imgproc320.dll \
    -lopencv_features2d320.dll \
    -lopencv_calib3d320.dll
    INCLUDEPATH += C:\dlib-19.4
    LIBS+= -lgdi32 -lcomctl32 -luser32 -lwinmm -lws2_32
    LIBS += -pthread
    #CONFIG += link_pkgconfig

#Vera
#INCLUDEPATH += F:\OpenCV_2\opencv-build\install\include
#LIBS += -LF:\OpenCV_2\opencv-build\install\x86\mingw\lib \
#    -lopencv_core320.dll \
#    -lopencv_highgui320.dll \
#    -lopencv_imgcodecs320.dll \
#    -lopencv_imgproc320.dll \
#    -lopencv_features2d320.dll \
#    -lopencv_calib3d320.dll
#INCLUDEPATH += F:\dlib-19.4
#LIBS+= -lgdi32 -lcomctl32 -luser32 -lwinmm -lws2_32
#LIBS += -pthread
#CONFIG += link_pkgconfig
