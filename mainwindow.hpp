#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "imagearea.hpp"
#include "videoprocessor.hpp"
#include "streamprocessor.hpp"
#include "imageprocessor.hpp"
#include "capturewrapper.hpp"
#include "imagestorage.hpp"
#include "utils.hpp"
#include <qwt_plot.h>
#include <qwt.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


namespace Ui {
class MainWindow;
}
/*!
 * \brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    typedef std::vector<cv::Point> Contour;
    typedef std::vector<Contour> Contours;
    
    struct QwtToolSet {
        QwtPlotMagnifier *mag;
        QwtPlotZoomer *zoom;
        QwtPlotCurve curve;
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openVideo();
    void openImage();
    QString seconds2HMS(double value);
public slots:
    void setProcessor(VideoProcessor * p) { videoProcessor = p; }
    void setBounds(QRect rect);
    void histogram(QImage hist);
private slots:
    void on_horizontalSlider_valueChanged(int value);
    void on_actionOpen_triggered();
    void on_action3D_triggered(bool checked);
    void on_spinBox_X1_valueChanged(int arg1);
    void on_spinBox_Y1_valueChanged(int arg1);
    void on_spinBox_X2_valueChanged(int arg1);
    void on_spinBox_Y2_valueChanged(int arg1);
    void on_actionSetup_triggered(bool checked);
    void on_actionOpen_Video_triggered();
    void plotResults(std::shared_ptr<Results> r);    
    void on_actionRun_triggered();
    void on_actionSave_triggered();
    void on_actionStop_triggered();  
    void progress(int value);
    void time(double value);
    void on_doubleSpinBox_2_valueChanged(double arg1);
    void on_doubleSpinBox_3_valueChanged(double arg1);
    void on_actionQuit_triggered();
    void detection();
    void on_actionAutodetection_triggered(bool checked);
    void on_actionAbout_triggered();
    void sensChanged(int value);
    void periodChanged(double value);
    void on_actionCapture_Device_triggered();
    
    void on_actionSnapshot_triggered();
    
    void on_groupBox_4_toggled(bool arg1);
    
signals:
    void stop();

private:
//    QSpinBox* sens;
//    QDoubleSpinBox* period;
    QVector<int> lightPixelsNumbers;
    QVector<double> lightPixelsMeans;
    unsigned fps;
    
    QwtToolSet lightsNumbersPlot;
    QwtToolSet lightsMeansPlot;
    
    Ui::MainWindow *ui;
    VideoProcessor *videoProcessor;
    ImageProcessor *imageProcessor;
    StreamProcessor *streamProcessor;

    QString imageFileName;
    QString videoFileName;
    
    bool isRunning;
    void initPlot(QwtPlot *plot, QwtToolSet& toolset, QString title, QString xlabel, QString ylabel);
    void writeSettings();
    void readSettings();
    QStringList countCameras();
};

#endif // MAINWINDOW_HPP
