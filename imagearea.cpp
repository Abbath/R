#include "imagearea.hpp"
#include "ui_imagearea.h"

ImageArea::ImageArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageArea),
    rect(true), d3(false), x1(0), y1(0), x2(image.width()-1), y2(image.height()-1), cc(0), threshold(255)
{
    ui->setupUi(this);
    converter.convert(image);
    pix = QImage(1366, 768, image.format());
    QFile file("bounds.conf");
    file.open(QFile::ReadOnly);
    QTextStream str(&file);
    str >> x1 >> y1 >> x2 >> y2;
    vid = false;
    repaint();
    //  grabMouse();
}

ImageArea::~ImageArea()
{
    delete ui;
}

void ImageArea::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setPen(Qt::green);
    if(!this->image.isNull()){
        QImage image = this->image;
        if(!rect) {
            painter.drawImage(0, 0, image);
            painter.drawRect(x1, y1, x2 - x1, y2 - y1);
            return;
        }else{
            counter = 0;
            quint32 minx = x2, miny = y2, maxx = x1, maxy = y1;
            for (unsigned  i = x1 + 1; i != x2; x1 < x2 ? ++i : --i ) {
                for(unsigned j = y1 + 1; j != y2; y1 < y2 ? ++j : --j) {
                    if(this->image.pixel(i, j) >= tre()) {
                        im.histogram[qGray(image.pixel(i, j))] += 1;
                        im.square++;
                        counter++;
                    }
                    /* if((image.pixel(i,j) - image.pixel(i,j+1) < 40) ||
                    (image.pixel(i,j) - image.pixel(i,j-1) < 40) ||
                    (image.pixel(i,j) - image.pixel(i+1,j) < 40) ||
                    (image.pixel(i,j) - image.pixel(i-1,j) < 40) ||
                    (image.pixel(i,j) - image.pixel(i+1,j+1) < 40) ||
                    (image.pixel(i,j) - image.pixel(i-1,j-1) < 40) ||
                    (image.pixel(i,j) - image.pixel(i+1,j-1) < 40) ||
                    (image.pixel(i,j) - image.pixel(i-1,j+1) < 40) ){
                painter.setPen(Qt::blue);
                painter.drawPoint(i, j);
                painter.setPen(Qt::green);
            }*/
                    if(this->image.pixel(i,j) < tre() && (this->image.pixel(i + 1, j) >= tre() ||
                                                    this->image.pixel(i, j + 1) >= tre() ||
                                                    this->image.pixel(i - 1, j) >= tre() ||
                                                    this->image.pixel(i, j - 1) >= tre() )) {
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
            im.x1 = minx;
            im.y1 = miny;
            im.x2 = maxx;
            im.y2 = maxy;
            for(unsigned i = x1; i != (x1 < x2 ? x2 + 1 : x2 - 1); x1 < x2 ? ++i : --i) {
                if( this->image.pixel(i, y1) >= tre() ) {
                    image.setPixel(i, y1, 0x00ff00);
                }
                if( this->image.pixel(i, y2) >= tre() ) {
                    image.setPixel(i, y2, 0x00ff00);
                }
            }
            for(unsigned i = y1; i != (y1 < y2 ? y2 + 1 : y2 - 1); y1 < y2 ? ++i : --i) {
                if( this->image.pixel(x1, i) >= tre()) {
                    image.setPixel(x1, i, 0x00ff00);
                }
                if( this->image.pixel(x2, i) >= tre()) {
                    image.setPixel(x2, i, 0x00ff00);
                }
            }
            //            if(d3) {
            //                painter.setBrush(Qt::black);
            //                painter.drawRect(0, 0, this->width(), this->height());
            //                std::sort(points.begin(), points.end(), [](Point a, Point b) { return a.z < b.z; });
            //                for ( Point p : points) {
            //                    pix.setPixel(p.x , p.y + 400, (p.c >= threshold ? qRgb(0, 255, 0) : qRgb(p.c, p.c, p.c)));
            //                }
            //                painter.drawImage(0, 0, pix);
            //                QLinearGradient fade(30, 90, 10, 500);
            //                fade.setColorAt(0.0, QColor(255, 255, 255, 255));
            //                fade.setColorAt(1.0, QColor(0, 0, 0, 255));
            //                painter.fillRect(30, 90, 10, 500, fade);
            //                painter.drawText(30, 70, QString::number(cc));
            //            }else{
            painter.drawImage(0, 0, image);
            /*for(auto i = 0; i < 256; ++i){
                painter.drawLine(800 + i, 700, 800 + i,700 - (1.0 * im.histogram[i] / im.square * 600));
            }*/
            //            }
            //painter.drawText(30, 30, QString::number(!rect ? abs(x1 - x2) * abs(y1 - y2) : 0));
            /*painter.drawText(30, 30, QString::number(!rect ? abs(x1 - x2) * abs(y1 - y2) : counter)+
                         QString(" x1: ")+QString::number(x1)+
                         QString(" y1: ")+QString::number(y1)+
                         QString(" x2:  ")+QString::number(x2)+
                         QString(" y2: ")+QString::number(y2)+
                         QString(" threshold: ")+QString::number(threshold));
        painter.drawText(30, 50, QString(" minx: ")+QString::number(minx)+
                         QString(" miny: ")+QString::number(miny)+
                         QString(" maxx:  ")+QString::number(maxx)+
                         QString(" maxy: ")+QString::number(maxy));*/
            qreal sum = 0.0;
            for( int i = 0; i < 256 ; ++i){
                sum += im.histogram[i]*i;
            }
            
            Display dis;
            dis.maxx = maxx;
            dis.minx = minx;
            dis.miny = miny;
            dis.maxy = maxy;
            dis.mean = mean = sum/counter;
            dis.sum = counter;
            emit displayChanged(dis);
            im.flush();
            painter.setPen(Qt::red);
            
            painter.drawLine(x1-4,y1,x1+4,y1);
            painter.drawLine(x1,y1-4,x1,y1+4);
            
            painter.drawLine(x2-4,y2,x2+4,y2);
            painter.drawLine(x2,y2-4,x2,y2+4);
            
            painter.drawLine(x2-4,y1,x2+4,y1);
            painter.drawLine(x2,y1-4,x2,y1+4);
            
            painter.drawLine(x1-4,y2,x1+4,y2);
            painter.drawLine(x1,y2-4,x1,y2+4);
        }  
    }
    e->accept();
}

void ImageArea::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && !d3){
        rect = false;
        x1 = e->x();
        y1 = e->y();
        if(e->x() < 0) x1 = 0;
        if(e->y() < 0) y1 = 0;
        if(e->x() >= image.width()) x1 = image.width()-1;
        if(e->y() >= image.height()) y1 = image.height()-1;
    }else{
        if(e->button() == Qt::MidButton){
            cc = qGray(pix.pixel(e->x(), e->y()));
            repaint();
        }
    }
}

