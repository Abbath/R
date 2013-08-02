#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:04:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = R
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.hpp

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
