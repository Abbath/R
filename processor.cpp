#include "processor.hpp"
#include <fstream>

/*!
 * \brief Processor::qrgbToGray
 * \param rgb
 * \return 
 */
unsigned int Processor::qrgbToGray(QRgb rgb)
{
    QColor color(rgb);
    return (color.red() + color.green() + color.blue()) / 3;
}
double Processor::getEnd() const
{
    return end;
}

void Processor::setEnd(double value)
{
    end = value;
}

double Processor::getStart() const
{
    return start;
}

void Processor::setStart(double value)
{
    start = value;
}

/*!
 * \brief Processor::Processor
 * \param parent
 */
Processor::Processor(QObject* parent)
{
    Q_UNUSED(parent)
    setAutoDelete(false);
}

/*!
 * \brief Processor::run
 */
void Processor::run()
{
    QVector<int> res;
    QVector<double> resm, t;
    VideoCapture capture(filename.toStdString().c_str());
    if (!capture.isOpened()) {
        QMessageBox::warning(0, "Error", "Capture failed (file not found?)\n");
        return;
    }
    Mat frame;
    res.clear();
    resm.clear();
    int frameNumbers = capture.get(CV_CAP_PROP_FRAME_COUNT);
    int fps = capture.get(CV_CAP_PROP_FPS); 
    int i = 0;
    if (start == -1) {
        start = 0;
    }
    if (end == -1) {
        end = frameNumbers / (double)fps;
    }
    for (int j = 0; j < int(start * fps); ++j, ++i) {
        if(!capture.grab()){
            QMessageBox::warning(0, "Error", "Grab failed\n");
            return;
        }
    }
    for (int k = int(start * fps); k < int(end * fps); k++) {
        capture >> frame;
        if (stop) {
            stop = false;
            break;
        }
        Mat imgmat;
        cvtColor(frame, imgmat, CV_BGR2GRAY, 1);
        cv::flip(imgmat,imgmat,0);
        QPair<int, double> pr = processImageCVMat(imgmat);
        res.push_back(pr.first);
        resm.push_back(pr.second);
        i++;
        int l = i - int(start * fps);
        if (!(l % (int((end - start) * fps) / 100))) {
            emit progress(l / (int((end - start) * fps) / 100));
        }
        emit time(i / (double)fps);
        t.push_back(i / (double)fps);
        QThread::currentThread()->usleep(500);
    }
    emit progress(100);
    capture.release();
    emit graphL(res, t);
    emit graphM(resm, t);
}

/*!
         * \brief Processor::processImage
         * \param _image
         * \return 
         */
QPair<int, double> Processor::processImage(QImage _image)
{
    int counter = 0;
    double sum = 0.0;
    QImage image = _image;
    if (!_image.isNull()) {
        int histogram[256] = { 0 };
        unsigned int minx = rect.right(), miny = rect.bottom(), maxx = rect.left(), maxy = rect.left();
        unsigned int x1 = rect.left(), x2 = rect.right(), y1 = rect.top(), y2 = rect.bottom();
        for (unsigned i = x1 + 1; i != x2; x1 < x2 ? ++i : --i) {
            for (unsigned j = y1 + 1; j != y2; y1 < y2 ? ++j : --j) {
                if (qrgbToGray(_image.pixel(i, j)) >= threshold) {
                    histogram[qrgbToGray(_image.pixel(i, j))]++;
                    counter++;
                }
                if (qrgbToGray(_image.pixel(i, j)) < threshold && (qrgbToGray(_image.pixel(i + 1, j)) >= threshold || qrgbToGray(_image.pixel(i, j + 1)) >= threshold || qrgbToGray(_image.pixel(i - 1, j)) >= threshold || qrgbToGray(_image.pixel(i, j - 1)) >= threshold)) {
                    image.setPixel(i, j, 0x00ff00);
                    if (minx > i) {
                        minx = i;
                    }
                    if (miny < j) {
                        miny = j;
                    }
                    if (maxx < i) {
                        maxx = i;
                    }
                    if (maxy < j) {
                        maxy = j;
                    }
                }
            }
        }
        
        for (unsigned i = x1; i != (x1 < x2 ? x2 + 1 : x2 - 1); x1 < x2 ? ++i : --i) {
            if (qrgbToGray(_image.pixel(i, y1)) >= threshold) {
                image.setPixel(i, y1, 0x00ff00);
            }
            if (qrgbToGray(_image.pixel(i, y2)) >= threshold) {
                image.setPixel(i, y2, 0x00ff00);
            }
        }
        for (unsigned i = y1; i != (y1 < y2 ? y2 + 1 : y2 - 1); y1 < y2 ? ++i : --i) {
            if (qrgbToGray(_image.pixel(x1, i)) >= threshold) {
                image.setPixel(x1, i, 0x00ff00);
            }
            if (qrgbToGray(_image.pixel(x2, i)) >= threshold) {
                image.setPixel(x2, i, 0x00ff00);
            }
        }
        for (int i = 0; i < 256; ++i) {
            sum += histogram[i] * i;
        }
        QRect r(minx, miny, maxx, maxy);
        emit maxMinBounds(r);
        emit frameChanged(image);
    }
    sum = (counter != 0 ? sum / counter : 0);
    QPair<int, double> res(counter, sum);
    return res;
}

