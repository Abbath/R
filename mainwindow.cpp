#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "capturedialog.hpp"
#include <qwt_scale_draw.h>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

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
    
    ui->progressBar->hide();
    ui->label_8->hide();
    ui->imagearea->setDisabled(true);
    
    initPlot(ui->l_plot, lightsNumbersPlot, QString("Lights"), QString("Time [s]"), QString("Points"));
    initPlot(ui->m_plot, lightsMeansPlot, QString("Lights mean"), QString("Time [s]"), QString("Mean"));
    
    videoProcessor = new VideoProcessor(this);
    imageProcessor = new ImageProcessor(this);
    streamProcessor = new StreamProcessor(this);
    videoProcessor->setImageProcessor(imageProcessor);
    streamProcessor->setImageProcessor(imageProcessor);
    imageProcessor->setH_size(ui->label_5->size());
    
    readSettings();
    
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<double> >("QVector<double>");
    qRegisterMetaType<std::shared_ptr<Results>>("std::shared_ptr<Results>");
    qRegisterMetaType<Contours>("Contours");
    
    connect(videoProcessor, SIGNAL(displayResults(std::shared_ptr<Results>)), this, SLOT(plotResults(std::shared_ptr<Results>)));
    connect(videoProcessor, SIGNAL(rectChanged(QRect)), this, SLOT(setBounds(QRect)));
    connect(imageProcessor, SIGNAL(frameChanged(QImage, Contours)), ui->imagearea, SLOT(frameChanged(QImage, Contours)));
    connect(videoProcessor, SIGNAL(rectChanged(QRect)), ui->imagearea, SLOT(boundsChanged(QRect)));
    connect(this, SIGNAL(stop()), videoProcessor, SLOT(stopThis()));
    connect(videoProcessor, SIGNAL(progress(int)), this, SLOT(progress(int)));
    connect(videoProcessor, SIGNAL(time(double)), this, SLOT(time(double)));
    connect(videoProcessor, SIGNAL(detection()), this, SLOT(detection()));
    connect(ui->sens, SIGNAL(valueChanged(int)), this, SLOT(sensChanged(int)));
    connect(ui->period, SIGNAL(valueChanged(double)), this, SLOT(periodChanged(double)));
    connect(imageProcessor, SIGNAL(histogram(QImage)), this, SLOT(histogram(QImage)));
    connect(streamProcessor, SIGNAL(time(double)), this, SLOT(time(double)));
    connect(streamProcessor, SIGNAL(displayResults(std::shared_ptr<Results>)), this, SLOT(plotResults(std::shared_ptr<Results>)));
    connect(this, SIGNAL(stop()), streamProcessor, SLOT(stopThis()));
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
}

/*!
 * \brief MainWindow::writeSettings
 */
void MainWindow::writeSettings()
{
    QSettings settings("CAD", "R");
    settings.beginGroup("MW");
    settings.setValue("threshold", ui->spinBox->value());
    settings.setValue("ad", ui->actionAutodetection->isChecked());
    settings.setValue("sens", ui->sens->value());
    settings.setValue("period", ui->period->value());
    settings.setValue("bounds", ui->imagearea->getBounds());
    settings.setValue("X1", ui->spinBox_X1->value());
    settings.setValue("X2", ui->spinBox_X2->value());
    settings.setValue("Y1", ui->spinBox_Y1->value());
    settings.setValue("Y2", ui->spinBox_Y2->value());
    settings.endGroup();
}

/*!
 * \brief MainWindow::readSettings
 */
void MainWindow::readSettings()
{
    QSettings settings("CAD", "R");
    ui->spinBox->setValue(settings.value("MW/threshold").toInt());
    ui->actionAutodetection->setChecked(settings.value("MW/ad").toBool());
    ui->sens->setValue(settings.value("MW/sens").toUInt());
    ui->period->setValue(settings.value("MW/period").toDouble());
    setBounds(settings.value("MW/bounds").toRect());
    ui->imagearea->setBounds(settings.value("MW/bounds").toRect());
    ui->spinBox_X1->setMaximum(settings.value("MW/X1").toInt());
    ui->spinBox_X1->setValue(settings.value("MW/X1").toInt());
    ui->spinBox_X2->setMaximum(settings.value("MW/X2").toInt());
    ui->spinBox_X2->setValue(settings.value("MW/X2").toInt());
    ui->spinBox_Y1->setMaximum(settings.value("MW/Y1").toInt());
    ui->spinBox_Y1->setValue(settings.value("MW/Y1").toInt());
    ui->spinBox_Y2->setMaximum(settings.value("MW/Y2").toInt());    
    ui->spinBox_Y2->setValue(settings.value("MW/Y2").toInt());
}

