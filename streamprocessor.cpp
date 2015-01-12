#include "streamprocessor.hpp"

/*!
 * \brief Processor::Processor
 * \param parent
 */
StreamProcessor::StreamProcessor(QObject* parent) : 
    QObject(parent), 
    end(-1),
    stop(false)
{
    readSettings();
    setAutoDelete(false);
}

/*!
 * \brief Processor::run
 */
void StreamProcessor::run()
{
    QVector<int> lightPixelsNumbers;
    QVector<double> lightPixelsMeans;
    QVector<double> timeStamps;
   
    CaptureWrapper capture(deviceNumber);
    try{
        capture.isOpened();
        
        int fps = 25;//capture.get(CV_CAP_PROP_FPS); 
                
        cv::Mat frame;
        
        for(int i = 0; end == -1 || i < int(end * fps); i++){
            if (stop) {
                stop = false;
                break;
            }
            
            capture >> frame;
            
            cv::Mat grayFrame;
            cv::cvtColor(frame, grayFrame, CV_BGR2GRAY, 1);
            cv::flip(grayFrame, grayFrame, 1);
            
            QPair<int, double> processingResult;
            
            if(imageProcessor){
                imageProcessor->setH_size(QSize(640, 480));
                processingResult = imageProcessor->process(grayFrame);
            }else{
                QMessageBox::critical(0, "Error", "No image processor is set!\nContact with developer!");
                processingResult = qMakePair(0, 0.0l);
            }
            
            lightPixelsNumbers.push_back(processingResult.first);
            lightPixelsMeans.push_back(processingResult.second);
                                    
            double timestamp = i / double(fps);
            timeStamps.push_back(timestamp);
            emit time(timestamp);
            
            QThread::currentThread()->usleep(50);
        }
        
        
        std::shared_ptr<Results> results(new Results);
        results->resultsNumbers = lightPixelsNumbers;
        results->resultMeans = lightPixelsMeans;
        results->timeStamps = timeStamps;

        emit displayResults(results);
    }
    catch(CaptureError e){
        QMessageBox::critical(0, "Error", e.getMessage());
        return;
    }
}

/*!
 * \brief VideoProcessor::writeSettings
 */
void StreamProcessor::writeSettings()
{
    QSettings settings("CAD", "R");
    settings.beginGroup("VP");
    settings.endGroup();
}

/*!
 * \brief VideoProcessor::readSettings
 */
void StreamProcessor::readSettings()
{
    QSettings settings("CAD", "R");
}

/*!
 * \brief Processor::stopThis
 */
void StreamProcessor::stopThis()
{
    stop = true;
}

/*!
 * \brief VideoProcessor::setImageProcessor
 * \param ip
 */
void StreamProcessor::setImageProcessor(ImageProcessor *ip)
{
    imageProcessor = ip;
}

StreamProcessor::~StreamProcessor()
{
    writeSettings();
}

/*!
 * \brief Processor::setEnd
 * \param value
 */
void StreamProcessor::setEnd(double value)
{
    if(value < std::numeric_limits<double>::epsilon())
        end = -1;
    else
        end = value;
}
