#include "imagearea.hpp"
#include "ui_imagearea.h"

/*!
 * \brief ImageArea::ImageArea
 * \param parent
 */
ImageArea::ImageArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageArea),
    rect(true)
{
    setX1(0);
    setX2(image.width()-1);
    setY1(0);
    setX2(image.height()-1);
    ui->setupUi(this);
    update();
}

/*!
 * \brief ImageArea::~ImageArea
 */
ImageArea::~ImageArea()
{
    delete ui;
}

/*!
 * \brief ImageArea::paintEvent
 * \param e
 */
void ImageArea::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setPen(Qt::green);
    if(!image.isNull()){
        if(!rect) {
            painter.drawImage(0, 0, image);
            painter.drawRect(bounds);
            return;
        }else{
            
            painter.drawImage(0, 0, pix);

            painter.setPen(Qt::red);
            
            painter.drawLine(x1()-4,y1(),x1()+4,y1());
            painter.drawLine(x1(),y1()-4,x1(),y1()+4);
            
            painter.drawLine(x2()-4,y2(),x2()+4,y2());
            painter.drawLine(x2(),y2()-4,x2(),y2()+4);
            
            painter.drawLine(x2()-4,y1(),x2()+4,y1());
            painter.drawLine(x2(),y1()-4,x2(),y1()+4);
            
            painter.drawLine(x1()-4,y2(),x1()+4,y2());
            painter.drawLine(x1(),y2()-4,x1(),y2()+4);
            
        }
    }
    e->accept();
}

/*!
 * \brief ImageArea::mousePressEvent
 * \param e
 */
void ImageArea::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        rect = false;
        setX1(e->x());
        setY1(e->y());
        if(e->x() < 0) setX1(0);
        if(e->y() < 0) setY1(0);
        if(e->x() >= image.width()) setX1(image.width()-1);
        if(e->y() >= image.height()) setY1(image.height()-1);
    }
}

/*!
 * \brief ImageArea::mouseReleaseEvent
 * \param e
 */
void ImageArea::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        setX2(e->x());
        setY2(e->y());
        if(e->x() < 0) setX2(0);
        if(e->y() < 0) setY2(0);
        if(e->x() >= image.width()) setX2(image.width() - 1);
        if(e->y() >= image.height()) setY2(image.height() - 1);
        rect = true;
        emit rectChanged(bounds);
        update();
    }
}

/*!
 * \brief ImageArea::mouseMoveEvent
 * \param e
 */
void ImageArea::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton){
        setX2(e->x());
        setY2(e->y());
        if(e->x() < 0) setX2(0);
        if(e->y() < 0) setY2(0);
        if(e->x() > image.width()) setX2(image.width());
        if(e->y() > image.height()) setY2(image.height());
        update();
    }else{
        
    }
}

/*!
 * \brief ImageArea::open
 * \param filename
 */
void ImageArea::open(QString filename)
{
    image.load(filename);
    pix = image;
    update();
}

/*!
 * \brief ImageArea::loadImage
 * \param _image
 */
void ImageArea::loadImage(QImage _image)
{
    image = _image;
    pix = _image;
    update();
}

/*!
 * \brief ImageArea::readConfig
 * \param confname
 */
void ImageArea::readConfig(QString confname)
{
    QFile file(confname);
    file.open(QFile::ReadOnly);
    QTextStream str(&file);
    int x1;
    int y1;
    int x2;
    int y2;
    str>> x1 >> y1 >> x2 >> y2;
    setX1(x1);
    setY1(y1);
    setX2(x2);
    setY2(y2);
}

/*!
 * \brief ImageArea::saveBounds
 */
void ImageArea::saveBounds()
{
    QFile file("bounds.conf");
    file.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream str(&file);
    str << x1() << "\n" << y1() << "\n" << x2() << "\n" << y2();
}

/*!
 * \brief ImageArea::saveResults
 */
void ImageArea::saveResults()
{
    QString name = QFileDialog::getSaveFileName(this, "Save data", "", "Data (*.dat)");
    QFile file(name);
    if(file.open(QFile::WriteOnly)){
        QTextStream str(&file);
        for(int i = 0 ; i < res.size(); ++i){
            str << i << " " << res[i] << " " << resm[i] << '\n';
        }
    }else{
        qDebug() << "Can not open file for writing!";
    }
}

/*!
 * \brief ImageArea::frameChanged
 * \param _image
 */
void ImageArea::frameChanged(QImage _image)
{
    pix = _image;
    rect = true;
    update();
}

//int ImageArea::scan(QVector<Point> &v)
//{
//    const int n = v.size();
//    int L_min = 30;
//    int kol = 5;
//    //std::fstream f("dbscan.cfg");
//    //f >> L_min >> kol;
//    QVector<QVector<int>> L;
//    QVector<int> rez;
//    QVector<int> marked;
//    QVector<int> group;
//    rez.resize(n);
//    L.resize(n);
//    marked.resize(n);
//    group.resize(n);
//    for(QVector<int> &x : L ){
//        x.resize(n);
//    }
//    for(int i = 0; i < n; ++i){
//        for(int j = 0; j < n; ++j){
//            L[i][j] = sqrt((v[i].x()-v[j].x())*(v[i].x()-v[j].x())+(v[i].y()-v[j].y())*(v[i].y()-v[j].y())) <= L_min;
//            if(L[i][j]){
//                rez[i]++;
//            }
//        }
//        for(int j = 0; j < n; j++){
//            L[i][j] *= j;
//        }
//    }
//    int c = 1;
//    for(int i = 0; i < n; ++i){
//        if(marked[i] == 0){
//            if(rez[i] < kol){
//                marked[i] = 1;
//                group[i] = -1;
//            }else{
//                auto uvec = group;
//                group[i] = c;
//                while(!std::equal(uvec.begin(),uvec.end(),group.begin())){
//                    uvec = group;
//                    for(int j = 0; j < n; ++j){
//                        if(group[j] == c && marked[j] == 0){
//                            if(rez[i] < kol){
//                                marked[j] = 1;
//                                group[j] = -1;
//                            }else{
//                                marked[j] = 1;
//                                for(int k = 0; k < n; ++k){
//                                    if(L[j][k] > 0){
//                                        group[k] = c;
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//                c++;
//            }
//        }else{
//            continue;
//        }
//    }
//    return c;
//}


