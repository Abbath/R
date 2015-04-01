#include "imageprocessor.hpp"

/*!
 * \brief ImageProcessor::ImageProcessor
 * \param parent
 */
ImageProcessor::ImageProcessor(QObject *parent) :
    QObject(parent), lightThreshold(255)
{
#ifdef QT_DEBUG
    testProcessMarginal();
    testProcessRealData();
#endif
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
    if(bounds.right() >= _image.width()){
        bounds.setRight(_image.width() - 1);
    }
    if(bounds.bottom() >= _image.height()){
        bounds.setBottom(_image.height()-1);
    }
    
    cv::Mat m = Utils::Image::QImage2Mat(_image);
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
    
    //lightMean = std::max(lightMean, double(lightThreshold));
    
    hist(matCopy(rec), contours, h);
    
    emit frameChanged(Utils::Image::Mat2QImage(matCopy), contours);
    
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
cv::Mat ImageProcessor::hist(cv::Mat im, const Contours& contours, const std::vector<cv::Vec4i> &h)
{
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = { range };
    
    cv::Mat hist;
    cv::Mat crop;
    
    if(contours.size() > 0){
        cv::Rect roi(cv::boundingRect(contours[0]));
        
        for(int i = 1 ; i < contours.size(); ++i){
            if(h[i][3] < 0 ){
                roi = roi | cv::boundingRect(contours[i]);
            }
        }
        
        if(0 <= roi.x && 0 <= roi.width && roi.x + roi.width <= im.cols && 0 <= roi.y && 0 <= roi.height && roi.y + roi.height <= im.rows){
            crop = cv::Mat(im, roi);
        }else{
            crop = cv::Mat(im);
        }
        cv::Mat mask(cv::Mat::zeros(crop.rows, crop.cols, CV_8UC1));
        cv::drawContours(mask, contours, -1, cv::Scalar(255), CV_FILLED, CV_AA, cv::noArray(), 1, -roi.tl());
        
        cv::calcHist(&crop, 1, 0, mask, hist, 1, &histSize, &histRange, true, false);
    }    else{
        cv::calcHist(&im, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);
    }
    
    int hist_w = h_size.width(); int hist_h = h_size.height();
    int bin_w = cvRound( (double) hist_w/histSize );
    
    cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );
    
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
    int max = 0; 
    double sum = 0.0;
    if(contours.size() > 0){
        for( int i = 0; i < histSize-1; i++ )
        {
            max = std::max(max, cvRound(hist.at<float>(i)));
            sum += hist.at<float>(i);
//            cv::line( histImage, cv::Point( bin_w*(i), hist_h /*- cvRound(hist.at<float>(i-1))*/ ) ,
//                      cv::Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
//                      cv::Scalar( i, i, i), 1, 8, 0  );
            cv::line( histImage, cv::Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ) ,
                      cv::Point( bin_w*(i+1), hist_h - cvRound(hist.at<float>(i+1)) ),
                      cv::Scalar( 0, 255, 0), 1, 8, 0  );
        }
    }
    
    emit histogram(Utils::Image::Mat2QImage(histImage), bin_w*255, max/sum); 
    
    return histImage;
}


void ImageProcessor::testProcessMarginal(){
    QImage image(400, 400, QImage::Format_ARGB32);
    image.fill(Qt::black);
    bounds = QRect(100, 200, 100, 200);
    lightThreshold = 128;
    h_size = image.size();
    auto test_res = process(image);
    Q_ASSERT_X(test_res.first == 0 && abs(test_res.second) < std::numeric_limits<double>::epsilon(), "ImageProcessor::testProcessMarginal", "Processing works wrong 1");
    
    lightThreshold = 0;
    auto test_res1 = process(image);
    Q_ASSERT_X(test_res1.first == 19109 && abs(test_res1.second) < std::numeric_limits<double>::epsilon(), "ImageProcessor::testProcessMarginal", "Processing works wrong 2");
    
    lightThreshold = 255;
    auto test_res2 = process(image);
    Q_ASSERT_X(test_res2.first == 0 && abs(test_res2.second) < std::numeric_limits<double>::epsilon(), "ImageProcessor::testProcessMarginal", "Processing works wrong 3");
    
    image.fill(Qt::white);
    
    lightThreshold = 128;
    auto test_res3 = process(image);
    Q_ASSERT_X(test_res3.first == 19109 && abs(test_res3.second - 255.0) < std::numeric_limits<double>::epsilon(), "ImageProcessor::testProcessMarginal", "Processing works wrong 4");
    
    lightThreshold = 0;
    auto test_res4 = process(image);
    Q_ASSERT_X(test_res4.first == 19109 && abs(test_res4.second - 255.0) < std::numeric_limits<double>::epsilon(), "ImageProcessor::testProcessMarginal", "Processing works wrong 5");
    
    lightThreshold = 255;
    auto test_res5 = process(image);
    Q_ASSERT_X(test_res5.first == 19109 && abs(test_res5.second - 255.0) < std::numeric_limits<double>::epsilon(), "ImageProcessor::testProcessMarginal", "Processing works wrong 6");
    
    lightThreshold = 128;
    bounds = QRect(0,0,500,500);
    auto test_res6 = process(image);
    //qDebug() << test_res6.first << " " << test_res6.second;
    Q_ASSERT_X(test_res6.first == 157609 && abs(test_res6.second - 255.0) < std::numeric_limits<double>::epsilon(), "ImageProcessor::testProcessMarginal", "Processing works wrong 7");
    
}

void ImageProcessor::testProcessRealData(){
    QImage image("4.bmp");
    bounds = QRect(100, 200, 100, 200);
    lightThreshold = 128;
    h_size = image.size();
    auto test_res = process(image);
    Q_ASSERT_X(test_res.first == 286 && abs(test_res.second - 150.402) < 0.01, "ImageProcessor::testProcess", "Processing works wrong");
}
