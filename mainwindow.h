#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <numeric>
#include <QTimer>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTouchEvent>
#include <QTextStream>
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
  void pressedStartStop();
  void pressedAnalyze();
  void pressedSave();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void xAxisChanged(QCPRange range);

private slots:
  void realtimeDataSlot();

private:
  void start();
  void stop();
  void writeFile(const QString& filename);
  void buttonEnable(QPushButton* button, bool enabled);

private:
  bool first;
  bool started;
  Ui::MainWindow *ui;
  QTimer dataTimer;
  QPalette button_enabled, button_disabled;
  QAccelerometer* accelerometer;
  AccelerometerFilter filter;
};

#endif // MAINWINDOW_H
