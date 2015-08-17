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
    bool record;
    std::vector<AccelerometerReading> history;

public:
	AccelerometerFilter();

    // Called when new data arrives
	bool filter(QAccelerometerReading *reading);

    // Get the last reading for displaying
    AccelerometerReadingDisplay get();

    // Get all the data for logging
    std::vector<AccelerometerReading> getAll();

    // Start/stop recording data since we can't start/stop
    // the accelerometer without freezing the application
    void start();
    void stop();
};

#endif // ACCELEROMETERFILTER_H
