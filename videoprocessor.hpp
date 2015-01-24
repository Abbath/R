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
#include "imageprocessor.hpp"
#include "lightdetector.hpp"
#include "capturewrapper.hpp"
#include "imagestorage.hpp"
#include "utils.hpp"

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
    void setAd(bool _ad) { autodetection = _ad; }
    void run();
    QPair<int, double> processImage(QImage _image);

    void setStart(double value);
    void setEnd(double value);
    
    void setSensitivity(unsigned int value);
    void setPeriod(double value);
    void setImageProcessor(ImageProcessor* ip);
    
    ~VideoProcessor();
    
signals:
    void rectChanged(QRect rect);
    void detection();
    void progress(int);
    void time(double);
    void displayResults(std::shared_ptr<Results> r);

public slots:
    void stopThis();

private:
    ImageProcessor* imageProcessor;
    LightDetector* lightDetector;
    QString filename;
    QPair<double, double> range;
    // TODO Replace with atomic_bool
    volatile bool stop;
    bool autodetection;
    void writeSettings();
    void readSettings();
};

#endif // PROCESSOR_H
