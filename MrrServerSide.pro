QT       += core gui serialport multimedia

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gstreamer_serverside.cpp \
    idle_sleep_notifications.c \
    main.cpp \
    mainwindow.cpp \
    config_object.cpp \
    networkcomms.cpp \
    serialcomms.cpp

HEADERS += \
    gstreamer_serverside.h \
    mainwindow.h \
    config_object.h \
    networkcomms.h \
    serialcomms.h

FORMS += \
    mainwindow.ui \
    configdialog.ui

QMAKE_MACOSX_DEPLOYMENT_TARGET = "10.15"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# GStreamer support
macx: LIBS += -L/Library/Frameworks/GStreamer.framework/Libraries -lgstreamer-1.0.0 -lglib-2.0.0
INCLUDEPATH += /Library/Frameworks/GStreamer.framework/Headers

DISTFILES += \
    TODO.txt
