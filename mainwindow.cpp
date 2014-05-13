#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <qwt_scale_draw.h>
#include <iostream>

/*!
 * \brief MainWindow::MainWindow
 * \param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    this->showMaximized();
    
    ui->groupBox->hide();
    ui->progressBar->hide();
    ui->label_8->hide();
    ui->l_plot->setTitle("Lights");
    ui->l_plot->setAxisTitle(ui->l_plot->xBottom, "Frame");
    ui->l_plot->setAxisTitle(ui->l_plot->yLeft,"Points");
    ui->l_plot->setAxisAutoScale( ui->l_plot->xBottom, true );
    ui->l_plot->setAxisAutoScale( ui->l_plot->yLeft, true );
    mag = new QwtPlotMagnifier(ui->l_plot->canvas());
    zoom = new QwtPlotZoomer(ui->l_plot->canvas());
    zoom->setRubberBandPen(QPen(Qt::white));
    curve.setRenderHint( QwtPlotItem::RenderAntialiased );
    curve.setPen( QPen( Qt::red ) );
    curve.attach( ui->l_plot );
    
    ui->m_plot->setTitle("Lights mean");
    ui->m_plot->setAxisTitle(ui->m_plot->xBottom, "Frame");
    ui->m_plot->setAxisTitle(ui->m_plot->yLeft,"Mean");
    ui->m_plot->setAxisAutoScale( ui->m_plot->xBottom, true );
    ui->m_plot->setAxisAutoScale( ui->m_plot->yLeft, true );
    mag1 = new QwtPlotMagnifier(ui->m_plot->canvas());
    zoom1 = new QwtPlotZoomer(ui->m_plot->canvas());
    zoom1->setRubberBandPen( QPen( Qt::white ) );
    curve1.setRenderHint( QwtPlotItem::RenderAntialiased );
    curve1.setPen( QPen( Qt::red ) );
    curve1.attach( ui->m_plot );
    
    vp = new Processor();
    
    ui->imagearea->readConfig("bounds.conf");
    QRect r = ui->imagearea->getRect();
    ui->spinBox_X1->setMaximum(r.left());
    ui->spinBox_Y1->setMaximum(r.top());
    ui->spinBox_X2->setMaximum(r.right());
    ui->spinBox_Y2->setMaximum(r.bottom());
    ui->spinBox_X1->setValue(r.left());
    ui->spinBox_Y1->setValue(r.top());
    ui->spinBox_X2->setValue(r.right());
    ui->spinBox_Y2->setValue(r.bottom());
    
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QVector<double>>("QVector<double>");
    
    connect(vp, SIGNAL(graphL(QVector<int>,QVector<double>)), this, SLOT(displayResultsL(QVector<int>,QVector<double>)),Qt::QueuedConnection);
    connect(vp, SIGNAL(graphM(QVector<double>,QVector<double>)), this, SLOT(displayResultsM(QVector<double>,QVector<double>)),Qt::QueuedConnection);
    
    connect(vp, SIGNAL(frameChanged(QImage)),ui->imagearea, SLOT(frameChanged(QImage)),Qt::QueuedConnection);
    connect(this, SIGNAL(stop()), vp, SLOT(stopThis()),Qt::QueuedConnection);
    connect(vp, SIGNAL(maxMinBounds(QRect)),this, SLOT(setMaxMinBounds(QRect)),Qt::QueuedConnection);
    connect(vp, SIGNAL(progress(int)), this,SLOT(progress(int)), Qt::QueuedConnection);
    connect(vp, SIGNAL(time(double)), this, SLOT(time(double)));
    this->showFullScreen();
}

/*!
 * \brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * \brief MainWindow::on_horizontalSlider_valueChanged
 * \param value
 */
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if(ui->action3D->isChecked()){
        ui->widget_3d->setStep((float)value/255.0);
    }else{
        vp->setThreshold(value);
        if(!filename.isNull()){
            QImage image(filename);
            QPair<int, double> id = vp->processImage(image);
            ui->label_light->setNum(id.first);
            ui->label_mean->setNum(id.second);
        }else{
            QPair<int,double> id = vp->processImage(ui->imagearea->getImage());
            ui->label_light->setNum(id.first);
            ui->label_mean->setNum(id.second);
        }
    }
}

