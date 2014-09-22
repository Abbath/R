#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <QObject>
#include <QtGui>
#include "imageconverter.hpp"

/*!
 * \brief The ImageProcessor class
 */
class ImageProcessor : public QObject
{
    Q_OBJECT
    typedef std::vector<cv::Point> Contour;
    typedef std::vector<Contour> Contours;

public:
    explicit ImageProcessor(QObject *parent = 0);
    QPair<int, double> processImage(QImage _image);
    QPair<int, double> processImage(cv::Mat &m);
    
    void setLightThreshold(unsigned th);
    void setBounds(QRect _bounds);
    
signals:
    void frameChanged(QImage image, Contours contours);
    
public slots:
    
private:
    unsigned int lightThreshold;
    QRect bounds;
    QImage drawOnQImage(QImage image, Contours contours);
    double mean(cv::Mat image, Contour contour);
};

#endif // IMAGEPROCESSOR_HPP
