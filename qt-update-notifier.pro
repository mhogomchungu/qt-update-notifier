#-------------------------------------------------
#
# Project created by QtCreator 2013-02-03T21:39:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-update-notifier
TEMPLATE = app

SOURCES += src/main.cpp\
	src/qtUpdateNotifier.cpp \
	src/logwindow.cpp \
	src/instance.cpp \
	src/configuredialog.cpp \
	src/utility.cpp \
	src/task.cpp \
	src/settings.cpp

HEADERS  += src/qtUpdateNotifier.h \
	src/logwindow.h \
	src/instance.h \
	src/configuredialog.h \
	src/utility.h \
	src/task.h \
	src/settings.h

RESOURCES += icons/icons.qrc

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/build-qt-update-notifier

LIBS += -lkdeui -L/home/local/KDE4/lib -L/usr/lib -lQtNetwork

FORMS += \
	src/logwindow.ui \
	src/configuredialog.ui

TRANSLATIONS += translations.ts/translations.ts
TRANSLATIONS += translations.ts/dutch_NL.ts
TRANSLATIONS += translations.ts/greek_GR.ts
TRANSLATIONS += translations.ts/german_DE.ts
TRANSLATIONS += translations.ts/czech_CS.ts
TRANSLATIONS += translations.ts/slovak_SK.ts

