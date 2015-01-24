#include "utils.hpp"

namespace Utils{
namespace Video{

int getAVIFPS(std::string filename){
    std::ifstream f(filename.c_str(), std::ifstream::binary | std::ifstream::in);
    if(f.is_open()){
        const int DISTANCE_FROM_FILE_BEGIN = 108;
        const int DISTANCE_FROM_SECTION_BEGIN = 20;
        f.seekg(DISTANCE_FROM_FILE_BEGIN + DISTANCE_FROM_SECTION_BEGIN, f.beg);
        int scale;
        f.read((char*)&scale, 4);
        int rate;
        f.read((char*)&rate, 4);
        f.close();
        return floor(double(rate / scale)+0.5);
    }else{
        return -1;
    }
}

}
namespace Image {

cv::Mat QImage2Mat(QImage &src)
{
    assert(!src.isNull());
    if(src.format() != QImage::Format_RGB888){
        src = src.convertToFormat(QImage::Format_RGB888);
    }
    cv::Mat tmp(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
    cv::Mat result(src.height(), src.width(), CV_8UC1);
    cvtColor(tmp, result, CV_BGR2GRAY, 1);
    return result;
}

QImage Mat2QImage(const cv::Mat &src)
{
    assert(!src.empty());
    cv::Mat temp;
    
    if(src.channels() == 1){
        cv::cvtColor(src, temp, CV_GRAY2RGB);
    }else if(src.channels() == 3){
        cv::cvtColor(src, temp, CV_BGR2RGB);
    }
    
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();
    return dest;
}
}
}
