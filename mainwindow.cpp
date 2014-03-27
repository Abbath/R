#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <qwt_scale_draw.h>
//#include <setup.hpp>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);this->showMaximized();
    QFile file("bounds.conf");
    if(file.open(QFile::ReadOnly)){
        QTextStream str(&file);
        int x1, x2, y1, y2;
        str >> x1 >> y1 >> x2 >> y2;
        ui->spinBox_2->setMaximum(x1);
        ui->spinBox_3->setMaximum(y1);
        ui->spinBox_4->setMaximum(x2);
        ui->spinBox_5->setMaximum(y2);
        ui->spinBox_2->setValue(x1);
        ui->spinBox_3->setValue(y1);
        ui->spinBox_4->setValue(x2);
        ui->spinBox_5->setValue(y2);
    }else{
        QMessageBox::information(this, "No config found", "Unable to open config");
    }
    ui->groupBox->hide();
    ui->widget_2->setTitle("Lights");
    ui->widget_2->setAxisTitle(ui->widget_2->xBottom, "Frame");
    ui->widget_2->setAxisTitle(ui->widget_2->yLeft,"Points");
    ui->widget_2->setAxisAutoScale( ui->widget_2->xBottom, true );
    ui->widget_2->setAxisAutoScale( ui->widget_2->yLeft, true );
    pan = new QwtPlotPanner(ui->widget_2->canvas());
    mag = new QwtPlotMagnifier(ui->widget_2->canvas());
    zoom = new QwtPlotZoomer(ui->widget_2->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    QPen pen = QPen( Qt::red );
    curve.setRenderHint( QwtPlotItem::RenderAntialiased );
    curve.setPen( pen );
    curve.attach( ui->widget_2 );
    
    ui->widget_4->setTitle("Lights mean");
    ui->widget_4->setAxisTitle(ui->widget_4->xBottom, "Frame");
    ui->widget_4->setAxisTitle(ui->widget_4->yLeft,"Mean");
    ui->widget_4->setAxisAutoScale( ui->widget_4->xBottom, true );
    ui->widget_4->setAxisAutoScale( ui->widget_4->yLeft, true );
    pan1 = new QwtPlotPanner(ui->widget_4->canvas());
    mag1 = new QwtPlotMagnifier(ui->widget_4->canvas());
    zoom1 = new QwtPlotZoomer(ui->widget_4->canvas());
    zoom1->setRubberBandPen(QPen(Qt::white));
    QPen pen1 = QPen( Qt::red );
    curve1.setRenderHint( QwtPlotItem::RenderAntialiased );
    curve1.setPen( pen1 );
    curve1.attach( ui->widget_4 );
    
    vp = new VideoProcessor();


    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QVector<double>>("QVector<double>");

    connect(vp, SIGNAL(graphL(QVector<int>)), this, SLOT(displayResultsL(QVector<int>)),Qt::QueuedConnection);
    connect(vp, SIGNAL(graphM(QVector<double>)), this, SLOT(displayResultsM(QVector<double>)),Qt::QueuedConnection);

    connect(ui->widget, SIGNAL(displayChanged(Display)), this, SLOT(setDisplay(Display)));
    connect(ui->widget, SIGNAL(rectChanged(QRect)), this, SLOT(setBounds(QRect)));

    connect(vp, SIGNAL(frameChanged(QImage)),ui->widget, SLOT(frameChanged(QImage)),Qt::QueuedConnection);
    connect(this, SIGNAL(stop()), vp, SLOT(stopThis()),Qt::QueuedConnection);
    //ui->horizontalSlider_2->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDisplay(Display dis)
{
    ui->label_top->setNum(static_cast<int>(dis.miny));
    ui->label_bot->setNum(static_cast<int>(dis.maxy));
    ui->label_left->setNum(static_cast<int>(dis.minx));
    ui->label_right->setNum(static_cast<int>(dis.maxx));
    ui->label_light->setNum(static_cast<int>(dis.sum));
    ui->label_mean->setNum(static_cast<double>(dis.mean));
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if(ui->action3D->isChecked()){
        ui->widget_3->setStep((float)value/255.0);
    }
    ui->widget->setThreshold(value);
}

void MainWindow::on_actionOpen_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName( this, tr("Open image file"), "", tr("Image files (*.bmp)"));
    QImage image(fileName);
    ui->spinBox_2->setMaximum(image.width());
    ui->spinBox_3->setMaximum(image.height());
    ui->spinBox_4->setMaximum(image.width());
    ui->spinBox_5->setMaximum(image.height());
    ui->widget->open(fileName);
    //ui->horizontalSlider_2->hide();
}

