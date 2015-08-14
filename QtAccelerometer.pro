QT       += core gui sensors
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtAccelerometer
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui
