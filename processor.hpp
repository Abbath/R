#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QRunnable>
#include <QMessageBox>
#include <QThread>
#include <QPainter>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

/*!
 * \brief The Processor class
 */
class Processor : public QObject, public QRunnable
{
    Q_OBJECT
    
    typedef std::vector<cv::Point> Contour;
    typedef std::vector<Contour> Contours;
public:
    explicit Processor(QObject* parent = 0);
    void setFilename(QString _filename){ filename = _filename; }
    void setThreshold(int _threshold){ lightThreshold = _threshold; }
    void setBounds(QRect _rect){ rect = _rect; }
    void setAd(bool _ad) { ad = _ad; }
    void run();
    QImage Mat2QImage(const cv::Mat &src);
    QPair<int, double> processImageCV(QImage _image);

    void setStart(double value);
    void setEnd(double value);
    
    void setSensitivity(unsigned int value);
    void setPeriod(double value);
    
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
    unsigned int lightThreshold;
    double start, end;
    volatile bool stop;
    bool ad;
    unsigned int sensitivity;
    double period;
    cv::Mat QImage2Mat(const QImage &src);
    double mean(cv::Mat image, Contour contour);
    QPair<int, double> processImageCVMat(cv::Mat &m);
    QImage drawOnQImage(QImage image, Contours contours);
    QRect autoDetectLight();
    void fixRange(int fps, int frameNumber);
};

#endif // VIDEOPROCESSOR_H
