#include "imageconverter.hpp"

/*!
 * \brief ImageConverter::QImage2Mat
 * \param src
 * \return 
 */
cv::Mat ImageConverter::QImage2Mat(const QImage &src)
{
    cv::Mat tmp;
    
    if(src.format() == QImage::Format_RGB888){
        cv::Mat tmp1(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
        tmp1.copyTo(tmp);
    }else if(src.format() == QImage::Format_RGB32){
        cv::Mat tmp1(src.height(), src.width(), CV_8UC4, (uchar*)src.bits(), src.bytesPerLine());
        tmp1.copyTo(tmp);
    }
    
    cv::Mat result(src.height(), src.width(), CV_8UC1);
    cvtColor(tmp, result, CV_BGR2GRAY, 1);
    return result;
}

/*!
 * \brief ImageConverter::Mat2QImage
 * \param src
 * \return 
 */
QImage ImageConverter::Mat2QImage(const cv::Mat &src)
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

ImageConverter::ImageConverter()
{
}
