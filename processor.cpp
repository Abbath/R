#include "processor.hpp"
#include <fstream>


/*!
 * \brief Processor::Processor
 * \param parent
 */
Processor::Processor(QObject* parent) : 
    QObject(parent), 
    lightThreshold(255), 
    start(-1), end(-1), 
    stop(false), ad(false), 
    sensitivity(60), 
    period(1.0)
{
    setAutoDelete(false);
}

/*!
 * \brief Processor::run
 */
void Processor::run()
{
    QVector<int> lightPixelsNumbers;
    QVector<double> lightPixelsMeans;
    QVector<double> timestamps;
    
    if(ad){
        rect = autoDetectLight();
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
        
        QPair<int, double> processingResult = processImageCVMat(grayFrame);
        
        lightPixelsNumbers.push_back(processingResult.first);
        lightPixelsMeans.push_back(processingResult.second);
        
        absIndex++;
        
        int relIndex = absIndex - int(start * fps);
        
        if (!(relIndex % (int((end - start) * fps) / 100))) {
            emit progress(relIndex / (int((end - start) * fps) / 100));
        }
        
        double timestamp = absIndex / double(fps);
        timestamps.push_back(timestamp);
        emit time(timestamp);
        
        QThread::currentThread()->usleep(50);
    }
    capture.release();
    
    emit progress(100);
    
    emit graphL(lightPixelsNumbers, timestamps);
    emit graphM(lightPixelsMeans, timestamps);
}

/*!
 * \brief Processor::autoDetect
 * \return 
 */
