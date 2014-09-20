#include "processor.hpp"
#include <fstream>


/*!
 * \brief Processor::Processor
 * \param parent
 */
VideoProcessor::VideoProcessor(QObject* parent) : 
    QObject(parent), 
    range(-1, -1),
    stop(false),
    ad(false)
{
    lightDetector = new LightDetector(this);
    readSettings();
    setAutoDelete(false);
    connect(lightDetector, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
}

/*!
 * \brief Processor::run
 */
void VideoProcessor::run()
{
    QVector<int> lightPixelsNumbers;
    QVector<double> lightPixelsMeans;
    QVector<double> timeStamps;
    
    if(ad){
        emit detection();
        QRect rect = lightDetector->detectLight(filename, qMakePair(range.first, range.second)); 
        if(stop){
            stop = false;
            return;
        }
        imageProcessor->setBounds(rect);
        emit rectChanged(rect);
    }
    
    cv::VideoCapture capture(filename.toStdString().c_str());
    
    if(!capture.isOpened()){
        QMessageBox::warning(0, "Error", "Capture failed (file not found?)\n");
        return;
    }
    
    int frameNumber = int(capture.get(CV_CAP_PROP_FRAME_COUNT));
    int fps = capture.get(CV_CAP_PROP_FPS); 
    
    lightDetector->fixRange(range, fps, frameNumber);
    
    if(range.first >= range.second && range.first > std::numeric_limits<double>::epsilon() && range.second > std::numeric_limits<double>::epsilon() ){
        return;
    }
    
    int absIndex = 0;
    
    for(int i = 0; i < int(range.first * fps); ++i, ++absIndex){
        if(!capture.grab()){
            QMessageBox::warning(0, "Error", "Grab failed\n");
            return;
        }
    }
    
    cv::Mat frame;
    
    for(int i = int(range.first * fps); i < int(range.second * fps); i++){
        if (stop) {
            stop = false;
            break;
        }
        
        capture >> frame;
        
        cv::Mat grayFrame;
        cv::cvtColor(frame, grayFrame, CV_BGR2GRAY, 1);
        cv::flip(grayFrame, grayFrame, 0);
        
        QPair<int, double> processingResult;
        
        if(imageProcessor){
            processingResult = imageProcessor->processImage(grayFrame);
        }else{
            QMessageBox::warning(0, "Error", "No image processor is set!\nConnect with developer!");
            processingResult = qMakePair(0, 0.0l);
        }
        
        lightPixelsNumbers.push_back(processingResult.first);
        lightPixelsMeans.push_back(processingResult.second);
        
        absIndex++;
        
        int relIndex = absIndex - int(range.first * fps);
        
        if (!(relIndex % (int((range.second - range.first) * fps) / 100))) {
            emit progress(relIndex / (int((range.second - range.first) * fps) / 100));
        }
        
        double timestamp = absIndex / double(fps);
        timeStamps.push_back(timestamp);
        emit time(timestamp);
        
        QThread::currentThread()->usleep(50);
    }
    capture.release();
    
    emit progress(100);
    
    std::shared_ptr<Results> results(new Results);
    results->resultsNumbers = lightPixelsNumbers;
    results->resultMeans = lightPixelsMeans;
    results->timeStamps = timeStamps;
    
    emit displayResults(results);
}

/*!
 * \brief VideoProcessor::writeSettings
 */
void VideoProcessor::writeSettings()
{
    QSettings settings("CAD", "R");
    settings.beginGroup("VP");
    settings.setValue("ad", ad);
    settings.endGroup();
}

/*!
 * \brief VideoProcessor::readSettings
 */
void VideoProcessor::readSettings()
{
    QSettings settings("CAD", "R");
    ad = settings.value("VP/ad").toBool();
}

/*!
 * \brief Processor::stopThis
 */
void VideoProcessor::stopThis()
{
    stop = true;
    if(ad){
        lightDetector->stopThis();
    }
}

/*!
 * \brief Processor::setPeriod
 * \param value
 */
void VideoProcessor::setPeriod(double value)
{
    lightDetector->setPeriod(value);
}

/*!
 * \brief VideoProcessor::setImageProcessor
 * \param ip
 */
void VideoProcessor::setImageProcessor(ImageProcessor *ip)
{
    imageProcessor = ip;
}

VideoProcessor::~VideoProcessor()
{
    writeSettings();
}

/*!
 * \brief Processor::setSensitivity
 * \param value
 */
void VideoProcessor::setSensitivity(unsigned int value)
{
    lightDetector->setSensitivity(value);
}

/*!
 * \brief Processor::setEnd
 * \param value
 */
void VideoProcessor::setEnd(double value)
{
    range.second = value;
}

/*!
 * \brief Processor::setStart
 * \param value
 */
void VideoProcessor::setStart(double value)
{
    range.first = value;
}
