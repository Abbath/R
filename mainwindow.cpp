#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), image("4.bmp"), pix(image),
    rect(false), d3(false), x1(0), y1(0), x2(image.width()), y2(image.height()), threshold(255)
{
    ui->setupUi(this);
        convert();
        pix = QImage(1366,768,image.format());

}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::green);
    QImage image = this->image;
    quint32 counter = 0 ;
        painter.drawImage(0,0,image);
        for (unsigned  i = x1+1; i != x2; x1 < x2 ? ++i : --i ) {
            for(unsigned j = y1+1; j != y2; y1 < y2 ? ++j : --j){
                if(image.pixel(i,j) >= tre()){
                    counter++;
                }
                if(image.pixel(i,j) < tre() && (image.pixel(i+1,j) >= tre() ||
                                                image.pixel(i,j+1) >= tre() ||
                                                image.pixel(i-1,j) >= tre() ||
                                                image.pixel(i,j-1) >= tre() )){
                    image.setPixel(i,j,0x00ff00);
                }
            }
        }
        for(unsigned i = x1; i != (x1 < x2 ? x2+1 : x2-1); x1 < x2 ? ++i : --i){
            if( image.pixel(i,y1) >= tre() ) image.setPixel(i,y1,0x00ff00);
            if( image.pixel(i,y2) >= tre() ) image.setPixel(i,y2,0x00ff00);
        }
        for(unsigned i = y1; i !=  (y1 < y2 ? y2+1 : y2-1); y1 < y2 ? ++i : --i){
            if( image.pixel(x1,i) >= tre() ) image.setPixel(x1,i,0x00ff00);
            if( image.pixel(x2,i) >= tre() ) image.setPixel(x2,i,0x00ff00);
        }
        painter.drawImage(0,0,image);
    if(d3){
        painter.setBrush(Qt::black);
        painter.drawRect(0,0,this->width(),this->height());
        std::sort(points.begin(), points.end(), [](Point a, Point b){ return a.z < b.z;});
        for ( Point p : points){
            //painter.setBrush(QBrush(QColor(qRgb(p.z,p.z,p.z))));
            //painter.setPen(QColor(qRgb(p.c,p.c,p.c)));
            pix.setPixel(p.x+300,p.y+500,qRgb(p.c,p.c,p.c));
            //painter.drawPoint(p.x+300,p.y+500);
        }

        //QImage pix = QPixmap::grabWidget(this).toImage();
        //        for( int i = 0; i < pix.width(); ++i){
        //            for(int j = 0; j < pix.height(); ++j){
        //                if(pix.pixel(i,j) == qRgb(0,0,0)){
        //                    pix.setPixel(i,j,pix.pixel(i,j > 0 ? j-1 : j));
        //                }
        //            }
        //        }
        painter.drawImage(0,0,pix);
        painter.setPen(Qt::black);
    }else{
        painter.drawImage(0,0,image);
        if(!rect)painter.drawRect(x1,y1,x2-x1,y2-y1);
    }
    painter.drawText(30,30,QString::number(counter)+QString(" x1: ")+QString::number(x1)+QString(" y1: ")+QString::number(y1)+QString(" x2:  ")+QString::number(x2)+QString(" y2: ")+QString::number(y2)+QString(" threshold: ")+QString::number(threshold));
    e->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton){
        rect = false;
        x1 = e->x();
        y1 = e->y();
        if(e->x() < 0) x1 = 0;
        if(e->y() < 0) y1 = 0;
        if(e->x() > image.width()) x1 = image.width();
        if(e->y() > image.height()) y1 = image.height();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton){
        x2 = e->x();
        y2 = e->y();
        if(e->x() < 0) x2 = 0;
        if(e->y() < 0) y2 = 0;
        if(e->x() > image.width()) x2 = image.width();
        if(e->y() > image.height()) y2 = image.height();
        rect = true;
        convert(x1,y1,x2,y2);
        repaint();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    x2 = e->x();
    y2 = e->y();
    if(e->x() < 0) x2 = 0;
    if(e->y() < 0) y2 = 0;
    if(e->x() > image.width()) x2 = image.width();
    if(e->y() > image.height()) y2 = image.height();
    repaint();
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    qint32 a = threshold + e->delta()/40;
    if( a > 255 ) a = 255;
    if( a < 0 ) a = 0;
    threshold = quint8(a);
    repaint();
}

void MainWindow::keyPressEvent(QKeyEvent *e){
    if(e->key() == Qt::Key_D && e->modifiers() == Qt::ControlModifier){
        d3 = !d3;
    }
    repaint();
}

MainWindow::~MainWindow()
{
    delete ui;
}
