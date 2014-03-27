#include "videoprocessor.h"
#include <fstream>
unsigned int VideoProcessor::qrgbToGray(QRgb rgb)
{
    QColor color(rgb);
    return (color.red()+color.green()+color.blue())/3;
}

VideoProcessor::VideoProcessor(QObject *parent)
{
    Q_UNUSED(parent)
    setAutoDelete(false);
}

void VideoProcessor::run()
{
    std::ofstream f("log1.txt");
    QVector<int> res;
    QVector<double> resm;
    CvCapture * capture = cvCaptureFromAVI(filename.toStdString().c_str());
    if(!capture)
    {
        QMessageBox::warning(0, "Error", "cvCaptureFromAVI failed (file not found?)\n");
        return;
    }
    IplImage* frame = NULL;
    res.clear();
    resm.clear();
    while ((frame = cvQueryFrame(capture))) {
        if(stop){
            stop = false;
            break;
        }
        QImage image = IplImage2QImage(frame).mirrored(false, true);
        QPair<int,double> pr = processImage(image);
        //QMessageBox::information(0,QString::number(pr.first),QString::number(pr.second));
        //QThread::currentThread()->sleep(100);
        //f << pr.first << " " << pr.second << std::endl;
        res.push_back(pr.first);
        resm.push_back(pr.second);
        QThread::currentThread()->usleep(500);
    }
    cvReleaseCapture(&capture);
    emit graphL(res);
    emit graphM(resm);
}

QPair<int,double> VideoProcessor::processImage(QImage _image)
{
    std::ofstream f("log.txt");
    int counter = 0;
    double sum = 0.0;
    QImage image = _image;
    if(!_image.isNull()){
        int histogram[256] = {0};
        unsigned int minx = rect.right(), miny = rect.bottom() , maxx = rect.left(), maxy = rect.left();
        unsigned int x1 = rect.left(), x2 = rect.right(), y1 = rect.top(), y2 = rect.bottom();
        for (unsigned  i = x1 + 1; i != x2; x1 < x2 ? ++i : --i ) {
            for(unsigned j = y1 + 1; j != y2; y1 < y2 ? ++j : --j) {
                if(qrgbToGray(_image.pixel(i, j)) >= threshold) {
                    //std::cout << _image.pixel(i,j) << std::endl;
                    histogram[qrgbToGray(_image.pixel(i, j))]++;
                    counter++;
                }
                if(qrgbToGray(_image.pixel(i,j)) < threshold && (qrgbToGray(_image.pixel(i + 1, j)) >= threshold ||
                                                     qrgbToGray(_image.pixel(i, j + 1)) >= threshold ||
                                                     qrgbToGray(_image.pixel(i - 1, j)) >= threshold ||
                                                     qrgbToGray(_image.pixel(i, j - 1)) >= threshold )) {
                    image.setPixel(i, j, 0x00ff00);
                    if(minx > i) {
                        minx = i;
                    }
                    if(miny < j) {
                        miny = j;
                    }
                    if(maxx < i) {
                        maxx = i;
                    }
                    if(maxy < j) {
                        maxy = j;
                    }
                }
            }
        }

        for(unsigned i = x1; i != (x1 < x2 ? x2 + 1 : x2 - 1); x1 < x2 ? ++i : --i) {
            if( qrgbToGray(_image.pixel(i, y1)) >= threshold ) {
                image.setPixel(i, y1, 0x00ff00);
            }
            if( qrgbToGray(_image.pixel(i, y2)) >= threshold ) {
                image.setPixel(i, y2, 0x00ff00);
            }
        }
        for(unsigned i = y1; i != (y1 < y2 ? y2 + 1 : y2 - 1); y1 < y2 ? ++i : --i) {
            if( qrgbToGray(_image.pixel(x1, i)) >= threshold) {
                image.setPixel(x1, i, 0x00ff00);
            }
            if( qrgbToGray(_image.pixel(x2, i)) >= threshold) {
                image.setPixel(x2, i, 0x00ff00);
            }
        }
        for( int i = 0; i < 256 ; ++i){
            sum += histogram[i]*i;
        }
        emit frameChanged(image);
    }
    sum = (counter != 0 ? sum/counter : 0);
    QPair<int,double> res(counter, sum);
   // f << counter << " " << sum << std::endl;
    return res;
}

QImage VideoProcessor::IplImage2QImage(const IplImage *iplImage)
{
    int height = iplImage->height;
    int width = iplImage->width;

    const uchar *qImageBuffer =(const uchar*)iplImage->imageData;
    QImage img(qImageBuffer, width, height, QImage::Format_RGB888);
    return img.rgbSwapped();
}

void VideoProcessor::stopThis()
{
    stop = true;
}