QPair<int, double> Processor::processImageCV(QImage _image)
{
    Mat m = QImage2Mat(_image);
    return processImageCVMat(m);
}


QPair<int, double> Processor::processImageCVMat(cv::Mat& m){
    Mat mm;
    m.copyTo(mm);
    cv::threshold(m, m, threshold-1, 255, THRESH_BINARY);
    rect = rect.normalized();
    Rect r(rect.left(),  rect.top(), rect.width(),rect.height());
    m = m(r);
    std::vector<std::vector<cv::Point>> arr;
    findContours(m, arr, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    double sum = 0, mean0 = 0;
    for(auto a: arr){
        sum += contourArea(a);
        
    }
    for (unsigned i = 0; i < arr.size(); i++){
        for(cv::Point& a: arr[i]){
            a.x += r.x;
            a.y +=r.y;
        }
        mean0 += mean(mm,arr[i]);
        drawContours(mm, arr, i, Scalar(0,255,0));
    }
    QPair<int, double> res(sum, mean0);
    emit frameChanged(Mat2QImage(mm));
    return res;
}

/*!
         * \brief Processor::IplImage2QImage
         * \param iplImage
         * \return 
         */
QImage Processor::IplImage2QImage(const IplImage* iplImage)
{
    int height = iplImage->height;
    int width = iplImage->width;
    
    const uchar* qImageBuffer = (const uchar*)iplImage->imageData;
    QImage img(qImageBuffer, width, height, QImage::Format_RGB888);
    return img.rgbSwapped();
}

/*!
         * \brief Processor::stopThis
         */
void Processor::stopThis()
{
    stop = true;
}

QImage Processor::Mat2QImage(cv::Mat const& src)
{
    cv::Mat temp;
    if(src.channels() == 1){
        cvtColor(src, temp,CV_GRAY2RGB);
    }else if(src.channels() == 3){
        cvtColor(src, temp,CV_BGR2RGB);
    }
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits();
    return dest;
}

cv::Mat Processor::QImage2Mat(QImage const& src)
{
    cv::Mat tmp;
    if(src.format() == QImage::Format_RGB888){
        cv::Mat tmp1(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
        tmp1.copyTo(tmp);
    }else{
        cv::Mat tmp1(src.height(),src.width(),CV_8UC4,(uchar*)src.bits(),src.bytesPerLine());
        tmp1.copyTo(tmp);
    }
    cv::Mat result(src.height(),src.width(),CV_8UC1);
    cvtColor(tmp, result, CV_BGR2GRAY, 1);
    return result;
}

double Processor::mean(cv::Mat image, std::vector<cv::Point> contour){
    typedef cv::vector<cv::Point> TContour;
    
    // The conversion to cv::vector<cv::vector<cv::Point>> is unavoidable,
    // but can easily be achieved with a temporary instance.
    cv::Mat imageWithContour(image.clone());
    typedef cv::vector<TContour> TContours;
    cv::drawContours(imageWithContour, TContours(1, contour), -1, cv::Scalar(255, 255, 255));
    
    // Get ROI image.
    cv::Rect roi(cv::boundingRect(contour));
    cv::Mat crop(image, roi);
    
    // Calculate ROI mean.
    cv::Mat mask(cv::Mat::zeros(crop.rows, crop.cols, CV_8UC1)); //the mask with the size of cropped image
    // The offset for drawContours has to be *minus* roi.tl();
    cv::drawContours(mask, TContours(1, contour), -1, cv::Scalar(255), CV_FILLED, CV_AA, cv::noArray(), 1, -roi.tl());
    
    auto mean(cv::mean(crop, mask));
    auto sum(cv::sum(mean));
    
    return sum[0];
}

