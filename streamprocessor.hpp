#ifndef STREAMPROCESSOR_HPP
#define STREAMPROCESSOR_HPP

#include <QObject>
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
#include "lightdetector.hpp"
#include "capturewrapper.hpp"
#include "imagestorage.hpp"

class StreamProcessor : public QObject, public QRunnable
{
    Q_OBJECT

    typedef std::vector<cv::Point> Contour;
    typedef std::vector<Contour> Contours;
public:
    explicit StreamProcessor(QObject *parent = 0);
    void setDeviceNumber(quint32 devnum){ deviceNumber = devnum; }
    void run();
    QPair<int, double> processImage(QImage _image);

    void setStart(double value);
    void setEnd(double value);
    void setRecord(bool r);
    void setFilename(QString filename);
    void setFPS(int fps);
    
    void setImageProcessor(ImageProcessor* ip);
    
    ~StreamProcessor();
    
signals:
    void frameChanged(QImage frame);
    void time(double);
    void displayResults(std::shared_ptr<Results> r);

public slots:
    void stopThis();

private:
    ImageProcessor* imageProcessor;
    quint32 deviceNumber;
    QString filename;
    int FPS;
    qint32 end;
    // TODO Replace with atomic_bool
    volatile bool stop;
    bool record;
    void writeSettings();
    void readSettings();
};

#endif // STREAMPROCESSOR_HPP
