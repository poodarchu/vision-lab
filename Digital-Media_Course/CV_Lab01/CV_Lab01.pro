#-------------------------------------------------
#
# Project created by QtCreator 2016-10-07T23:41:22
#
#-------------------------------------------------

QT       += core gui concurrent widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CV_Lab01
TEMPLATE = app


SOURCES += main.cpp\
        cvlabapp.cpp \
    medianfilter.cpp

HEADERS  += cvlabapp.h \
    medianfilter.h

FORMS    += cvlabapp.ui

target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/imagescaling
INSTALLS += target

wince: DEPLOYMENT_PLUGIN += qgif qjpeg
