#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <helpers.hpp>
#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <imagearea.hpp>
#include <videoprocessor.h>
#include <qwt_plot.h>
#include <qwt.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
//#include <QtWidgets>


namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    //void keyPressEvent(QKeyEvent *e);
    ~MainWindow();
public slots:
    void setDisplay(Display dis);
    void setBounds(QRect rect);
private slots:
    void on_horizontalSlider_valueChanged(int value);
    void on_actionOpen_triggered();
    void on_action3D_triggered(bool checked);
    void on_actionSave_Bounds_triggered();
    void on_pushButton_clicked();
    void on_spinBox_2_valueChanged(int arg1);
    void on_spinBox_3_valueChanged(int arg1);
    void on_spinBox_4_valueChanged(int arg1);
    void on_spinBox_5_valueChanged(int arg1);
    void on_actionSetup_triggered(bool checked);
    void on_actionOpen_Video_triggered();
    void displayResultsL(const QVector<int> &res);
    void displayResultsM(const QVector<double> &res);
    void on_actionRun_triggered();
    void on_actionSave_triggered();

 //   void on_horizontalSlider_2_valueChanged(int value);

    void on_actionStop_triggered();

private:
    QwtPlotPanner *pan;
    QwtPlotMagnifier *mag;
    QwtPlotZoomer *zoom;
    QwtPlotCurve curve;
    
    QwtPlotPanner *pan1;
    QwtPlotMagnifier *mag1;
    QwtPlotZoomer *zoom1;
    QwtPlotCurve curve1;
    
    Ui::MainWindow *ui;
    VideoProcessor *vp;

    QString fileNameV;
signals:
    void stop();
};

#endif // MAINWINDOW_HPP
