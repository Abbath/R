#include "videoprocessor.hpp"
#include <fstream>


/*!
 * \brief Processor::Processor
 * \param parent
 */
VideoProcessor::VideoProcessor(QObject* parent) : 
    QObject(parent), 
    range(-1, -1),
    stop(false),
    autodetection(false)
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
    
    if(autodetection){
        emit detection();
        QRect rect = lightDetector->detectLight(filename, qMakePair(range.first, range.second)); 
        if(stop){
            stop = false;
            return;
        }
        imageProcessor->setBounds(rect);
        emit rectChanged(rect);
    }
    
    std::ifstream f(filename.toStdString().c_str(), std::ifstream::binary | std::ifstream::in);
    f.seekg(108+20, f.beg);
    int scale;
    f.read((char*)&scale, 4);
    int rate;
    f.read((char*)&rate, 4);
    f.close();
    
    CaptureWrapper capture(filename);
    try{
        capture.isOpened();
        
        int frameNumber = int(capture.get(CV_CAP_PROP_FRAME_COUNT));
        int fps = floor(double(rate / scale)+0.5); //capture.get(CV_CAP_PROP_FPS); 
        
        lightDetector->fixRange(range, fps, frameNumber);
        
        if(range.first >= range.second 
                && range.first > std::numeric_limits<double>::epsilon() 
                && range.second > std::numeric_limits<double>::epsilon()){
            return;
        }
        
        int absIndex = 0;
        
        for(int i = 0; i < int(range.first * fps); ++i, ++absIndex){
            capture.grab();
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
                processingResult = imageProcessor->process(grayFrame);
            }else{
                QMessageBox::critical(0, "Error", "No image processor is set!\nContact with developer!");
                processingResult = qMakePair(0, 0.0l);
            }
            
            lightPixelsNumbers.push_back(processingResult.first);
            lightPixelsMeans.push_back(processingResult.second);
            
            absIndex++;
            
            int relIndex = absIndex - int(range.first * fps);
            
            const int hundreds = int((range.second - range.first) * fps) / 100;
            if (hundreds && !(relIndex % (hundreds))) {
                emit progress(relIndex / (hundreds));
            }
            
            double timestamp = absIndex / double(fps);
            timeStamps.push_back(timestamp);
            emit time(timestamp);
            
            QThread::currentThread()->usleep(50);
        }
        
        
        std::shared_ptr<Results> results(new Results);
        results->resultsNumbers = lightPixelsNumbers;
        results->resultMeans = lightPixelsMeans;
        results->timeStamps = timeStamps;

    
        emit displayResults(results);
        emit progress(100);
    }
    catch(CaptureError e){
        QMessageBox::critical(0, "Error", e.getMessage());
        return;
    }
}

/*!
 * \brief VideoProcessor::writeSettings
 */
void VideoProcessor::writeSettings()
{
    QSettings settings("CAD", "R");
    settings.beginGroup("VP");
    settings.setValue("ad", autodetection);
    settings.endGroup();
}

/*!
 * \brief VideoProcessor::readSettings
 */
void VideoProcessor::readSettings()
{
    QSettings settings("CAD", "R");
    autodetection = settings.value("VP/ad").toBool();
}

/*!
 * \brief Processor::stopThis
 */
void VideoProcessor::stopThis()
{
    stop = true;
    if(autodetection){
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
