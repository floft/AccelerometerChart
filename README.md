Accelerometer Chart
===================

This is a simple GPL strip chart accelerometer app for Windows.

### Building ###

 * Plug in the phone to USB. Open Visual Studio 2013.
   Tools --> Windows Phone 8.1 --> Developer Unlock.
   Click Register.

 * Download and install
   [Qt 5.5.0 for Windows RT 32-bit](http://www.qt.io/download-open-source/#section-2)
   or if you already have Qt installed, add the Windows Phone arm and
   Windows Runtime 8.1 x64 components.

#### Using Qt Creator ####

 * Enable WinRT plugin in
   Qt Creator --> Help --> About Plugins --> Device Support --> WinRt.
   Restart Qt Creator.

 * Open AccelerometerChart.pro, select whether to build for desktop or phone.

#### Using Visual Studio ####

 * Open a bash prompt in the directory, run the following (or a command prompt
   and change the command accordingly) after changing the path to wherever you
   installed Qt for WinRT:

       QTDIR="/c/Qt/Qt5.5.0/5.5/winphone\_arm/bin"
       PATH="$PATH:/c/Program Files/Microsoft SDKs/Windows/v6.0A/Bin"
       "C:\Qt\Qt5.5.0\5.5\winphone\_arm\bin\qmake.exe"
       -spec winphone-arm-msvc2013 -tp vc AccelerometerChart.pro
       "CONFIG+=windeployqt"

 * Make sure you don't have qmake from a different Qt installation in your path
   and then open and build the created project in Visual Studio.

### Examples Used ###

 * [QCustomPlot Interaction](http://qcustomplot.com/index.php/demos/interactionexample)
   for initial setup of the plot

 * [QCustomPlot Realtime Data](http://qcustomplot.com/index.php/demos/realtimedatademo)
   for plotting live data

 * [Qt Pinch Zoom](http://qt.apidoc.info/4.8.5/touch-pinchzoom-graphicsview-cpp.html)
   for improved QCustomPlot touchscreen support

 * [Qt Bubble Level](http://qt.apidoc.info/4.7.4/demos-mobile-qtbubblelevel.html)
   for accelerometer readings

### Known Bugs ###

 * Native file dialog is not shown on Windows Phone 8.1 because Qt for WinRT
   is using PickSaveFileAsync rather than PickSaveFileAndContinue, and so
   saving files is limited, see
   [QTBUG-47802](https://bugreports.qt.io/browse/QTBUG-47802) or
   [QTBUG-44387](https://bugreports.qt.io/browse/QTBUG-44387).
