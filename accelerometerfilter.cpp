#include "accelerometerfilter.h"

AccelerometerFilter::AccelerometerFilter()
    : newData(false), record(false)
{
}

bool AccelerometerFilter::filter(QAccelerometerReading *qreading)
{
  QMutexLocker lock(&mutex);

  if (record)
  {
    history.push_back(AccelerometerReading(qreading->x(), qreading->y(), qreading->z()));
    newData = true;
  }

  return false;
}

AccelerometerReadingDisplay AccelerometerFilter::get()
{
    QMutexLocker lock(&mutex);

    if (history.empty())
        return AccelerometerReadingDisplay();

    const AccelerometerReading& last = history.back();
    AccelerometerReadingDisplay r(last.x, last.y, last.z, newData);
    newData = false;
    return r;
}

std::vector<AccelerometerReading> AccelerometerFilter::getAll()
{
    QMutexLocker lock(&mutex);
    return history;
}

void AccelerometerFilter::start()
{
    QMutexLocker lock(&mutex);
    history.clear();
    record = true;
}

void AccelerometerFilter::stop()
{
    QMutexLocker lock(&mutex);
    record = false;
}
