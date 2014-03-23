#include "converter.hpp"

Converter::Converter()
{

}

QVector<Point> Converter::convert(QImage &image, int left, int top, int right, int bottom){
    QVector<Point> points;
    QImage pix(1366, 768, image.format());
    Point origin;
    if(left < 0) left = 0;
    if(top < 0) top = 0;
    if(right > image.width()) right = image.width();
    if(bottom > image.height()) bottom = image.height();
    points.clear();
    pix.fill(Qt::black);
    if(left > right){
        left ^= right;
        right ^= left;
        left ^= right;
    }
    if(top > bottom){
        top ^= bottom;
        bottom ^= top;
        top ^= bottom;
    }
    for( int i = left; i < right; ++i){
        for( int j = top; j < bottom; ++j){
            Point p;
            p.x = i;
            p.y = j;
            p.z = qGray(image.pixel(i, j));
            p.c = p.z;
            QVector<int> v;
            if(i!=left) v.push_back(qGray(image.pixel(i - 1, j)));
            if(i < right-1) v.push_back(qGray(image.pixel(i + 1, j)));
            if(j!=top) v.push_back(qGray(image.pixel(i, j - 1)));
            if(j < bottom-1) v.push_back(qGray(image.pixel(i, j + 1)));
            if(i!=left && j!= top) v.push_back(qGray(image.pixel(i - 1, j - 1)));
            if(i < right-1 && j!=top) v.push_back(qGray(image.pixel(i + 1, j - 1)));
            if(j < bottom-1 && i!=left) v.push_back(qGray(image.pixel(i - 1, j + 1)));
            if(i < right-1 && j < bottom-1) v.push_back(qGray(image.pixel(i + 1, j + 1)));
            int min = *(std::min_element(v.begin(), v.end()));
            if(min < qGray(image.pixel(i,j))){
                for( unsigned k = 0; k < p.c-min; ++k){
                    Point p0;
                    p0.x = i;
                    p0.y = j;
                    p0.z = qGray(image.pixel(i, j)) - (k + 1);
                    p0.c = qGray(image.pixel(i, j));
                    points.push_back(p0);
                }
            }
            points.push_back(p);
        }
    }
    origin.x = 0;
    origin.y = 0;
    origin.z = 0.0;
    rotate(points, origin, 3.1415/180*35.2, 3.1415/4, -3.1415/4);
    return points;
}
void Converter::rotate(QVector<Point> &points, Point origin, double angle_x, double angle_y, double angle_z){
    for(auto it = points.begin(); it != points.end(); ++it){
        double xt = it->x; double yt = it->y; double zt = it->z;
        if(fabs(angle_x) > std::numeric_limits<double>::epsilon()){
            it->y = cos(angle_x) * (yt-origin.y) - sin(angle_x) * (zt-origin.z);
            it->z = sin(angle_x) * (yt-origin.y) + cos(angle_x) * (zt-origin.z);
        }
        xt = it->x; yt = it->y; zt = it->z;
        if(fabs(angle_y) > std::numeric_limits<double>::epsilon()){
            it->x = cos(angle_y) * (xt-origin.x) + sin(angle_y) * (zt-origin.z);
            it->z = -sin(angle_y) * (xt-origin.x) + cos(angle_y) * (zt-origin.z);
        }
        xt = it->x; yt = it->y; zt = it->z;
        if(fabs(angle_z) > std::numeric_limits<double>::epsilon()){
            it->x = cos(angle_z) * (xt-origin.x) - sin(angle_z) * (yt-origin.y);
            it->y = sin(angle_z) * (xt-origin.x) + cos(angle_z) * (yt-origin.y);
        }
    }
}

int Converter::processVideo(QString s)
{
    CvCapture * capture = cvCaptureFromAVI(s.toStdString().c_str());
    if(!capture)
    {
        QMessageBox::warning(0, "Error", "cvCaptureFromAVI failed (file not found?)\n");
        return 0;
    }
    /*QWidget * w = new QWidget();
    w->setLayout(new QGridLayout);
    QProgressBar * b = new QProgressBar();
    w->layout()->addWidget(b);
    w->show();*/
    //int fps = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    // qDebug() << "* FPS: %d" <<  fps << "\n";
    IplImage* frame = NULL;
    int frame_number = 0;
    while ((frame = cvQueryFrame(capture))) {
        /*char filename[100];
        strcpy(filename, "frame_");
        char frame_id[30];
        sprintf(frame_id,"%d",frame_number);
        strcat(filename, frame_id);
        strcat(filename, ".bmp");
        QMatrix matrix;
        matrix.rotate(180);
        QImage img = IplImage2QImage(frame).mirrored(false, true);
        img.save(QString(filename), "BMP", 100);*/
        frame_number++;
       // b->setValue((b->value()+1)%100);
    }
    cvReleaseCapture(&capture);
    return frame_number;
}

QImage Converter::IplImage2QImage(const IplImage *iplImage)
{
    int height = iplImage->height;
    int width = iplImage->width;

    const uchar *qImageBuffer =(const uchar*)iplImage->imageData;
    QImage img(qImageBuffer, width, height, QImage::Format_RGB888);
    return img.rgbSwapped();
}
