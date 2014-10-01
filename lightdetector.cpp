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
    readSettings();
}

/*!
 * \brief LightDetector::readSettings
 */
void LightDetector::readSettings()
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
    assert(!filename.isEmpty());
    stop = false;
    CaptureWrapper capture(filename);
    cv::Mat tmp;
    try{
        capture.isOpened();
        
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
            capture.grab();
        }
        
        capture.read(oldframe);
        
        for(auto i = 0u; i < unsigned(period*fps); ++i){
            capture.grab();
        }
        
        capture.read(frame);
        
        cv::absdiff(frame, oldframe, dif);
        cv::threshold(dif, dif, (100 - sensitivity), 255, cv::THRESH_BINARY);
        
        tmp = dif.clone();
        
        for(auto i = unsigned((range.first + period) * fps) + 2u; i < unsigned(range.second * fps); ++i){
            if(stop){
                stop = false;
                return QRect(0, 0, 0, 0);
            }
            
            absIndex++;
            
            if(i % unsigned(period*fps) == 0){
                oldframe = frame.clone();
                
                capture.read(frame);
                
                cv::absdiff(frame, oldframe, dif);
                cv::threshold(dif, dif, (100 - sensitivity), 255, cv::THRESH_BINARY);
                
                cv::bitwise_or(dif, tmp, tmp);
                
                int relIndex = absIndex - int(range.first * fps);
                
                emit progress(relIndex / (fps * (range.second -range.first) / 100.0));
            }else{
                capture.grab();
            }
            QThread::currentThread()->usleep(50);
        }
    }
    catch(CaptureError e){
        QMessageBox::warning(0, "Error", e.getMessage());
        return QRect(0, 0, 0, 0);
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
    assert(fps != 0);
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
void LightDetector::writeSettings()
{
    QSettings settings("CAD", "R");
    settings.beginGroup("LD");
    settings.setValue("period", period);
    settings.setValue("sens", sensitivity);
    settings.endGroup();    
}

LightDetector::~LightDetector()
{
    writeSettings();
}
