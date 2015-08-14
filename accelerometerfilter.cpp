#include "accelerometerfilter.h"

AccelerometerFilter::AccelerometerFilter()
{
}

bool AccelerometerFilter::filter(QAccelerometerReading *qreading)
{
  QMutexLocker lock(&mutex);
  reading = AccelerometerReading(qreading->x(), qreading->y(), qreading->z());
  return false;
}

AccelerometerReading AccelerometerFilter::get()
{
    QMutexLocker lock(&mutex);
    AccelerometerReading r = reading;
    reading.newData = false;
    return r;
}