/*!
 * \brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    videoProcessor->stopThis();
    streamProcessor->stopThis();
    QThread::currentThread()->msleep(50);
    writeSettings();
    delete ui;
}

/*!
 * \brief MainWindow::on_horizontalSlider_valueChanged
 * \param value
 */
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    if(!isRunning){
        if (ui->action3D->isChecked()) {
            //ui->widget_3d->setStep((float)value/255.0);
        } else {
            imageProcessor->setLightThreshold(value);
            if(!imageFileName.isNull() || !videoFileName.isNull()){
                imageProcessor->setH_size(ui->label_5->size());
                QPair<int, double> id = imageProcessor->process(ImageStorage::getInstance().getImage());
                ui->label_light->setNum(id.first);
                ui->label_mean->setNum(id.second);
            }
        }
    }
}

/*!
 * \brief MainWindow::on_actionOpen_triggered
 */
void MainWindow::openImage()
{
    ImageStorage::getInstance().loadImage(imageFileName);
    if(ImageStorage::getInstance().getImage().isNull()){
        QMessageBox::critical(0, "Error", "Image open failed (file not found?)\n");            
    }else{
        ui->spinBox_X1->setMaximum(ImageStorage::getInstance().getImageWidth());
        ui->spinBox_Y1->setMaximum(ImageStorage::getInstance().getImageHeight());
        ui->spinBox_X2->setMaximum(ImageStorage::getInstance().getImageWidth());
        ui->spinBox_Y2->setMaximum(ImageStorage::getInstance().getImageHeight());
        imageProcessor->setLightThreshold(ui->spinBox->value());
        imageProcessor->setBounds(ui->imagearea->getBounds());
        ui->imagearea->setEnabled(true);
        ui->imagearea->clearContours();
        imageProcessor->setH_size(ui->label_5->size());        
        QPair<int, double> id = imageProcessor->process(ImageStorage::getInstance().getImage());
        ui->label_light->setNum(id.first);
        ui->label_mean->setNum(id.second);
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
        openImage();
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
 * \brief MainWindow::setBounds
 * \param rect
 */
void MainWindow::setBounds(QRect rect)
{
    if(!isRunning || videoFileName.isEmpty()){
        ui->spinBox_X1->setValue(rect.left());
        ui->spinBox_Y1->setValue(rect.top());
        ui->spinBox_X2->setValue(rect.right());
        ui->spinBox_Y2->setValue(rect.bottom());
        if (ui->action3D->isChecked()) {
            //    ui->widget_3d->setStep((float)ui->spinBox->value()/255.0);
            //    ui->widget_3d->setImage(ui->imagearea->getImage().copy(ui->imagearea->getRect()));
        }
        imageProcessor->setLightThreshold(ui->spinBox->value());
        imageProcessor->setBounds(rect);
        if(!ImageStorage::getInstance().isImageNull()){
            imageProcessor->setH_size(ui->label_5->size());            
            QPair<int, double> id = imageProcessor->process(ImageStorage::getInstance().getImage());
            ui->label_light->setNum(id.first);
            ui->label_mean->setNum(id.second);
        }
    }
}

void MainWindow::histogram(QImage hist)
{
    ui->label_5->setPixmap(QPixmap::fromImage(hist));
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
void MainWindow::openVideo()
{
    CaptureWrapper capture(videoFileName);
    try{
        capture.isOpened();
        auto frameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
        fps = Utils::Video::getAVIFPS(videoFileName.toStdString());
        auto videoLength = frameNumber / double(fps);
        ui->doubleSpinBox_2->setMaximum(videoLength);
        ui->doubleSpinBox_3->setMaximum(videoLength);
        ui->period->setMaximum(videoLength);
        videoProcessor->setFilename(videoFileName);
        cv::Mat frame;
        capture.read(frame);
        QImage image = Utils::Image::Mat2QImage(frame);
        ui->spinBox_X1->setMaximum(image.width());
        ui->spinBox_Y1->setMaximum(image.height());
        ui->spinBox_X2->setMaximum(image.width());
        ui->spinBox_Y2->setMaximum(image.height());
        ui->imagearea->setEnabled(true);
        ui->imagearea->clearContours();
        ImageStorage::getInstance().setImage(image.mirrored(false, true));
        ui->imagearea->update();
    }catch(CaptureError e){
        QMessageBox::critical(this, "Error", e.getMessage());
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
        openVideo();
    }
}

/*!
 * \brief MainWindow::plotResults
 * \param r
 */
void MainWindow::plotResults(std::shared_ptr<Results> r)
{
    if((r->resultMeans.isEmpty()) || 
            (r->resultsNumbers.isEmpty()) ||
            (r->timeStamps.isEmpty()) ||
            !(r->resultMeans.size() == r->resultsNumbers.size() && r->resultMeans.size() == r->timeStamps.size())) return;
    
    this->lightPixelsNumbers = r->resultsNumbers;
    this->lightPixelsMeans = r->resultMeans;
    
    ui->l_plot->detachItems(QwtPlotItem::Rtti_PlotCurve, false);
    ui->m_plot->detachItems(QwtPlotItem::Rtti_PlotCurve, false);
    ui->l_plot->replot();
    ui->m_plot->replot();
    
    QVector<QPointF> pointsNumbers(r->resultsNumbers.size());
    quint32 counter = 0;
    auto pointsNIt = pointsNumbers.begin();
    
    for (auto ri = r->resultsNumbers.constBegin(); ri != r->resultsNumbers.constEnd(); ++ri, ++pointsNIt, ++counter) {
        (*pointsNIt) = QPointF(r->timeStamps[counter], (*ri));
    }
    
    QwtPointSeriesData* numbers = new QwtPointSeriesData(pointsNumbers);
    lightsNumbersPlot.curve.setData(numbers);
    lightsNumbersPlot.curve.attach(ui->l_plot);
    ui->l_plot->replot();
    
    QVector<QPointF> pointsMeans(r->resultMeans.size());
    counter = 0;
    auto pointsMIt = pointsMeans.begin();
    
    for (auto ri = r->resultMeans.constBegin(); ri != r->resultMeans.constEnd(); ++ri, ++pointsMIt, ++counter) {
        (*pointsMIt) = QPointF(r->timeStamps[counter], (*ri));
    }
    
    QwtPointSeriesData* means = new QwtPointSeriesData(pointsMeans);
    lightsMeansPlot.curve.setData(means);
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
            imageProcessor->setLightThreshold(ui->spinBox->value());
            imageProcessor->setBounds(ui->imagearea->getBounds());
            QThreadPool::globalInstance()->start(videoProcessor);
        }
    }
    
}

/*!
 * \brief MainWindow::on_actionSave_triggered
 */
void MainWindow::on_actionSave_triggered()
{
    QString name = QFileDialog::getSaveFileName(this, "Save data", "", "Text (*.txt)");
    if(name.isEmpty() || name.isNull()){
        return;
    }
    if(!name.endsWith(".txt")){
        name += ".txt";
    }
    QFile file(name);
    if (file.open(QFile::WriteOnly)) {
        QTextStream str(&file);
        for (int i = 0; i < lightPixelsNumbers.size(); ++i) {
            str << i << " " << i/double(fps) << " " << lightPixelsNumbers[i] << " " << lightPixelsMeans[i] << '\n';
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
    ui->progressBar->hide();
    ui->label_8->hide();
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
QString MainWindow::seconds2HMS(double value)
{
    int oddSeconds =  int(value) % 60;
    QString s =  oddSeconds < 10 ? "0" + QString::number(oddSeconds) : QString::number(oddSeconds);
    int oddMinutes = int(value) % 3600 / 60;
    QString m = oddMinutes < 10 ? "0" + QString::number(oddMinutes) : QString::number(oddMinutes);
    QString h = QString::number(int(value) / 3600);
    QString t = h + ":" + m + ":" + s;
    
    return t;
}

void MainWindow::time(double value)
{
    QString t = seconds2HMS(value);
    ui->label_8->setText(t);
}

/*!
 * \brief MainWindow::on_doubleSpinBox_2_valueChanged
 * \param arg1
 */
void MainWindow::on_doubleSpinBox_2_valueChanged(double arg1)
{
    videoProcessor->setStart(arg1);
}

/*!
 * \brief MainWindow::on_doubleSpinBox_3_valueChanged
 * \param arg1
 */
void MainWindow::on_doubleSpinBox_3_valueChanged(double arg1)
{
    videoProcessor->setEnd(arg1);
    streamProcessor->setEnd(arg1);
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
    isRunning = true;
}

/*!
 * \brief MainWindow::on_actionAutodetection_triggered
 * \param checked
 */
void MainWindow::on_actionAutodetection_triggered(bool checked)
{
    if(checked){
        
    }
    videoProcessor->setAd(checked);
}

/*!
 * \brief MainWindow::on_actionAbout_triggered
 */
void MainWindow::on_actionAbout_triggered()
{
    QString cv;
#if defined(__GNUC__) || defined(__GNUG__)
    cv = "GCC " + QString::number(__GNUC__) + "." + QString::number(__GNUC_MINOR__) + "."+QString::number(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    cv = "MSVC " + QString::number(_MSC_FULL_VER);
#endif
    QMessageBox::about(this,"About", "Lab-on-a-chip light analyser. © 2013-2015\nVersion 2.4.4\nQt version: " + QString(QT_VERSION_STR) + "\nCompiler Version: " + cv);
}

/*!
 * \brief MainWindow::sensChanged
 * \param value
 */
void MainWindow::sensChanged(int value)
{
    videoProcessor->setSensitivity(value);
}

/*!
 * \brief MainWindow::periodChanged
 * \param value
 */
void MainWindow::periodChanged(double value)
{
    videoProcessor->setPeriod(value);
}

void MainWindow::on_actionCapture_Device_triggered()
{
    if(!isRunning){
        videoFileName.clear();
        CaptureDialog dialog;
        dialog.setItems(countCameras());
        if(dialog.exec() == QDialog::Accepted){
            int devnum = dialog.getDeviceNumber();            
            try{
                CaptureWrapper capture(devnum);
                cv::Mat frame;
                capture.read(frame);
                cv::Mat grayFrame;
                cv::cvtColor(frame, grayFrame, CV_BGR2GRAY, 1);
                QImage image = Utils::Image::Mat2QImage(grayFrame);
                ui->spinBox_X1->setMaximum(image.width());
                ui->spinBox_Y1->setMaximum(image.height());
                ui->spinBox_X2->setMaximum(image.width());
                ui->spinBox_Y2->setMaximum(image.height());
                ImageStorage::getInstance().setImage(image.mirrored(true, false));            
            }catch(CaptureError e){
                QMessageBox::critical(this, "Error", e.getMessage());
            }
            streamProcessor->setDeviceNumber(devnum);
            imageProcessor->setLightThreshold(ui->spinBox->value());
            imageProcessor->setBounds(ui->imagearea->getBounds());
            ui->imagearea->setEnabled(true);
            ui->imagearea->clearContours();
            //ui->imagearea->update();
            if(dialog.isRecord()){
                streamProcessor->setRecord(true);  
                streamProcessor->setFilename(dialog.getFileName());
                streamProcessor->setFPS(dialog.getFps());
            }else{
                streamProcessor->setRecord(false);
            }
            isRunning = true;
            QThreadPool::globalInstance()->start(streamProcessor);
        }   
    }
}

QStringList MainWindow::countCameras()
{
    QStringList list;
    int maxTested = 10;
    for (int i = 0; i < maxTested; i++){
        cv::VideoCapture temp_camera(i);
        bool res = temp_camera.isOpened();
        if (res)
        {
            list << QString::number(i);
        }
    }
    return list;
}

void MainWindow::on_actionSnapshot_triggered()
{
    if(isRunning){
        QImage im = ImageStorage::getInstance().getImage();
        QString name = QFileDialog::getSaveFileName(this, "Save snapshot", ".", "Image files (*.bmp)");
        if(!name.endsWith(".bmp")){
            name += ".bmp";
        }
        im.save(name);
    }
}

void MainWindow::on_groupBox_4_toggled(bool arg1)
{
    videoProcessor->setAd(arg1);
}
