#ifndef LIGHTDETECTOR_HPP
#define LIGHTDETECTOR_HPP

#include <QObject>
#include <QtCore>
#include <QtGui>
#include <iostream>
#include <fstream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <QMessageBox>
#include "capturewrapper.hpp"
#include "utils.hpp"

/*!
 * \brief The LightDetector class
 */
class LightDetector : public QObject
{
    Q_OBJECT
    
    typedef std::vector<cv::Point> Contour;
    typedef std::vector<Contour> Contours;
public:
    explicit LightDetector(QObject *parent = 0);
    QRect detectLight(QString filename, QPair<double, double> range);
    void fixRange(QPair<double, double> &range, int fps, int frameNumber);
    void setSensitivity(int sens);
    void setPeriod(double period);
    void stopThis();
    ~LightDetector();
    void readSettings();
    void writeSettings();
signals:
    void progress(int);
public slots:
    
private:
    volatile bool stop;
    double period;
    unsigned sensitivity;
};

#endif // LIGHTDETECTOR_HPP
