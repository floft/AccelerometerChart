#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QTouchEvent>
#include <QMainWindow>
#include <QtSensors/QAccelerometer>
#include <QInputDialog>
#include "qcustomplot.h"

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
  void saveFile();
  void contextMenuRequest(QPoint pos);
  void moveLegend();

private slots:
  void realtimeDataSlot();
  void filter(QAccelerometerReading* reading);
  
private:
  qreal x, y, z;
  Ui::MainWindow *ui;
  QTimer dataTimer;
  QAccelerometer* accelerometer;
};

#endif // MAINWINDOW_H
