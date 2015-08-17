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

winrt {
    WINRT_MANIFEST.version = 1.0.0.0
    WINRT_MANIFEST.name = Accelerometer Chart
    WINRT_MANIFEST.publisher = Solution Engineering, Inc.
    WINRT_MANIFEST.description = A simple GPL strip chart accelerometer
}
