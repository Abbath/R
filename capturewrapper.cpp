#include "capturewrapper.hpp"

/*!
 * \brief CaptureWrapper::CaptureWrapper
 * \param filename
 */
CaptureWrapper::CaptureWrapper(QString filename) : capture(filename.toStdString().c_str())
{
}

/*!
 * \brief CaptureWrapper::isOpened
 */
void CaptureWrapper::isOpened()
{
    if(!capture.isOpened()){
        throw CaptureError("Can't open video!");
    }
}

/*!
 * \brief CaptureWrapper::grab
 */
void CaptureWrapper::grab()
{
    if(!capture.grab()){
        throw CaptureError("Can't grab frame!");
    }
}

/*!
 * \brief CaptureWrapper::read
 * \param frame
 */
void CaptureWrapper::read(cv::Mat &frame)
{
    if(!capture.read(frame)){
        throw CaptureError("Can't read frame!");
    }
}

/*!
 * \brief CaptureWrapper::get
 * \param id
 * \return 
 */
double CaptureWrapper::get(int id)
{
    return capture.get(id);
}

/*!
 * \brief CaptureWrapper::~CaptureWrapper
 */
CaptureWrapper::~CaptureWrapper()
{
    capture.release();
}

/*!
 * \brief CaptureWrapper::operator >>
 * \param frame
 * \return 
 */
CaptureWrapper& CaptureWrapper::operator>>(cv::Mat &frame)
{
    if(!capture.read(frame)){
        throw CaptureError("Can't read frame!");
    }else{
        return *this;
    }
}