QRect Processor::autoDetectLight(){
    emit detection();
    
    cv::VideoCapture capture(filename.toStdString().c_str());
    
    if (!capture.isOpened()) {
        QMessageBox::warning(0, "Error", "Capture failed (file not found?)\n");
        return rect;
    }
    
    cv::Mat frame, oldframe, dif;
    
    int frameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
    int fps = capture.get(CV_CAP_PROP_FPS); 
    
    fixRange(fps, frameNumber);
    
    if(period >= fabs(end - start) && (end - start) * fps >= 10){
        period = (end - start) / 2;
    }else if((end - start) * fps < 10){
        QMessageBox::warning(0, "Error", "Too small range for autodetection\n");        
        return rect;
    }
    
    if(start >= end && start > std::numeric_limits<double>::epsilon() && end > std::numeric_limits<double>::epsilon()){
        return rect;
    }
    
    unsigned absIndex = 0;
    
    for(auto i = 0u; i < unsigned(start * fps); ++i, ++absIndex){
        if(!capture.grab()){
            QMessageBox::warning(0, "Error", "Grab failed\n");
            return rect;
        }
    }
    
    if(!capture.read(oldframe)){
        QMessageBox::warning(0, "Error", "Read failed\n");                
        return rect;
    }
    
    for(auto i = 0u; i < unsigned(period*fps); ++i){
        if(!capture.grab()){
            QMessageBox::warning(0, "Error", "Grab failed\n");
            return rect;
        }    
    }
    
    if(!capture.read(frame)){
        QMessageBox::warning(0, "Error", "Read failed\n"); 
        return rect;
    }
        
    cv::absdiff(frame, oldframe, dif);
    cv::threshold(dif, dif, (100 - sensitivity), 255, cv::THRESH_BINARY);
    
    cv::Mat tmp = dif.clone();
    
    for(auto i = unsigned((start + period) * fps) + 2u; i < unsigned(end * fps); ++i){
        if (stop) {
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
                return rect;
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
 * \brief Processor::processImageCV
 * \param _image
 * \return 
 */
QPair<int, double> Processor::processImageCV(QImage _image)
{
    if(_image.isNull()){
        return qMakePair(0, 0);
    }
    cv::Mat m = QImage2Mat(_image);
    return processImageCVMat(m);
}

/*!
 * \brief Processor::processImageCVMat
 * \param m
 * \return 
 */
QPair<int, double> Processor::processImageCVMat(cv::Mat& m){
    cv::Mat matCopy;
    m.copyTo(matCopy);
    
    cv::threshold(m, m, lightThreshold - 1, 255, cv::THRESH_BINARY);
    
    rect = rect.normalized();
    cv::Rect rec(rect.left(), rect.top(), rect.width(), rect.height());
    m = m(rec);
    
    Contours contours;
    cv::findContours(m, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    
    double lightArea = 0, lightMean = 0;
        
    for (auto& c : contours){
        auto localLightArea = cv::contourArea(c);
        lightArea += localLightArea;
        
        for(auto& p: c){
            p.x += rec.x;
            p.y += rec.y;
        }
        
        lightMean += localLightArea * mean(matCopy, c);
    }
    
    lightMean /= lightArea;
    
    if(lightMean < lightThreshold){
        lightMean = lightThreshold;
    }
    
    emit frameChanged(drawOnQImage(Mat2QImage(matCopy), contours));
    
    QPair<int, double> result(lightArea, lightMean);
    
    return result;
}

/*!
 * \brief Processor::drawOnQImage
 * \param image
 * \param contours
 * \return 
 */
QImage Processor::drawOnQImage(QImage image, Contours contours)
{
    QPainter p;
    p.begin(&image);
    p.setPen(QPen(QColor(0, 255, 0)));
    
    for(auto contourIt = contours.begin(); contourIt != contours.end(); ++contourIt){
        for(auto pointIt = contourIt->begin(); pointIt != (contourIt->end() - 1); ++pointIt){
            p.drawLine(pointIt->x, pointIt->y, (pointIt + 1)->x, (pointIt + 1)->y);
        }
        p.drawLine((contourIt->end() - 1)->x, (contourIt->end() - 1)->y, (contourIt->begin())->x, (contourIt->begin())->y);
    }
    return image;
}


/*!
 * \brief Processor::Mat2QImage
 * \param src
 * \return 
 */
QImage Processor::Mat2QImage(cv::Mat const& src)
{
    cv::Mat temp;
    
    if(src.channels() == 1){
        cv::cvtColor(src, temp, CV_GRAY2RGB);
    }else if(src.channels() == 3){
        cv::cvtColor(src, temp, CV_BGR2RGB);
    }
    
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();
    return dest;
}

/*!
 * \brief Processor::QImage2Mat
 * \param src
 * \return 
 */
cv::Mat Processor::QImage2Mat(QImage const& src)
{
    cv::Mat tmp;
    
    if(src.format() == QImage::Format_RGB888){
        cv::Mat tmp1(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
        tmp1.copyTo(tmp);
    }else if(src.format() == QImage::Format_RGBA8888){
        cv::Mat tmp1(src.height(), src.width(), CV_8UC4, (uchar*)src.bits(), src.bytesPerLine());
        tmp1.copyTo(tmp);
    }
    
    cv::Mat result(src.height(), src.width(), CV_8UC1);
    cvtColor(tmp, result, CV_BGR2GRAY, 1);
    return result;
}

/*!
 * \brief Processor::mean
 * \param image
 * \param contour
 * \return 
 */
double Processor::mean(cv::Mat image, Contour contour){
    
    cv::Mat imageWithContour(image.clone());
    cv::drawContours(imageWithContour, Contours(1, contour), -1, cv::Scalar(255, 255, 255));
    
    cv::Rect roi(cv::boundingRect(contour));
    cv::Mat crop(image, roi);
    
    cv::Mat mask(cv::Mat::zeros(crop.rows, crop.cols, CV_8UC1));

    cv::drawContours(mask, Contours(1, contour), -1, cv::Scalar(255), CV_FILLED, CV_AA, cv::noArray(), 1, -roi.tl());
    
    auto mean(cv::mean(crop, mask));

    return mean[0];
}

/*!
 * \brief Processor::fixRange
 * \param fps
 * \param frameNumber
 */
void Processor::fixRange(int fps, int frameNumber)
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
void Processor::stopThis()
{
    stop = true;
}

/*!
 * \brief Processor::qrgbToGray
 * \param rgb
 * \return 
 */
unsigned int Processor::qrgbToGray(QRgb rgb)
{
    QColor color(rgb);
    return (color.red() + color.green() + color.blue()) / 3;
}

/*!
 * \brief Processor::setPeriod
 * \param value
 */
void Processor::setPeriod(double value)
{
    period = value;
}


/*!
 * \brief Processor::setSensitivity
 * \param value
 */
void Processor::setSensitivity(unsigned int value)
{
    sensitivity = value;
}

/*!
 * \brief Processor::setEnd
 * \param value
 */
void Processor::setEnd(double value)
{
    end = value;
}

/*!
 * \brief Processor::setStart
 * \param value
 */
void Processor::setStart(double value)
{
    start = value;
}
