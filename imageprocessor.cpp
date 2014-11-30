#include "imageprocessor.hpp"

/*!
 * \brief ImageProcessor::ImageProcessor
 * \param parent
 */
ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent), lightThreshold(255)
{
}

/*!
 * \brief ImageProcessor::processImage
 * \param _image
 * \return 
 */
QPair<int, double> ImageProcessor::process(QImage _image)
{
    assert(!_image.isNull());
    if(_image.isNull()){
        return qMakePair(0, 0.0l);
    }
    cv::Mat m = ImageConverter::QImage2Mat(_image);
    return process(m);
}

/*!
 * \brief ImageProcessor::processImage
 * \param m
 * \return 
 */
QPair<int, double> ImageProcessor::process(cv::Mat &m)
{
    assert(!m.empty() && m.channels() == 1);
    cv::Mat matCopy;
    m.copyTo(matCopy);
    
    cv::threshold(m, m, lightThreshold - 1, 255, cv::THRESH_BINARY);
    
    cv::Rect rec(bounds.left(), bounds.top(), bounds.width(), bounds.height());
    m = m(rec);
    
    Contours contours;
    std::vector<cv::Vec4i> h;
    cv::findContours(m, contours, h, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    
    double lightArea = 0;
    double lightMean = 0;
    
    for (unsigned int i = 0; i < contours.size(); ++i){
        for(unsigned int j = 0; j < contours[i].size(); ++j){
            contours[i][j].x += rec.x;
            contours[i][j].y += rec.y;
        }
        if(h[i][3] < 0){
            auto localLightArea = cv::contourArea(contours[i]);
            lightArea += localLightArea;
            
            lightMean += localLightArea * mean(matCopy, contours[i]);
        }
    }
    
    if(lightArea != 0){
        lightMean /= lightArea;
    }else{
        lightMean = 0;
    }
    
    lightMean = std::max(lightMean, double(lightThreshold));
    
    emit frameChanged(ImageConverter::Mat2QImage(matCopy), contours);
    
    QPair<int, double> result(lightArea, lightMean);
    
    return result;
}

/*!
 * \brief ImageProcessor::setLightThreshold
 * \param th
 */
void ImageProcessor::setLightThreshold(unsigned th)
{
    lightThreshold = th;
}

/*!
 * \brief ImageProcessor::setBounds
 * \param _bounds
 */
void ImageProcessor::setBounds(const QRect &_bounds)
{
    if(_bounds != QRect(0, 0, 0, 0)){
        bounds = _bounds.normalized();
    }
}

/*!
 * \brief ImageProcessor::mean
 * \param image
 * \param contour
 * \return 
 */
double ImageProcessor::mean(cv::Mat image, Contour contour)
{
    assert(!image.empty());
    
    if(contour.size() == 0){
        return 0.0l;
    }
    
    cv::Mat imageWithContour(image.clone());
    cv::drawContours(imageWithContour, Contours(1, contour), -1, cv::Scalar(255, 255, 255));
    
    cv::Rect roi(cv::boundingRect(contour));
    cv::Mat crop(image, roi);
    
    cv::Mat mask(cv::Mat::zeros(crop.rows, crop.cols, CV_8UC1));
    
    cv::drawContours(mask, Contours(1, contour), -1, cv::Scalar(255), CV_FILLED, CV_AA, cv::noArray(), 1, -roi.tl());
    
    auto meanValue = cv::mean(crop, mask);
    
    return meanValue[0];
}

