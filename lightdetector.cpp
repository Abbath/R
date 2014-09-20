#include "lightdetector.hpp"

/*!
 * \brief LightDetector::LightDetector
 * \param parent
 */
LightDetector::LightDetector(QObject *parent) :
    QObject(parent),
    stop(false),
    period(1.0),
    sensitivity(60)
{
    QSettings settings("CAD", "R");
    period = settings.value("LD/period").toDouble();
    sensitivity = settings.value("LD/sens").toInt();
}

/*!
 * \brief LightDetector::detectLight
 * \param filename
 * \param range
 * \return 
 */
QRect LightDetector::detectLight(QString filename, QPair<double, double> range)
{
    cv::VideoCapture capture(filename.toStdString().c_str());
    
    if (!capture.isOpened()) {
        QMessageBox::warning(0, "Error", "Capture failed (file not found?)\n");
        return QRect(0, 0, 0, 0);;
    }
    
    cv::Mat frame, oldframe, dif;
    
    int frameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
    int fps = capture.get(CV_CAP_PROP_FPS); 
    
    fixRange(range, fps, frameNumber);
    
    if(period >= fabs(range.second - range.first) && (range.second - range.first) * fps >= 10){
        period = (range.second - range.first) / 2;
    }else if((range.second - range.first) * fps < 10){
        QMessageBox::warning(0, "Error", "Too small range for autodetection\n");        
        return QRect(0, 0, 0, 0);;
    }
    
    if(range.first >= range.second && range.first > std::numeric_limits<double>::epsilon() && range.second > std::numeric_limits<double>::epsilon()){
        return QRect(0, 0, 0, 0);;
    }
    
    unsigned absIndex = 0;
    
    for(auto i = 0u; i < unsigned(range.first * fps); ++i, ++absIndex){
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
    
    for(auto i = unsigned((range.first + period) * fps) + 2u; i < unsigned(range.second * fps); ++i){
        if(stop){
            stop = false;
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
            
            int relIndex = absIndex - int(range.first * fps);
            
            emit progress(relIndex / (fps * (range.second -range.first) / 100.0));
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
        boundRect[i] = cv::boundingRect(contours[i]);
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
    return result;
}

/*!
 * \brief LightDetector::fixRange
 * \param range
 * \param fps
 * \param frameNumber
 */
void LightDetector::fixRange(QPair<double, double>& range, int fps, int frameNumber)
{
    if(range.first == -1){
        range.first = 0;
    }    
    if(range.second == -1){
        range.second = frameNumber / double(fps);
    }
    if(range.first * fps > frameNumber){
        range.first = frameNumber;
    }
    if(range.second * fps > frameNumber){
        range.second = frameNumber;
    }
    if(range.first < 0.0){
        range.first = 0.0;
    }
    if(range.second < 0.0){
        range.second = 0.0;
    }    
}

/*!
 * \brief LightDetector::setSensitivity
 * \param sens
 */
void LightDetector::setSensitivity(int sens)
{
    sensitivity = sens;
}

/*!
 * \brief LightDetector::setPeriod
 * \param _period
 */
void LightDetector::setPeriod(double _period)
{
    period = _period;
}

/*!
 * \brief LightDetector::stopThis
 */
void LightDetector::stopThis()
{
    stop = true;
}

/*!
 * \brief LightDetector::~LightDetector
 */
LightDetector::~LightDetector()
{
    QSettings settings("CAD", "R");
    settings.beginGroup("LD");
    settings.setValue("period", period);
    settings.setValue("sens", sensitivity);
    settings.endGroup();
}
