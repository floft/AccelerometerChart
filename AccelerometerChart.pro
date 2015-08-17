QT       += core gui sensors
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AccelerometerChart
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    qcustomplot.cpp \
    accelerometerfilter.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    accelerometerfilter.h

FORMS    += mainwindow.ui
