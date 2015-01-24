#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <QtGui>
#include <string>
#include <fstream>
#include <cmath>

namespace Utils{
    namespace Video{
        int getAVIFPS(std::string filename);
    }
    namespace Image {
    cv::Mat QImage2Mat(QImage &src);
    QImage Mat2QImage(const cv::Mat &src);
    }
}
#endif // UTILS_HPP

