#-------------------------------------------------
#
# Project created by QtCreator 2013-02-03T21:39:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-update-notifier
TEMPLATE = app

SOURCES += main.cpp\
qtUpdateNotifier.cpp \
    check_updates.cpp \
    logwindow.cpp

HEADERS  += qtUpdateNotifier.h \
    check_updates.h \
    logwindow.h

RESOURCES += icons.qrc


INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/build-qt-update-notifier

LIBS += -lkdeui -L/home/local/KDE4/lib -L/usr/lib

FORMS += \
    logwindow.ui

