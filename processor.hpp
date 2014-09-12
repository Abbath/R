#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QRunnable>
#include <QMessageBox>
#include <QThread>
#include <QPainter>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/photo/photo.hpp>
#include <iostream>

using namespace cv;

/*!
 * \brief The Processor class
 */
class Processor : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Processor(QObject* parent = 0);
    void setFilename(QString _filename){filename = _filename;}
    void setThreshold(int _threshold){threshold = _threshold;}
    void setRect(QRect _rect){rect = _rect;}
    void setAd(bool _ad) { ad = _ad; }
    void run();
    QImage Mat2QImage(const cv::Mat &src);
    QPair<int, double> processImageCV(QImage _image);
    double getStart() const;
    void setStart(double value);
    
    double getEnd() const;
    void setEnd(double value);
    
signals:
    void frameChanged(QImage frame);
    void rectChanged(QRect rect);
    void maxMinBounds(QRect rect);
    void detection();
    void progress(int);
    void time(double);
    void graphL(const QVector<int> &v, const QVector<double>&v0);
    void graphM(const QVector<double> &v, const QVector<double>&v0);
public slots:
    void stopThis();
private:
    unsigned int qrgbToGray(QRgb rgb);
private:
    QString filename;
    QRect rect;
    unsigned int threshold;
    double start, end;
    volatile bool stop;
    bool ad;
    cv::Mat QImage2Mat(const QImage &src);
    double mean(cv::Mat image, std::vector<cv::Point> contour);
    QPair<int, double> processImageCVMat(cv::Mat &m);
    QImage drawOnQImage(QImage image, std::vector<std::vector<cv::Point>> contours);
    QRect autoDetect();
};

#endif // VIDEOPROCESSOR_H
