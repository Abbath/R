#ifndef IMAGECONVERTER_HPP
#define IMAGECONVERTER_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <QtGui>

/*!
 * \brief The ImageConverter class
 */
class ImageConverter
{
public:
    static cv::Mat QImage2Mat(const QImage &src);
    static QImage Mat2QImage(const cv::Mat &src);
    ImageConverter();
};

#endif // IMAGECONVERTER_HPP