/*!
 * \brief MainWindow::on_actionOpen_triggered
 */
void MainWindow::on_actionOpen_triggered()
{
    filename = QFileDialog::getOpenFileName( this, tr("Open image file"), "", tr("Image files (*.bmp)"));
    QImage image(filename);
    ui->spinBox_X1->setMaximum(image.width());
    ui->spinBox_Y1->setMaximum(image.height());
    ui->spinBox_X2->setMaximum(image.width());
    ui->spinBox_Y2->setMaximum(image.height());
    vp->setThreshold(ui->spinBox->value());
    vp->setRect(ui->imagearea->getRect());
    QPair<int,double> id = vp->processImage(image);
    ui->label_light->setNum(id.first);
    ui->label_mean->setNum(id.second);
    ui->imagearea->open(filename);
}

/*!
 * \brief MainWindow::on_action3D_triggered
 * \param checked
 */
void MainWindow::on_action3D_triggered(bool checked)
{
    /*if(checked){
        ui->widget_3d->setStep((float)ui->spinBox->value()/255.0);
        ui->widget_3d->setImage(ui->imagearea->getImage().copy(ui->imagearea->getRect()));
        std::cout << ui->imagearea->getImage().width();
    }else{
        QImage empty;
        ui->widget_3d->setStep(1.0f);
        ui->widget_3d->setImage(empty);
    }*/
}

/*!
 * \brief MainWindow::on_pushButton_clicked
 */
void MainWindow::on_pushButton_clicked()
{
    QFile file("bounds.conf");
    file.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream str(&file);
    str << ui->spinBox_X1->value() << "\n" << ui->spinBox_Y1->value() << "\n" << ui->spinBox_X2->value() << "\n" << ui->spinBox_Y2->value();
}

/*!
 * \brief MainWindow::setBounds
 * \param rect
 */
void MainWindow::setBounds(QRect rect)
{
    ui->spinBox_X1->setValue(rect.left());
    ui->spinBox_Y1->setValue(rect.top());
    ui->spinBox_X2->setValue(rect.right());
    ui->spinBox_Y2->setValue(rect.bottom());
    if(ui->action3D->isChecked()){
        ui->widget_3d->setStep((float)ui->spinBox->value()/255.0);
        ui->widget_3d->setImage(ui->imagearea->getImage().copy(ui->imagearea->getRect()));
    }
    vp->setThreshold(ui->spinBox->value());
    vp->setRect(rect);
    
    if(!filename.isNull()){
        QImage image(filename);
        QPair<int, double> id = vp->processImage(image);
        ui->label_light->setNum(id.first);
        ui->label_mean->setNum(id.second); 
    }else{
        QPair<int,double> id = vp->processImage(ui->imagearea->getImage());
        ui->label_light->setNum(id.first);
        ui->label_mean->setNum(id.second);
    }
}

void MainWindow::setMaxMinBounds(QRect rect)
{
    if(!filename.isNull()){
        ui->label_left->setNum(rect.left());
        ui->label_top->setNum(rect.top());
        ui->label_right->setNum(rect.right());
        ui->label_bot->setNum(rect.bottom());
    }
}

/*!
 * \brief MainWindow::on_spinBox_2_valueChanged
 * \param arg1
 */
void MainWindow::on_spinBox_X1_valueChanged(int arg1)
{
    ui->imagearea->setX1(arg1);
}

/*!
 * \brief MainWindow::on_spinBox_3_valueChanged
 * \param arg1
 */
void MainWindow::on_spinBox_Y1_valueChanged(int arg1)
{
    ui->imagearea->setY1(arg1);
}

void MainWindow::on_spinBox_X2_valueChanged(int arg1)
{
    ui->imagearea->setX2(arg1);
}

/*!
 * \brief MainWindow::on_spinBox_5_valueChanged
 * \param arg1
 */
void MainWindow::on_spinBox_Y2_valueChanged(int arg1)
{
    ui->imagearea->setY2(arg1);
}


/*!
 * \brief MainWindow::on_actionSetup_triggered
 * \param checked
 */
