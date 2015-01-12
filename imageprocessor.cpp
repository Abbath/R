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
    
    cv::Mat im = hist(matCopy(rec));
        
    emit frameChanged(ImageConverter::Mat2QImage(matCopy), contours);
    emit histogram(ImageConverter::Mat2QImage(im)); 
    
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
QSize ImageProcessor::getH_size() const
{
    return h_size;
}

void ImageProcessor::setH_size(const QSize &value)
{
    h_size = value;
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

/*!
 * \brief ImageProcessor::hist
 * \param im
 * \return 
 */
cv::Mat ImageProcessor::hist(cv::Mat im)
{
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = { range };
    
    cv::Mat hist;
    
    cv::calcHist(&im, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);
       
    int hist_w = h_size.width(); int hist_h = h_size.height();
    int bin_w = cvRound( (double) hist_w/histSize );
    
    cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );
    
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    
    for( int i = 0; i < histSize; i++ )
    {
        cv::line( histImage, cv::Point( bin_w*(i), hist_h /*- cvRound(hist.at<float>(i-1))*/ ) ,
              cv::Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
              cv::Scalar( 0, 255, 0), 1, 8, 0  );
    }
    
    return histImage;
}

