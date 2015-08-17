#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  first(true),
  started(true)
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

  // Buttons
  connect(ui->buttonStartStop, SIGNAL(released()), this, SLOT(pressedStartStop()));
  connect(ui->buttonAnalyze, SIGNAL(released()), this, SLOT(pressedAnalyze()));
  connect(ui->buttonSave, SIGNAL(released()), this, SLOT(pressedSave()));

  // Setup accelerometer
  accelerometer = new QAccelerometer(this);
  accelerometer->setAccelerationMode(QAccelerometer::Combined);
  accelerometer->setDataRate(30);
  accelerometer->addFilter(&filter);

  // We'll always keep it running since on the phone apparently calling
  // stop makes the whole application freeze. Thus, we'll just start/stop
  // recording data not actually requesting data from the accelerometer
  accelerometer->start();

  // Setup a timer that repeatedly calls MainWindow::realtimeDataSlot
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));

  // Button enabled/disabled coloring
  button_enabled = ui->buttonStartStop->palette();
  button_disabled = ui->buttonStartStop->palette();
  button_enabled.setColor(QPalette::Button, QColor(Qt::white));
  button_disabled.setColor(QPalette::Button, QColor(Qt::lightGray));

  start();
}

MainWindow::~MainWindow()
{
  delete accelerometer;
  delete ui;
}

void MainWindow::buttonEnable(QPushButton* button, bool enabled)
{
  if (!button)
      return;

  button->setEnabled(enabled);
  button->setAutoFillBackground(true);
  button->setPalette((enabled)?button_enabled:button_disabled);
  button->update();
}

