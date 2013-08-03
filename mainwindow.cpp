#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), image("4.bmp"), pix(image),
    rect(false), d3(false), x1(0), y1(0), x2(0), y2(0), threshold(255)
{
    ui->setupUi(this);
    //if(d3){
        convert();
        pix = QImage(1366,768,image.format());
        origin.x = 0;
        origin.y = 0;
        origin.z = 0.0;
        rotate(3.1415/180*35.2,0,0);
        rotate(0,3.1415/4,0);
        rotate(0,0,-3.1415/4);
    //}
    // rotate(0,0,3.1415/4);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::green);
    QImage image = this->image;
    quint32 counter = 0 ;
    if(rect){
        painter.drawImage(0,0,image);
        for (unsigned  i = x1; i != x2; x1 < x2 ? ++i : --i ) {
            for(unsigned j = y1; j != y2; y1 < y2 ? ++j : --j){
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
        painter.drawImage(0,0,image);
    }else{
        for (int i = 0; i < image.width(); ++i) {
            for(int j = 0; j < image.height(); ++j){
                if(image.pixel(i,j) >= qRgb(255,255,255)){
                    counter++;
                }
            }
        }
    }
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
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton){
        x2 = e->x();
        y2 = e->y();
        rect = true;
        repaint();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    x2 = e->x();
    y2 = e->y();
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