void MainWindow::on_actionSetup_triggered(bool checked)
{
    if(checked){
        ui->groupBox->show();
    }else{
        ui->groupBox->hide();
    }
}

/*!
 * \brief MainWindow::on_actionOpen_Video_triggered
 */
void MainWindow::on_actionOpen_Video_triggered()
{
    filename.clear();
    fileNameV = QFileDialog::getOpenFileName( this, tr("Open data file"), "", tr("Video files (*.avi)"));
    CvCapture * capture = cvCaptureFromAVI(fileNameV.toStdString().c_str());
    if(!capture)
    {
        QMessageBox::warning(0, "Error", "Capture From AVI failed (file not found?)\n");
    }else{
        vp->setFilename(fileNameV);
        IplImage* frame = cvQueryFrame(capture);
        QImage image = vp->IplImage2QImage(frame);
        ui->imagearea->loadImage(image.mirrored(false,true));
    }
}

/*!
 * \brief MainWindow::displayResultsL
 * \param res
 */
void MainWindow::displayResultsL(const QVector<int> &res, const QVector<double> &t)
{
    this->res = res;
    ui->l_plot->detachItems( QwtPlotItem::Rtti_PlotCurve, false );
    ui->l_plot->replot();
    
    QVector < QPointF > points( res.size() );
    quint32 counter = 0;
    auto pointsIt = points.begin();
    
    for ( auto ri = res.constBegin(); ri != res.constEnd(); ++ ri, ++ pointsIt, ++ counter ) {
        (*pointsIt) = QPointF( t[counter], (*ri) );
    }
    
    QwtPointSeriesData * data = new QwtPointSeriesData(points);
    curve.setData(data);
    curve.attach( ui->l_plot );
    ui->l_plot->replot();
}

/*!
 * \brief MainWindow::displayResultsM
 * \param res
 */
void MainWindow::displayResultsM(const QVector<double> &res, const QVector<double> &t)
{
    this->resm = res;
    ui->m_plot->detachItems( QwtPlotItem::Rtti_PlotCurve, false );
    ui->m_plot->replot();
    
    QVector < QPointF > points( res.size() );
    quint32 counter = 0;
    auto pointsIt = points.begin();
    
    for ( auto ri = res.constBegin(); ri != res.constEnd(); ++ ri, ++ pointsIt, ++ counter ) {
        (*pointsIt) = QPointF( t[counter], (*ri) );
    }
    
    QwtPointSeriesData * data = new QwtPointSeriesData(points);
    curve1.setData(data);
    curve1.attach( ui->m_plot );
    ui->m_plot->replot();
}

/*!
 * \brief MainWindow::on_actionRun_triggered
 */
void MainWindow::on_actionRun_triggered()
{
    ui->progressBar->show();
    ui->label_8->show();
    if(!fileNameV.isNull()){
        vp->setThreshold(ui->spinBox->value());
        vp->setRect(ui->imagearea->getRect());
        QThreadPool::globalInstance()->start(vp);
    }
}

/*!
 * \brief MainWindow::on_actionSave_triggered
 */
void MainWindow::on_actionSave_triggered()
{
    // ui->imagearea->saveResults();
    QString name = QFileDialog::getSaveFileName(this, "Save data", "", "Data (*.dat)");
    QFile file(name);
    if(file.open(QFile::WriteOnly)){
        QTextStream str(&file);
        for(int i = 0 ; i < res.size(); ++i){
            str << i << " " << res[i] << " " << resm[i] << '\n';
        }
    }else{
        qDebug() << "Can not open file for writing!";
    }
}

/*!
 * \brief MainWindow::on_actionStop_triggered
 */
void MainWindow::on_actionStop_triggered()
{
    emit stop();
}

void MainWindow::progress(int value)
{
    ui->progressBar->setValue(value);
    if(value == 100){
        ui->progressBar->hide();
        ui->label_8->hide();
    }
}

void MainWindow::time(double value)
{
    if(value < 60){
        ui->label_8->setText(QString::number(value,'g',2)+"s");
    }else if(value < 3600){
        ui->label_8->setText(QString::number((int)value/60)+"m"+QString::number((int)value%60)+"s");
    }
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    vp->setStart(arg1);
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    vp->setEnd(arg1);
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}
