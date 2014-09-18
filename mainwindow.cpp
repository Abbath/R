#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <qwt_scale_draw.h>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>

/*!
 * \brief MainWindow::MainWindow
 * \param parent
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isRunning(false)
{
    ui->setupUi(this);
    
    this->showMaximized();
    
    ui->groupBox->hide();
    ui->progressBar->hide();
    ui->label_8->hide();
    ui->imagearea->setDisabled(true);
    
    initPlot(ui->l_plot, lightsNumbersPlot, QString("Lights"), QString("Time [s]"), QString("Points"));
    initPlot(ui->m_plot, lightsMeansPlot, QString("Lights mean"), QString("Time [s]"), QString("Mean"));
    
    vp = new Processor;
    
    ui->imagearea->readConfig("bounds.conf");
    QRect bounds = ui->imagearea->getBounds();
    ui->spinBox_X1->setMaximum(bounds.left());
    ui->spinBox_Y1->setMaximum(bounds.top());
    ui->spinBox_X2->setMaximum(bounds.right());
    ui->spinBox_Y2->setMaximum(bounds.bottom());
    ui->spinBox_X1->setValue(bounds.left());
    ui->spinBox_Y1->setValue(bounds.top());
    ui->spinBox_X2->setValue(bounds.right());
    ui->spinBox_Y2->setValue(bounds.bottom());
    
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<double> >("QVector<double>");
    
    connect(vp, SIGNAL(graphL(QVector<int>, QVector<double>)), this, SLOT(displayResultsL(QVector<int>, QVector<double>))/*, Qt::QueuedConnection*/);
    connect(vp, SIGNAL(graphM(QVector<double>, QVector<double>)), this, SLOT(displayResultsM(QVector<double>, QVector<double>))/*, Qt::QueuedConnection*/);
    connect(vp, SIGNAL(rectChanged(QRect)), this, SLOT(setBounds(QRect)));
    connect(vp, SIGNAL(frameChanged(QImage)), ui->imagearea, SLOT(frameChanged(QImage))/*, Qt::QueuedConnection*/);
    connect(vp, SIGNAL(rectChanged(QRect)), ui->imagearea, SLOT(boundsChanged(QRect))/*, Qt::QueuedConnection*/);
    connect(this, SIGNAL(stop()), vp, SLOT(stopThis())/*, Qt::QueuedConnection*/);
    connect(vp, SIGNAL(maxMinBounds(QRect)), this, SLOT(setMaxMinBounds(QRect))/*, Qt::QueuedConnection*/);
    connect(vp, SIGNAL(progress(int)), this, SLOT(progress(int))/*, Qt::QueuedConnection*/);
    connect(vp, SIGNAL(time(double)), this, SLOT(time(double)));
    connect(vp, SIGNAL(detection()), this, SLOT(detection()));
    this->showMaximized();
}

/*!
 * \brief MainWindow::initPlot
 * \param plot
 * \param mag
 * \param zoom
 * \param curve
 * \param title
 * \param xlabel
 * \param ylabel
 */
void MainWindow::initPlot(QwtPlot* plot, QwtToolSet &toolset, QString title, QString xlabel, QString ylabel) {
    plot->setTitle(title);
    plot->setAxisTitle(plot->xBottom, xlabel);
    plot->setAxisTitle(plot->yLeft, ylabel);
    plot->setAxisAutoScale(plot->xBottom, true);
    plot->setAxisAutoScale(plot->yLeft, true);
    toolset.mag = new QwtPlotMagnifier(plot->canvas());
    toolset.zoom = new QwtPlotZoomer(plot->canvas());
    toolset.zoom->setRubberBandPen(QPen(Qt::white));
    toolset.curve.setRenderHint(QwtPlotItem::RenderAntialiased);
    toolset.curve.setPen(QPen(Qt::red));
    toolset.curve.attach(plot);
    Q_UNUSED(toolset);
}

