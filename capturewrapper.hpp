#ifndef CAPTUREWRAPPER_HPP
#define CAPTUREWRAPPER_HPP

#include <QtCore>
#include <opencv2/highgui/highgui.hpp>

/*!
 * \brief The CaptureError class
 */
class CaptureError {
public:
    CaptureError(QString m) : message(m) {}
    QString getMessage() { return message; }
private:
    QString message;
};
         
/*!
 * \brief The CaptureWrapper class
 */
class CaptureWrapper
{
public:
    CaptureWrapper(QString filename);
    void isOpened();
    void grab();
    void read(cv::Mat& frame);
    double get(int id);
    ~CaptureWrapper();
    CaptureWrapper &operator>> (cv::Mat& frame); 
private:
    cv::VideoCapture capture;
};

#endif // CAPTUREWRAPPER_HPP
