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
    logwindow.cpp \
    instance.cpp \
    startsynaptic.cpp \
    configuredialog.cpp \
    checkoldpackages.cpp \
    utility.cpp

HEADERS  += qtUpdateNotifier.h \
    check_updates.h \
    logwindow.h \
    instance.h \
    startsynaptic.h \
    configuredialog.h \
    checkoldpackages.h \
    utility.h

RESOURCES += icons.qrc


INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/build-qt-update-notifier

LIBS += -lkdeui -L/home/local/KDE4/lib -L/usr/lib -lQtNetwork

FORMS += \
    logwindow.ui \
    configuredialog.ui

TRANSLATIONS += translations.ts/translations.ts
TRANSLATIONS += translations.ts/dutch_NL.ts
TRANSLATIONS += translations.ts/greek_GR.ts
TRANSLATIONS += translations.ts/german_DE.ts

