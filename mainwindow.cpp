#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  srand(QDateTime::currentDateTime().toTime_t());
  ui->setupUi(this);
  
  ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                  QCP::iSelectLegend | QCP::iSelectPlottables);

  // Axes
  ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  ui->customPlot->xAxis->setAutoTickStep(false);
  ui->customPlot->xAxis->setTickStep(2);
  ui->customPlot->xAxis->setLabel("Time");
  ui->customPlot->xAxis->setLabelPadding(0);
  ui->customPlot->xAxis->setSelectableParts(QCPAxis::spAxis);

  ui->customPlot->yAxis->setRange(-20, 20);
  ui->customPlot->yAxis->setLabel("Acceleration (m/s^2)");
  ui->customPlot->yAxis->setLabelPadding(0);
  ui->customPlot->yAxis->setSelectableParts(QCPAxis::spAxis);

  ui->customPlot->axisRect()->setupFullAxesBox();

  // Legend
  ui->customPlot->legend->setVisible(true);
  QFont legendFont = font();
  legendFont.setPointSize(10);
  ui->customPlot->legend->setFont(legendFont);
  ui->customPlot->legend->setSelectedFont(legendFont);
  ui->customPlot->legend->setSelectableParts(QCPLegend::spNone);
  
  // X, Y, Z reading graphs
  ui->customPlot->addGraph(); // blue line
  ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
  ui->customPlot->graph(0)->setName("X");
  ui->customPlot->graph(0)->setSelectable(false);
  ui->customPlot->addGraph(); // red line
  ui->customPlot->graph(1)->setPen(QPen(Qt::red));
  ui->customPlot->graph(1)->setName("Y");
  ui->customPlot->graph(1)->setSelectable(false);
  ui->customPlot->addGraph(); // green line
  ui->customPlot->graph(2)->setPen(QPen(Qt::green));
  ui->customPlot->graph(2)->setName("Z");
  ui->customPlot->graph(2)->setSelectable(false);

  // Scale in a particular direction when an axis is selected
  connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
  
  // Make bottom and left axes transfer their ranges to top and right axes:
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // Adjust the tick marks on x-axis scale
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));

  // Context menu popup
  ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

  // Setup accelerometer
  accelerometer = new QAccelerometer(this);
  accelerometer->setAccelerationMode(QAccelerometer::Combined);
  accelerometer->setDataRate(30);
  accelerometer->addFilter(&filter);
  accelerometer->start();

  // Setup a timer that repeatedly calls MainWindow::realtimeDataSlot
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

MainWindow::~MainWindow()
{
  delete accelerometer;
  delete ui;
}

void MainWindow::xAxisChanged(QCPRange range)
{
  double diff = range.upper - range.lower;
  ui->customPlot->xAxis->setTickStep(diff/5);

  if (diff < 1)
    ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss.zzz");
  else
    ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
}

/*void MainWindow::setFilename(QMouseEvent* event, QCPPlotTitle* title)
{
  Q_UNUSED(event)
  // Set the plot title by double clicking on it
  bool ok;
  QString fileName = QInputDialog::getText(this, "Qt Accelerometer", "Filename:", QLineEdit::Normal, title->text(), &ok);
  if (ok)
  {
    //title->setText(newTitle);
    ui->customPlot->replot();
  }
}*/

void MainWindow::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
  
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::contextMenuRequest(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
    menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
  } else  // general context menu on graphs requested
  {
    menu->addAction("Save to file", this, SLOT(saveFile()));
  }

  menu->popup(ui->customPlot->mapToGlobal(pos));
}

void MainWindow::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      ui->customPlot->replot();
    }
  }
}

void MainWindow::saveFile()
{
}

void MainWindow::realtimeDataSlot()
{
  bool newData = false;
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
  static double lastPointKey = 0;

  if (key-lastPointKey > 0.01) // at most add point every 10 ms
  {
    AccelerometerReading reading = filter.get();
    newData = reading.newData;

    if (reading.newData)
    {
      ui->customPlot->graph(0)->addData(key, reading.x);
      ui->customPlot->graph(1)->addData(key, reading.y);
      ui->customPlot->graph(2)->addData(key, reading.z);

      // remove data older than some minutes
      ui->customPlot->graph(0)->removeDataBefore(key-300);
      ui->customPlot->graph(1)->removeDataBefore(key-300);
      ui->customPlot->graph(2)->removeDataBefore(key-300);

      lastPointKey = key;
    }
  }

  if (ui->customPlot->getPaused() == 0)
  {
    // make key axis range scroll with the data (at a constant range size of 8)
    // but only if we haven't scrolled to the left
    static bool first = true;
    QCPRange xrange = ui->customPlot->xAxis->range();
    if ((xrange.upper >= key-0.5 && xrange.upper <= key+0.5) || first)
      ui->customPlot->xAxis->setRange(key+0.25, xrange.upper-xrange.lower, Qt::AlignRight);
    first = false;

    ui->customPlot->replot();
  }

  // Calculate frames per second and readings per second
  static double lastFpsKey = 0;
  static int frameCount = 0;
  static int readingCount = 0;
  ++frameCount;

  if (newData)
    ++readingCount;

  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
    ui->statusBar->showMessage(
          QString("%1 FPS, %2 RPS, Readings: %3")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(readingCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->customPlot->graph(0)->data()->count())
          , 0);
    lastFpsKey = key;
    frameCount = 0;
    readingCount = 0;
  }
}
