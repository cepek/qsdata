#-------------------------------------------------
#
# Project created by QtCreator 2015-12-25T19:04:42
#
#-------------------------------------------------
include(../qsdata.pri)

QT       += core gui widgets printsupport

TARGET = qsdata
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    samplestoetalon.cpp \
    notes.cpp \
    qsdatasettings.cpp \
    version.cpp \
    parameterseditor.cpp \
    xmltoascii.cpp

HEADERS += mainwindow.h \
    samplestoetalon.h \
    notes.h \
    qsdatasettings.h \
    version.h \
    parameterseditor.h \
    xmltoascii.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qsdlib/release/ -lqsdlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qsdlib/debug/ -lqsdlib
else:unix: LIBS += -L$$OUT_PWD/../qsdlib/ -lqsdlib

INCLUDEPATH += $$PWD/../qsdlib
DEPENDPATH += $$PWD/../qsdlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qsdlib/release/libqsdlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qsdlib/debug/libqsdlib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qsdlib/release/qsdlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qsdlib/debug/qsdlib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../qsdlib/libqsdlib.a

RESOURCES += \
    qsdata.qrc

DISTFILES +=
