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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <algorithm>
#include <QTimer>
#include <QString>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QTouchEvent>
#include <QTextStream>
#include <QMainWindow>
#include <QStandardPaths>
#include <QtSensors/QAccelerometer>
#include <QtSensors/QAccelerometerFilter>
#include <QInputDialog>
#include "qcustomplot.h"
#include "accelerometerfilter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private slots:
  void mousePress();
  void mouseWheel();
  void pressedStartStop();
  void pressedAnalyze();
  void pressedSave();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void xAxisChanged(QCPRange range);

private slots:
  void realtimeDataSlot();
  void finishSlot();
  void startSlot();

private:
  void start();
  void delayStart();
  void stop();
  void writeFile(const QString& filename);
  void buttonEnable(QPushButton* button, bool enabled);

private:
  QSettings settings;
  bool first;
  bool started;
  Ui::MainWindow *ui;
  QTimer dataTimer;
  QTimer startTimer;
  QTimer finishTimer;
  QPalette button_enabled, button_disabled;
  QAccelerometer* accelerometer;
  AccelerometerFilter filter;
};

#endif // MAINWINDOW_H
