#include "processor.hpp"
#include <fstream>


/*!
 * \brief Processor::Processor
 * \param parent
 */
VideoProcessor::VideoProcessor(QObject* parent) : 
    QObject(parent), 
    start(-1),
    end(-1), 
    stop(false),
    ad(false), 
    sensitivity(60), 
    period(1.0)
{
    setAutoDelete(false);
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
        imageProcessor->setBounds(autoDetectLight());
    }
    
    cv::VideoCapture capture(filename.toStdString().c_str());
    
    if(!capture.isOpened()){
        QMessageBox::warning(0, "Error", "Capture failed (file not found?)\n");
        return;
    }
    
    int frameNumber = int(capture.get(CV_CAP_PROP_FRAME_COUNT));
    int fps = capture.get(CV_CAP_PROP_FPS); 
    
    fixRange(fps, frameNumber);
    
    if(start >= end && start > std::numeric_limits<double>::epsilon() && end > std::numeric_limits<double>::epsilon() ){
        return;
    }
    
    int absIndex = 0;
    
    for(int i = 0; i < int(start * fps); ++i, ++absIndex){
        if(!capture.grab()){
            QMessageBox::warning(0, "Error", "Grab failed\n");
            return;
        }
    }
    
    cv::Mat frame;
    
    for(int i = int(start * fps); i < int(end * fps); i++){
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
        
        int relIndex = absIndex - int(start * fps);
        
        if (!(relIndex % (int((end - start) * fps) / 100))) {
            emit progress(relIndex / (int((end - start) * fps) / 100));
        }
        
        double timestamp = absIndex / double(fps);
        timeStamps.push_back(timestamp);
        emit time(timestamp);
        
        QThread::currentThread()->usleep(50);
    }
    capture.release();
    
    emit progress(100);
    
    
    std::shared_ptr< Results > results( new Results );
    results->resultsNumbers = lightPixelsNumbers;
    results->resultMeans = lightPixelsMeans;
    results->timeStamps = timeStamps;
    
    emit displayResults(results);
}

/*!
 * \brief Processor::autoDetectLight
 * \return 
 */
QRect VideoProcessor::autoDetectLight(){
    emit detection();
    
    cv::VideoCapture capture(filename.toStdString().c_str());
    
    if (!capture.isOpened()) {
        QMessageBox::warning(0, "Error", "Capture failed (file not found?)\n");
        return QRect(0, 0, 0, 0);;
    }
    
    cv::Mat frame, oldframe, dif;
    
    int frameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
    int fps = capture.get(CV_CAP_PROP_FPS); 
    
    fixRange(fps, frameNumber);
    
    if(period >= fabs(end - start) && (end - start) * fps >= 10){
        period = (end - start) / 2;
    }else if((end - start) * fps < 10){
        QMessageBox::warning(0, "Error", "Too small range for autodetection\n");        
        return QRect(0, 0, 0, 0);;
    }
    
    if(start >= end && start > std::numeric_limits<double>::epsilon() && end > std::numeric_limits<double>::epsilon()){
        return QRect(0, 0, 0, 0);;
    }
    
    unsigned absIndex = 0;
    
    for(auto i = 0u; i < unsigned(start * fps); ++i, ++absIndex){
        if(!capture.grab()){
            QMessageBox::warning(0, "Error", "Grab failed\n");
            return QRect(0, 0, 0, 0);;
        }
    }
    
    if(!capture.read(oldframe)){
        QMessageBox::warning(0, "Error", "Read failed\n");                
        return QRect(0, 0, 0, 0);;
    }
    
    for(auto i = 0u; i < unsigned(period*fps); ++i){
        if(!capture.grab()){
            QMessageBox::warning(0, "Error", "Grab failed\n");
            return QRect(0, 0, 0, 0);;
        }    
    }
    
    if(!capture.read(frame)){
        QMessageBox::warning(0, "Error", "Read failed\n"); 
        return QRect(0, 0, 0, 0);;
    }
    
    cv::absdiff(frame, oldframe, dif);
    cv::threshold(dif, dif, (100 - sensitivity), 255, cv::THRESH_BINARY);
    
    cv::Mat tmp = dif.clone();
    
    for(auto i = unsigned((start + period) * fps) + 2u; i < unsigned(end * fps); ++i){
        if(stop){
            break;
        }
        
        absIndex++;
        
        if(i % unsigned(period*fps) == 0){
            oldframe = frame.clone();
            
            if(!capture.read(frame)){
                QMessageBox::warning(0, "Error", "Read failed\n");                
            }
            
            cv::absdiff(frame, oldframe, dif);
            cv::threshold(dif, dif, (100 - sensitivity), 255, cv::THRESH_BINARY);
            cv::bitwise_or(dif, tmp, tmp);
            
            int relIndex = absIndex - int(start * fps);
            
            emit progress(relIndex / (fps * (end -start) / 100.0));
        }else{
            if(!capture.grab()){
                QMessageBox::warning(0, "Error", "Grab failed\n"); 
                return QRect(0, 0, 0, 0);
            }
        }
        QThread::currentThread()->usleep(50);
    }
    
    cv::flip(tmp, tmp, 0);
    cv::Mat diffMap(tmp.rows, tmp.cols, CV_8UC1);
    cv::cvtColor(tmp, diffMap, CV_RGB2GRAY);
    
    Contours contours;
    cv::findContours(diffMap, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    
    std::vector<cv::Rect> boundRect(contours.size());
    int maxx = 0, minx = diffMap.cols, maxy = 0, miny = diffMap.rows; 
    
    for(unsigned i = 0; i < contours.size(); ++i){
        boundRect[i] = boundingRect(contours[i]);
        if(boundRect[i].x < minx){
            minx = boundRect[i].x;
        }
        if(boundRect[i].y < miny){
            miny = boundRect[i].y;
        }
        if((boundRect[i].x + boundRect[i].width) > maxx){
            maxx = boundRect[i].x + boundRect[i].width;
        }
        if((boundRect[i].y + boundRect[i].height) > maxy){
            maxy = boundRect[i].y + boundRect[i].height;
        }
    }
    
    capture.release();
    
    QRect result(minx, miny, maxx - minx, maxy - miny);
    emit rectChanged(result);
    return result;
}

/*!
 * \brief Processor::fixRange
 * \param fps
 * \param frameNumber
 */
void VideoProcessor::fixRange(int fps, int frameNumber)
{
    if(start == -1){
        start = 0;
    }    
    if(end == -1){
        end = frameNumber / double(fps);
    }
    if(start * fps > frameNumber){
        start = frameNumber;
    }
    if(end * fps > frameNumber){
        end = frameNumber;
    }
    if(start < 0.0){
        start = 0.0;
    }
    if(end < 0.0){
        end = 0.0;
    }    
}

/*!
 * \brief Processor::stopThis
 */
void VideoProcessor::stopThis()
{
    stop = true;
}

/*!
 * \brief Processor::setPeriod
 * \param value
 */
void VideoProcessor::setPeriod(double value)
{
    period = value;
}

/*!
 * \brief VideoProcessor::setImageProcessor
 * \param ip
 */
void VideoProcessor::setImageProcessor(ImageProcessor *ip)
{
    imageProcessor = ip;
}

/*!
 * \brief Processor::setSensitivity
 * \param value
 */
void VideoProcessor::setSensitivity(unsigned int value)
{
    sensitivity = value;
}

/*!
 * \brief Processor::setEnd
 * \param value
 */
void VideoProcessor::setEnd(double value)
{
    end = value;
}

/*!
 * \brief Processor::setStart
 * \param value
 */
void VideoProcessor::setStart(double value)
{
    start = value;
}
