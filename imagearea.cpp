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
    //QVector<Point> points;
    quint32 minx = 0, miny = 0, maxx = 0, maxy = 0;
    if(!this->image.isNull()){
        QImage image = this->image;
        if(!rect) {
            painter.drawImage(0, 0, image);
            painter.drawRect(x1, y1, x2 - x1, y2 - y1);
            return;
        }else{
            counter = 0;
            if(!vid){
                minx = x2, miny = y2, maxx = x1, maxy = y1;
                for (unsigned  i = x1 + 1; i != x2; x1 < x2 ? ++i : --i ) {
                    for(unsigned j = y1 + 1; j != y2; y1 < y2 ? ++j : --j) {
                        if(this->image.pixel(i, j) >= tre()) {
                            im.histogram[qGray(image.pixel(i, j))] += 1;
                            im.square++;
                            counter++;
                        }
                        if(this->image.pixel(i,j) < tre() && (this->image.pixel(i + 1, j) >= tre() ||
                                                              this->image.pixel(i, j + 1) >= tre() ||
                                                              this->image.pixel(i - 1, j) >= tre() ||
                                                              this->image.pixel(i, j - 1) >= tre() )) {
                            //  points.push_back({i,j,0,0});
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
                //painter.drawText(30,30,QString::number(scan(points)));

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
            }
            painter.drawImage(0, 0, image);
            if(!vid){
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
            }
            painter.setPen(Qt::red);
            
            painter.drawLine(x1-4,y1,x1+4,y1);
            painter.drawLine(x1,y1-4,x1,y1+4);
            
            painter.drawLine(x2-4,y2,x2+4,y2);
            painter.drawLine(x2,y2-4,x2,y2+4);
            
            painter.drawLine(x2-4,y1,x2+4,y1);
            painter.drawLine(x2,y1-4,x2,y1+4);
            
            painter.drawLine(x1-4,y2,x1+4,y2);
            painter.drawLine(x1,y2-4,x1,y2+4);

            vid = false;
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
            update();
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
        update();
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
        update();
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
    update();
}

void ImageArea::set3D()
{
    d3 = !d3;
    QFuture<QVector<Point>> f = QtConcurrent::run(&converter, &Converter::convert, image, x1, y1, x2, y2);
    points = f.result();//converter.convert(image,x1,y1,x2,y2);
    update();
}

void ImageArea::setThreshold(int v)
{
    threshold = v;
    update();
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
    //QFuture<int> fn = QtConcurrent::run(&converter, &Converter::processVideo, fileNameV);
    //frame_num = fn.result();
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
        update();
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
        update();
        res.push_back(counter);
        resm.push_back(mean);
    }
    cvReleaseCapture(&capture);
    emit graph(res);
    emit graph(resm);
}

int ImageArea::scan(QVector<Point> &v)
{
    const int n = v.size();
    int L_min = 30;
    int kol = 5;
    //std::fstream f("dbscan.cfg");
    //f >> L_min >> kol;
    QVector<QVector<int>> L;
    QVector<int> rez;
    QVector<int> marked;
    QVector<int> group;
    rez.resize(n);
    L.resize(n);
    marked.resize(n);
    group.resize(n);
    for(QVector<int> &x : L ){
        x.resize(n);
    }
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            L[i][j] = sqrt((v[i].x-v[j].x)*(v[i].x-v[j].x)+(v[i].y-v[j].y)*(v[i].y-v[j].y)) <= L_min;
            if(L[i][j]){
                rez[i]++;
            }
        }
        for(int j = 0; j < n; j++){
            L[i][j] *= j;
        }
    }
    int c = 1;
    for(int i = 0; i < n; ++i){
        if(marked[i] == 0){
            if(rez[i] < kol){
                marked[i] = 1;
                group[i] = -1;
            }else{
                auto uvec = group;
                group[i] = c;
                while(!std::equal(uvec.begin(),uvec.end(),group.begin())){
                    uvec = group;
                    for(int j = 0; j < n; ++j){
                        if(group[j] == c && marked[j] == 0){
                            if(rez[i] < kol){
                                marked[j] = 1;
                                group[j] = -1;
                            }else{
                                marked[j] = 1;
                                for(int k = 0; k < n; ++k){
                                    if(L[j][k] > 0){
                                        group[k] = c;
                                    }
                                }
                            }
                        }
                    }
                }
                c++;
            }
        }else{
            continue;
        }
    }
    return c;
}

void ImageArea::frameChanged(QImage _image)
{
    image = _image;
    vid = true;
    rect = true;
    update();
}
