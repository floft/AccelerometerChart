/*
 * Accelerometer Chart
 * Copyright (C) 2015 Solution Engineering, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

bool AccelerometerFilter::empty()
{
    QMutexLocker lock(&mutex);
    return history.empty();
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