void MainWindow::on_action3D_triggered(bool checked)
{
    if(checked){
        ui->widget_3->setStep((float)ui->widget->getThreshold()/255.0);
        ui->widget_3->setImage(ui->widget->getImage().copy(ui->widget->getRect()));
    }else{
        QImage empty;
        ui->widget_3->setStep(1.0f);
        ui->widget_3->setImage(empty);
    }
    //ui->widget->set3D();
}

//void MainWindow::on_actionSetup_triggered()
//{
//    //Setup * p = new Setup(0, ui->widget->getSize());
//    if(ui->groupBox->isHidden()){
//        ui->groupBox->show();
//    }else{
//        ui->groupBox->hide();
//    }
//    //p->show();
//}

void MainWindow::on_actionSave_Bounds_triggered()
{
    ui->widget->saveBounds();
}

void MainWindow::on_pushButton_clicked()
{
    //ui->widget->saveBounds();
    QFile file("bounds.conf");
    file.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream str(&file);
    str << ui->spinBox_2->value() << "\n" << ui->spinBox_3->value() << "\n" << ui->spinBox_4->value() << "\n" << ui->spinBox_5->value();
}

void MainWindow::setBounds(QRect rect)
{
    ui->spinBox_2->setValue(rect.left());
    ui->spinBox_3->setValue(rect.top());
    ui->spinBox_4->setValue(rect.right());
    ui->spinBox_5->setValue(rect.bottom());
    if(ui->action3D->isChecked()){
        ui->widget_3->setStep((float)ui->widget->getThreshold()/255.0);
        ui->widget_3->setImage(ui->widget->getImage().copy(ui->widget->getRect()));
    }
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    ui->widget->setX1(arg1);
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    ui->widget->setY1(arg1);
}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    ui->widget->setX2(arg1);
}

void MainWindow::on_spinBox_5_valueChanged(int arg1)
{
    ui->widget->setY2(arg1);
}


void MainWindow::on_actionSetup_triggered(bool checked)
{
    if(checked){
        ui->groupBox->show();
    }else{
        ui->groupBox->hide();
    }
}

void MainWindow::on_actionOpen_Video_triggered()
{
    fileNameV = QFileDialog::getOpenFileName( this, tr("Open data file"), "", tr("Video files (*.avi)"));
    vp->setFilename(fileNameV);
    std::cout << ui->widget->getThreshold() << std::endl;
    vp->setThreshold(ui->widget->getThreshold());
    vp->setRect(ui->widget->getRect());
}

void MainWindow::displayResultsL(const QVector<int> &res)
{
    
    ui->widget_2->detachItems( QwtPlotItem::Rtti_PlotCurve, false );
    ui->widget_2->replot();
    
    QVector < QPointF > points( res.size() );
    quint32 counter = 0;
    auto pointsIt = points.begin();
    
    for ( auto ri = res.constBegin(); ri != res.constEnd(); ++ ri, ++ pointsIt, ++ counter ) {
        (*pointsIt) = QPointF( counter, (*ri) );
    }
    
    QwtPointSeriesData * data = new QwtPointSeriesData(points);
    curve.setData(data);
    curve.attach( ui->widget_2 );
    ui->widget_2->replot();
}

void MainWindow::displayResultsM(const QVector<double> &res)
{
    ui->widget_4->detachItems( QwtPlotItem::Rtti_PlotCurve, false );
    ui->widget_4->replot();
    
    QVector < QPointF > points( res.size() );
    quint32 counter = 0;
    auto pointsIt = points.begin();
    
    for ( auto ri = res.constBegin(); ri != res.constEnd(); ++ ri, ++ pointsIt, ++ counter ) {
        (*pointsIt) = QPointF( counter, (*ri) );
    }
    
    QwtPointSeriesData * data = new QwtPointSeriesData(points);
    curve1.setData(data);
    curve1.attach( ui->widget_4 );
    ui->widget_4->replot();
}

void MainWindow::on_actionRun_triggered()
{
    QThreadPool::globalInstance()->start(vp);
}

/*void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    ui->widget->getFrame(value);
}*/

void MainWindow::on_actionSave_triggered()
{
    ui->widget->saveResults();
}

void MainWindow::on_actionStop_triggered()
{
    emit stop();
}
