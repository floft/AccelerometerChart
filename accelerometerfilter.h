#ifndef ACCELEROMETERFILTER_H
#define ACCELEROMETERFILTER_H

#include <array>
#include <QString>
#include <QDateTime>
#include <QAccelerometerFilter>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>

// Only for displaying
struct AccelerometerReadingDisplay
{
    qreal x;
    qreal y;
    qreal z;
    bool newData;

    AccelerometerReadingDisplay()
        : x(0), y(0), z(0), newData(false) { }

    AccelerometerReadingDisplay(qreal x, qreal y,
                                qreal z, bool newData)
        : x(x), y(y), z(z), newData(newData) { }
};

// Actually stored
struct AccelerometerReading
{
    qreal x;
    qreal y;
    qreal z;
    QString time;

    AccelerometerReading()
        : x(0), y(0), z(0) { }

    AccelerometerReading(qreal x, qreal y, qreal z)
        : x(x), y(y), z(z),
          time(QDateTime::currentDateTime().
               toString("yyyy/MM/dd hh:mm:ss.zzz")) { }
};

class AccelerometerFilter
	: public QObject, public QAccelerometerFilter
{
	Q_OBJECT

    QMutex mutex;
    bool newData;
    std::vector<AccelerometerReading> history;

public:
	AccelerometerFilter();

    // Called when new data arrives
	bool filter(QAccelerometerReading *reading);

    // Get the last reading for displaying
    AccelerometerReadingDisplay get();

    // Get all the data for logging
    std::vector<AccelerometerReading> getAll();

    // Clear all the data when starting a new recording session
    void clear() { history.clear(); }
};

#endif // ACCELEROMETERFILTER_H
