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
  void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
  void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
  void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
  void selectionChanged();
  void mousePress();
  void mouseWheel();
  void addRandomGraph();
  void removeSelectedGraph();
  void removeAllGraphs();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void graphClicked(QCPAbstractPlottable *plottable);

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
