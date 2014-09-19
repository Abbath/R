#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QRunnable>
#include <QMessageBox>
#include <QThread>
#include <QPainter>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <memory>
#include "results.hpp"
#include "imageconverter.hpp"
#include "imageprocessor.hpp"

/*!
 * \brief The Processor class
 */
class VideoProcessor : public QObject, public QRunnable
{
    Q_OBJECT
    
    typedef std::vector<cv::Point> Contour;
    typedef std::vector<Contour> Contours;
public:
    explicit VideoProcessor(QObject* parent = 0);
    void setFilename(QString _filename){ filename = _filename; }
    void setAd(bool _ad) { ad = _ad; }
    void run();
    QPair<int, double> processImage(QImage _image);

    void setStart(double value);
    void setEnd(double value);
    
    void setSensitivity(unsigned int value);
    void setPeriod(double value);
    void setImageProcessor(ImageProcessor* ip);
    
    ~VideoProcessor();
    
signals:
    void frameChanged(QImage frame);
    void rectChanged(QRect rect);
    void detection();
    void progress(int);
    void time(double);
    void displayResults(std::shared_ptr<Results> r);

public slots:
    void stopThis();

private:
    ImageProcessor* imageProcessor;
    QString filename;
    double start, end;
    volatile bool stop;
    bool ad;
    unsigned int sensitivity;
    double period;
    QRect autoDetectLight();
    void fixRange(int fps, int frameNumber);
    void writeSettings();
    void readSettings();
};

#endif // PROCESSOR_H