/*!
 * \brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    vp->stopThis();
    delete ui;
}

/*!
 * \brief MainWindow::on_horizontalSlider_valueChanged
 * \param value
 */
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if (ui->action3D->isChecked()) {
        //ui->widget_3d->setStep((float)value/255.0);
    } else {
        vp->setThreshold(value);
        if (!imageFileName.isNull()) {
            QImage image(imageFileName);
            QPair<int, double> id = vp->processImageCV(image);
            ui->label_light->setNum(id.first);
            ui->label_mean->setNum(id.second);
        } else if(!videoFileName.isNull()){
            QPair<int, double> id = vp->processImageCV(ui->imagearea->getImage());
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
    if(!isRunning){
        auto newImageFileName = QFileDialog::getOpenFileName(this, tr("Open image file"), "", tr("Image files (*.bmp)"));
        if(newImageFileName.isEmpty() || newImageFileName.isNull() || newImageFileName == imageFileName){
            return;
        }
        videoFileName.clear();
        imageFileName = newImageFileName;
        QImage image(imageFileName);
        ui->spinBox_X1->setMaximum(image.width());
        ui->spinBox_Y1->setMaximum(image.height());
        ui->spinBox_X2->setMaximum(image.width());
        ui->spinBox_Y2->setMaximum(image.height());
        vp->setThreshold(ui->spinBox->value());
        vp->setBounds(ui->imagearea->getBounds());
        ui->imagearea->setEnabled(true);
        ui->imagearea->open(imageFileName);
        QPair<int, double> id = vp->processImageCV(image);
        ui->label_light->setNum(id.first);
        ui->label_mean->setNum(id.second);
    }
}

/*!
 * \brief MainWindow::on_action3D_triggered
 * \param checked
 */
void MainWindow::on_action3D_triggered(bool checked)
{
    Q_UNUSED(checked);
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
    if (ui->action3D->isChecked()) {
        //    ui->widget_3d->setStep((float)ui->spinBox->value()/255.0);
        //    ui->widget_3d->setImage(ui->imagearea->getImage().copy(ui->imagearea->getRect()));
    }
    vp->setThreshold(ui->spinBox->value());
    vp->setBounds(rect);
    
    if (!imageFileName.isNull()) {
        QImage image(imageFileName);
        QPair<int, double> id = vp->processImageCV(image);
        ui->label_light->setNum(id.first);
        ui->label_mean->setNum(id.second);
    } else if(!videoFileName.isNull()){
        QPair<int, double> id = vp->processImageCV(ui->imagearea->getImage());
        ui->label_light->setNum(id.first);
        ui->label_mean->setNum(id.second);
    }
}

/*!
 * \brief MainWindow::setMaxMinBounds
 * \param rect
 */
void MainWindow::setMaxMinBounds(QRect rect)
{
    if (!imageFileName.isNull()) {
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
    setBounds(ui->imagearea->getBounds());
}

/*!
 * \brief MainWindow::on_spinBox_3_valueChanged
 * \param arg1
 */
void MainWindow::on_spinBox_Y1_valueChanged(int arg1)
{
    ui->imagearea->setY1(arg1);
    setBounds(ui->imagearea->getBounds());    
}

/*!
 * \brief MainWindow::on_spinBox_X2_valueChanged
 * \param arg1
 */
void MainWindow::on_spinBox_X2_valueChanged(int arg1)
{
    ui->imagearea->setX2(arg1);
    setBounds(ui->imagearea->getBounds());
    
}

/*!
 * \brief MainWindow::on_spinBox_5_valueChanged
 * \param arg1
 */
void MainWindow::on_spinBox_Y2_valueChanged(int arg1)
{
    ui->imagearea->setY2(arg1);
    setBounds(ui->imagearea->getBounds());
}

/*!
 * \brief MainWindow::on_actionSetup_triggered
 * \param checked
 */
void MainWindow::on_actionSetup_triggered(bool checked)
{
    if (checked) {
        ui->groupBox->show();
    } else {
        ui->groupBox->hide();
    }
}

/*!
 * \brief MainWindow::on_actionOpen_Video_triggered
 */
void MainWindow::on_actionOpen_Video_triggered()
{
    if(!isRunning){
        auto newVideoFileName = QFileDialog::getOpenFileName(this, tr("Open data file"), "", tr("Video files (*.avi)"));
        if (newVideoFileName.isEmpty() || newVideoFileName.isNull() || newVideoFileName == videoFileName){
            return;
        }
        imageFileName.clear();
        videoFileName = newVideoFileName;
        cv::VideoCapture capture(videoFileName.toStdString().c_str());
        if (!capture.isOpened()) {
            QMessageBox::warning(0, "Error", "Capture From AVI failed (file not found?)\n");
        } else {
            auto frameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
            auto fps = capture.get(CV_CAP_PROP_FPS);
            auto videoLength = frameNumber / double(fps);
            ui->doubleSpinBox_2->setMaximum(videoLength);
            ui->doubleSpinBox_3->setMaximum(videoLength);
            vp->setFilename(videoFileName);
            cv::Mat frame;
            capture.read(frame);
            QImage image = vp->Mat2QImage(frame);
            ui->spinBox_X1->setMaximum(image.width());
            ui->spinBox_Y1->setMaximum(image.height());
            ui->spinBox_X2->setMaximum(image.width());
            ui->spinBox_Y2->setMaximum(image.height());
            ui->imagearea->setEnabled(true);
            ui->imagearea->loadImage(image.mirrored(false, true));
        }
    }
}

/*!
 * \brief MainWindow::displayResultsL
 * \param res
 */
void MainWindow::displayResultsL(const QVector<int>& res, const QVector<double>& t)
{
    this->lightPixelsNumbers = res;
    ui->l_plot->detachItems(QwtPlotItem::Rtti_PlotCurve, false);
    ui->l_plot->replot();
    
    QVector<QPointF> points(res.size());
    quint32 counter = 0;
    auto pointsIt = points.begin();
    
    for (auto ri = res.constBegin(); ri != res.constEnd(); ++ri, ++pointsIt, ++counter) {
        (*pointsIt) = QPointF(t[counter], (*ri));
    }
    
    QwtPointSeriesData* data = new QwtPointSeriesData(points);
    lightsNumbersPlot.curve.setData(data);
    lightsNumbersPlot.curve.attach(ui->l_plot);
    ui->l_plot->replot();
}

/*!
 * \brief MainWindow::displayResultsM
 * \param res
 */
void MainWindow::displayResultsM(const QVector<double>& res, const QVector<double>& t)
{
    this->lightPixelsMeans = res;
    ui->m_plot->detachItems(QwtPlotItem::Rtti_PlotCurve, false);
    ui->m_plot->replot();
    
    QVector<QPointF> points(res.size());
    quint32 counter = 0;
    auto pointsIt = points.begin();
    
    for (auto ri = res.constBegin(); ri != res.constEnd(); ++ri, ++pointsIt, ++counter) {
        (*pointsIt) = QPointF(t[counter], (*ri));
    }
    
    QwtPointSeriesData* data = new QwtPointSeriesData(points);
    lightsMeansPlot.curve.setData(data);
    lightsMeansPlot.curve.attach(ui->m_plot);
    ui->m_plot->replot();
}

/*!
 * \brief MainWindow::on_actionRun_triggered
 */
void MainWindow::on_actionRun_triggered()
{
    if(!isRunning && !videoFileName.isEmpty()){
        isRunning = true;    
        ui->progressBar->show();
        ui->label_8->show();
        if (!videoFileName.isNull()) {
            vp->setThreshold(ui->spinBox->value());
            vp->setBounds(ui->imagearea->getBounds());
            QThreadPool::globalInstance()->start(vp);
        }
    }
    
}

/*!
 * \brief MainWindow::on_actionSave_triggered
 */
void MainWindow::on_actionSave_triggered()
{
    QString name = QFileDialog::getSaveFileName(this, "Save data", "", "Text (*.txt)");
    QFile file(name);
    if (file.open(QFile::WriteOnly)) {
        QTextStream str(&file);
        for (int i = 0; i < lightPixelsNumbers.size(); ++i) {
            str << i << " " << lightPixelsNumbers[i] << " " << lightPixelsMeans[i] << '\n';
        }
    } else {
        QMessageBox::warning(this, "Warning!", "Can not open file for writing!");
    }
}

/*!
 * \brief MainWindow::on_actionStop_triggered
 */
void MainWindow::on_actionStop_triggered()
{
    isRunning = false;
    emit stop();
}

/*!
 * \brief MainWindow::progress
 * \param value
 */
void MainWindow::progress(int value)
{
    ui->progressBar->setValue(value);
    if (value == 100) {
        isRunning = false;
        ui->progressBar->hide();
        ui->label_8->hide();
    }
}

/*!
 * \brief MainWindow::time
 * \param value
 */
void MainWindow::time(double value)
{
    QString s = int(value) % 60 < 10 ? "0" + QString::number(int(value) % 60) : QString::number(int(value) % 60);
    QString m = int(value) % 3600 / 60 < 10 ? "0" + QString::number(int(value) % 3600 / 60) : QString::number(int(value) % 3600 / 60);
    QString h = QString::number(int(value) / 3600);
    ui->label_8->setText(h + ":" + m + ":" + s);
}

/*!
 * \brief MainWindow::on_doubleSpinBox_2_valueChanged
 * \param arg1
 */
void MainWindow::on_doubleSpinBox_2_valueChanged(double arg1)
{
    vp->setStart(arg1);
}

/*!
 * \brief MainWindow::on_doubleSpinBox_3_valueChanged
 * \param arg1
 */
void MainWindow::on_doubleSpinBox_3_valueChanged(double arg1)
{
    vp->setEnd(arg1);
}

/*!
 * \brief MainWindow::on_actionQuit_triggered
 */
void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

/*!
 * \brief MainWindow::detection
 */
void MainWindow::detection()
{
    ui->label_8->setText("Detection...");
}

void MainWindow::on_actionAutodetection_triggered(bool checked)
{
    vp->setAd(checked);
}

void MainWindow::on_actionAbout_triggered()
{
    QString cv;
#if defined(__GNUC__) || defined(__GNUG__)
    cv = "GCC " + QString::number(__GNUC__) + "."+QString::number(__GNUC_MINOR__) + "."+QString::number(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    cv = "MSVC " + QString::number(_MSC_FULL_VER);
#endif
    QMessageBox::about(this,"R", "Lab-on-a-chip light analyser. © 2013-2014\nQt version: " + QString(QT_VERSION_STR) + "\nCompiler Version: " + cv);
}