void MainWindow::start()
{
  ui->customPlot->graph(0)->removeDataAfter(0);
  ui->customPlot->graph(1)->removeDataAfter(0);
  ui->customPlot->graph(2)->removeDataAfter(0);

  first = true;

  filter.start();
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::stop()
{
  filter.stop();
  dataTimer.stop();
  buttonEnable(ui->buttonStartStop, true);
  buttonEnable(ui->buttonAnalyze, true);
  buttonEnable(ui->buttonSave, true);
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
  if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
    menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    menu->popup(ui->customPlot->mapToGlobal(pos));
  }
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

void MainWindow::realtimeDataSlot()
{
  if (first)
  {
    // These are here instead of in start() so that they actually do something
    // when the application is first started up.
    buttonEnable(ui->buttonStartStop, true);
    buttonEnable(ui->buttonAnalyze, false);
    buttonEnable(ui->buttonSave, false);
  }

  bool newData = false;
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
  static double lastPointKey = 0;

  if (key-lastPointKey > 0.01) // at most add point every 10 ms
  {
    AccelerometerReadingDisplay reading = filter.get();
    newData = reading.newData;

    if (reading.newData)
    {
      ui->customPlot->graph(0)->addData(key, reading.x);
      ui->customPlot->graph(1)->addData(key, reading.y);
      ui->customPlot->graph(2)->addData(key, reading.z);

      // remove data older than some minutes
      /*
      ui->customPlot->graph(0)->removeDataBefore(key-300);
      ui->customPlot->graph(1)->removeDataBefore(key-300);
      ui->customPlot->graph(2)->removeDataBefore(key-300);
      */

      lastPointKey = key;
    }
  }

  if (ui->customPlot->getPaused() == 0)
  {
    // make key axis range scroll with the data (at a constant range size of 8)
    // but only if we haven't scrolled to the left
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

void MainWindow::pressedStartStop()
{
    if (started)
    {
        started = false;
        stop();
        ui->buttonStartStop->setText("Start");
    }
    else
    {
        started = true;
        start();
        ui->buttonStartStop->setText("Stop");
    }
}

void MainWindow::pressedAnalyze()
{
    buttonEnable(ui->buttonStartStop, false);
    buttonEnable(ui->buttonAnalyze, false);
    buttonEnable(ui->buttonSave, false);

    std::vector<AccelerometerReading> history = filter.getAll();
    QString msg;

    if (history.empty())
    {
        msg = "No data.";
    }
    else
    {
      double sum_x = std::accumulate(history.begin(), history.end(), 0.0,
          [](double result, const AccelerometerReading& r) {
          return result + r.x;
      });
      double sum_y = std::accumulate(history.begin(), history.end(), 0.0,
          [](double result, const AccelerometerReading& r) {
          return result + r.y;
      });
      double sum_z = std::accumulate(history.begin(), history.end(), 0.0,
          [](double result, const AccelerometerReading& r) {
          return result + r.z;
      });
      double avg_x = sum_x/history.size();
      double avg_y = sum_y/history.size();
      double avg_z = sum_z/history.size();
      double sum_xm2 = std::accumulate(history.begin(), history.end(), 0.0,
          [avg_x](double result, const AccelerometerReading& r) {
          return result + std::pow(1.0*r.x - avg_x, 2);
      });
      double sum_ym2 = std::accumulate(history.begin(), history.end(), 0.0,
          [avg_y](double result, const AccelerometerReading& r) {
          return result + std::pow(1.0*r.y - avg_y, 2);
      });
      double sum_zm2 = std::accumulate(history.begin(), history.end(), 0.0,
          [avg_z](double result, const AccelerometerReading& r) {
          return result + std::pow(1.0*r.z - avg_z, 2);
      });
      double stdev_s_x = 0;
      double stdev_s_y = 0;
      double stdev_s_z = 0;

      if (history.size() > 1)
      {
        stdev_s_x = std::sqrt(1.0/(history.size()-1)*sum_xm2);
        stdev_s_y = std::sqrt(1.0/(history.size()-1)*sum_ym2);
        stdev_s_z = std::sqrt(1.0/(history.size()-1)*sum_zm2);
      }

      QTextStream s(&msg);
      s << "Averages (m/s^2):" << endl
        << " X " << QString::number(avg_x, 'f', 8) << endl
        << " Y " << QString::number(avg_y, 'f', 8) << endl
        << " Z " << QString::number(avg_z, 'f', 8) << endl
        << endl
        << "Sample Stdev (m/s^2):" << endl
        << " X " << QString::number(stdev_s_x, 'f', 8) << endl
        << " Y " << QString::number(stdev_s_y, 'f', 8) << endl
        << " Z " << QString::number(stdev_s_z, 'f', 8) << endl;
    }

    QMessageBox m;
    m.setText(msg);
    m.setStandardButtons(QMessageBox::Ok);
    m.setDefaultButton(QMessageBox::Ok);
    m.exec();

    buttonEnable(ui->buttonStartStop, true);
    buttonEnable(ui->buttonAnalyze, true);
    buttonEnable(ui->buttonSave, true);
}

void MainWindow::pressedSave()
{
  buttonEnable(ui->buttonStartStop, false);
  buttonEnable(ui->buttonAnalyze, false);
  buttonEnable(ui->buttonSave, false);

  // Note, at the moment we get the following message and this is empty.
  //   io\qstandardpaths_winrt.cpp:118: class QString __cdecl
  //   QStandardPaths::writableLocation(enum QStandardPaths::
  //   StandardLocation): Unimplemented code.
  QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

  if (!path.isEmpty())
    path += QDir::separator();

  QString date = QDateTime::currentDateTime().toString("yyyyMMdd_hhmm");
  QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                        path + "accelerometer_" + date + ".csv", tr("*.csv"));

  if (!filename.isEmpty())
  {
    buttonEnable(ui->buttonSave, false);
    writeFile(filename);
  }

  buttonEnable(ui->buttonStartStop, true);
  buttonEnable(ui->buttonAnalyze, true);
  buttonEnable(ui->buttonSave, true);
}

void MainWindow::writeFile(const QString& filename)
{
  QFile file(filename);

  if (file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
  {
    QTextStream f(&file);
    std::vector<AccelerometerReading> history = filter.getAll();

    f << "timestamp, x (m/s^2), y (m/s^2), z (m/s^2)" << endl;

    for (std::vector<AccelerometerReading>::const_iterator i = history.begin();
         i != history.end(); ++i)
      f << i->time << ", " << i->x << ", " << i->y << ", " << i->z << endl;
  }
  else
  {
      QMessageBox m;
      m.setText("Could not save file.");
      m.setStandardButtons(QMessageBox::Ok);
      m.setDefaultButton(QMessageBox::Ok);
      m.exec();
  }

  buttonEnable(ui->buttonSave, true);
}