void ImageArea::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && !d3){
        x2 = e->x();
        y2 = e->y();
        if(e->x() < 0) x2 = 0;
        if(e->y() < 0) y2 = 0;
        if(e->x() >= image.width()) x2 = image.width() - 1;
        if(e->y() >= image.height()) y2 = image.height() - 1;
        rect = true;
        emit rectChanged(QRect(x1,y1,x2-x1,y2-y1));
        repaint();
    }
}

void ImageArea::mouseMoveEvent(QMouseEvent *e)
{
    if(!d3 && e->buttons() & Qt::LeftButton){
        x2 = e->x();
        y2 = e->y();
        if(e->x() < 0) x2 = 0;
        if(e->y() < 0) y2 = 0;
        if(e->x() > image.width()) x2 = image.width();
        if(e->y() > image.height()) y2 = image.height();
        repaint();
    }else{
        
    }
}

void ImageArea::wheelEvent(QWheelEvent *e)
{
    //qint32 a = threshold + e->delta()/40;
    //if( a > 255 ) a = 255;
    //if( a < 0 ) a = 0;
    //threshold = quint8(a);
    //repaint();
    e->accept();
}

void ImageArea::open(QString filename)
{
    fileNameV.clear();
    image.load(filename);
    repaint();
}

void ImageArea::set3D()
{
    d3 = !d3;
    QFuture<QVector<Point>> f = QtConcurrent::run(&converter, &Converter::convert, image, x1, y1, x2, y2);
    points = f.result();//converter.convert(image,x1,y1,x2,y2);
    repaint();
}

void ImageArea::setThreshold(int v)
{
    threshold = v;
    repaint();
}

void ImageArea::saveBounds()
{
    QFile file("bounds.conf");
    file.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream str(&file);
    str << x1 << "\n" << y1 << "\n" << x2 << "\n" << y2;
}

void ImageArea::saveResults()
{
    QString name = QFileDialog::getSaveFileName(this, "Save data", "", "Data (*.dat)");
    QFile file(name);
    if(file.open(QFile::WriteOnly)){
        QTextStream str(&file);
        for(int i = 0 ; i < res.size(); ++i){
            str << i << " " << res[i] << " " << resm[i] << '\n';
        }
    }
}

int ImageArea::openVideo(){
    fileNameV = QFileDialog::getOpenFileName( this, tr("Open data file"), "", tr("Video files (*.avi)"));
    //this->setCursor(Qt::WaitCursor);
    QFuture<int> fn = QtConcurrent::run(&converter, &Converter::processVideo, fileNameV);
    frame_num = fn.result();
    //this->setCursor(Qt::ArrowCursor);
    return frame_num;
}

void ImageArea::getFrame(int n)
{
    if(!fileNameV.isEmpty()){
        CvCapture * capture = cvCaptureFromAVI(fileNameV.toStdString().c_str());
        IplImage * frame;
        for(int i = 0; i < n; ++i){
            if(cvQueryFrame(capture) == NULL){
                return;
            }
        }
        frame = cvQueryFrame(capture);
        image = converter.IplImage2QImage(frame).mirrored(false, true);
        cvReleaseCapture(&capture);
        repaint();
    }
}

void ImageArea::run(){
    CvCapture * capture = cvCaptureFromAVI(fileNameV.toStdString().c_str());
    if(!capture)
    {
        QMessageBox::warning(0, "Error", "cvCaptureFromAVI failed (file not found?)\n");
        return;
    }
    /*QWidget * w = new QWidget();
    w->setLayout(new QGridLayout);
    QProgressBar * b = new QProgressBar();
    w->layout()->addWidget(b);
    w->show();*/
    //int fps = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    // qDebug() << "* FPS: %d" <<  fps << "\n";
    IplImage* frame = NULL;
    //int frame_number = 0;
    res.clear();
    while ((frame = cvQueryFrame(capture))) {
        /*char filename[100];
        strcpy(filename, "frame_");
        char frame_id[30];
        sprintf(frame_id,"%d",frame_number);
        strcat(filename, frame_id);
        strcat(filename, ".bmp");
        //QMatrix matrix;
        //matrix.rotate(180);
        QImage img = IplImage2QImage(frame).mirrored(false, true);
        img.save(QString(filename), "BMP", 100);
        frame_number++;
        b->setValue((b->value()+1)%100);*/
        image = converter.IplImage2QImage(frame).mirrored(false, true);
        repaint();
        res.push_back(counter);
        resm.push_back(mean);
    }
    cvReleaseCapture(&capture);
    emit graph(res);
    emit graph(resm);
}
