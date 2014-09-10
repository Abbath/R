#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QRunnable>
#include <QMessageBox>
#include <QThread>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;

class Processor : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Processor(QObject* parent = 0);
    void setFilename(QString _filename){filename = _filename;}
    void setThreshold(int _threshold){threshold = _threshold;}
    void setRect(QRect _rect){rect = _rect;}
    void run();
    QImage IplImage2QImage(const IplImage *iplImage);
    QPair<int, double> processImage(QImage _image);
    QPair<int, double> processImageCV(QImage _image);
    double getStart() const;
    void setStart(double value);
    
    double getEnd() const;
    void setEnd(double value);
    
signals:
    void frameChanged(QImage frame);
    void maxMinBounds(QRect rect);
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
    unsigned int threshold = 255;
    double start = -1, end = -1;
    volatile bool stop = false;
    cv::Mat QImage2Mat(const QImage &src);
    QImage Mat2QImage(const cv::Mat &src);
    double mean(cv::Mat image, std::vector<cv::Point> contour);
    QPair<int, double> processImageCVMat(cv::Mat &m);
};

#endif // VIDEOPROCESSOR_H
