#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T11:33:00
#
#-------------------------------------------------
include(../qsdata.pri)

QT       -= gui
QT       += widgets printsupport

TARGET = qsdlib
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

SOURCES += qsdlib.cpp \
    qcustomplot.cpp \
    spectraldata.cpp \
    spectralsampleetalon.cpp \
    nonnegativeleastsquares.cpp \
    parameters.cpp

HEADERS += qsdlib.h \
    qcustomplot.h \
    spectraldata.h \
    spectralsampleetalon.h \
    nonnegativeleastsquares.h \
    parameters.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
