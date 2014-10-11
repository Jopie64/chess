#-------------------------------------------------
#
# Project created by QtCreator 2014-10-11T20:43:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Chess
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=gnu++0x

SOURCES += main.cpp \
    chessboard.cpp

HEADERS += \
    chessboard.h
