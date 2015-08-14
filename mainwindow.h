#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QTouchEvent>
#include <QMainWindow>
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
  void saveFile();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void xAxisChanged(QCPRange range);

private slots:
  void realtimeDataSlot();
  
private:
  Ui::MainWindow *ui;
  QTimer dataTimer;
  QAccelerometer* accelerometer;
  AccelerometerFilter filter;
};

#endif // MAINWINDOW_H
