#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include "imagearea.hpp"
#include "processor.hpp"
#include <qwt_plot.h>
#include <qwt.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
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
    
    struct QwtToolSet {
        QwtPlotMagnifier *mag;
        QwtPlotZoomer *zoom;
        QwtPlotCurve curve;
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void setProcessor(Processor * p) { vp = p; }
    void setBounds(QRect rect);
    void setMaxMinBounds(QRect rect);
private slots:
    void on_horizontalSlider_valueChanged(int value);
    void on_actionOpen_triggered();
    void on_action3D_triggered(bool checked);
    void on_pushButton_clicked();
    void on_spinBox_X1_valueChanged(int arg1);
    void on_spinBox_Y1_valueChanged(int arg1);
    void on_spinBox_X2_valueChanged(int arg1);
    void on_spinBox_Y2_valueChanged(int arg1);
    void on_actionSetup_triggered(bool checked);
    void on_actionOpen_Video_triggered();
    void displayResultsL(const QVector<int> &lightPixelsNumbers,const QVector<double> &t);
    void displayResultsM(const QVector<double> &lightPixelsNumbers,const QVector<double> &t);
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

signals:
    void stop();

private:
    QVector<int> lightPixelsNumbers;
    QVector<double> lightPixelsMeans;
    
    QwtToolSet lightsNumbersPlot;
    QwtToolSet lightsMeansPlot;
    
    Ui::MainWindow *ui;
    Processor *vp;

    QString imageFileName;
    QString videoFileName;
    
    bool isRunning;
    void initPlot(QwtPlot *plot, QwtToolSet& toolset, QString title, QString xlabel, QString ylabel);
};

#endif // MAINWINDOW_HPP
