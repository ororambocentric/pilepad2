#-------------------------------------------------
#
# Project created by QtCreator 2014-07-28T22:39:43
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pilepad
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    editnotedialog.cpp \
    core.cpp \
    note.cpp \
    qmyhighlighter.cpp \
    editorhistory.cpp

HEADERS  += mainwindow.h \
    editnotedialog.h \
    core.h \
    note.h \
    keytranslator.h \
    qmyhighlighter.h \
    editorhistory.h

FORMS    += mainwindow.ui \
    editnotedialog.ui

RESOURCES += \
    resources.qrc
