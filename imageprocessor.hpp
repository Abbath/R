#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <QObject>
#include <QtGui>
#include "utils.hpp"

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
    QPair<int, double> process(QImage _image);
    QPair<int, double> process(cv::Mat &m);
    
    void setLightThreshold(unsigned th);
    void setBounds(const QRect& _bounds);
    
    QSize getH_size() const;
    void setH_size(const QSize &value);
    
    void testProcessRealData();
    void testProcessMarginal();
signals:
    void frameChanged(QImage image, Contours contours);
    void histogram(QImage hist);
public slots:
    
private:
    unsigned int lightThreshold;
    QRect bounds;
    QSize h_size;
    double mean(cv::Mat image, Contour contour);
    cv::Mat hist(cv::Mat im);
};

#endif // IMAGEPROCESSOR_HPP
