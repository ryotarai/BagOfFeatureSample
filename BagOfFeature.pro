#-------------------------------------------------
#
# Project created by QtCreator 2012-06-18T21:35:35
#
#-------------------------------------------------

QT       += core gui

TARGET = BagOfFeature
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    debugmat.h

FORMS    += mainwindow.ui

LIBS += -L/usr/local/lib/ \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_features2d \
    -lopencv_nonfree \
    -lopencv_flann \
    -lopencv_calib3d

INCLUDEPATH += /usr/local/include
