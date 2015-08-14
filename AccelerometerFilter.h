#ifndef ACCELEROMETERFILTER_H
#define ACCELEROMETERFILTER_H

#include <array>
#include <QAccelerometerFilter>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>

struct AccelerometerReading
{
    qreal x;
    qreal y;
    qreal z;
    bool newData;

    AccelerometerReading()
        : x(0), y(0), z(0), newData(false) { }

    AccelerometerReading(qreal x, qreal y, qreal z)
        : x(x), y(y), z(z), newData(true) { }
};

class AccelerometerFilter
	: public QObject, public QAccelerometerFilter
{
	Q_OBJECT

    QMutex mutex;

protected:
    AccelerometerReading reading;

public:
	AccelerometerFilter();
	bool filter(QAccelerometerReading *reading);
    AccelerometerReading get();
};

#endif // ACCELEROMETERFILTER_H
