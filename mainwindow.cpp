#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), image("4.bmp"), pix(image),
    rect(false), d3(false), x1(0), y1(0), x2(image.width()-1), y2(image.height()-1), cc(0), threshold(255)
{
    ui->setupUi(this);
    convert();
    pix = QImage(1366, 768, image.format());
    grabMouse();
}

void MainWindow::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setPen(Qt::green);
    QImage image = this->image;
    if(!rect) {
        painter.drawImage(0, 0, image);
        painter.drawRect(x1, y1, x2 - x1, y2 - y1);
        return;
    }
    quint32 counter = 0, minx = x1, miny = y1, maxx = x2, maxy = y2;
    for (unsigned  i = x1 + 1; i != x2; x1 < x2 ? ++i : --i ) {
        for(unsigned j = y1 + 1; j != y2; y1 < y2 ? ++j : --j) {
            if(image.pixel(i, j) >= tre()) {
                counter++;
            }
//            if(image.pixel(i,j) - image.pixel(i,j+1) < 40) image.setPixel(i,j,qRgb(0,0,255));
//            if(image.pixel(i,j) - image.pixel(i,j-1) < 40) image.setPixel(i,j,qRgb(0,0,255));
//            if(image.pixel(i,j) - image.pixel(i+1,j) < 40) image.setPixel(i,j,qRgb(0,0,255));
//            if(image.pixel(i,j) - image.pixel(i-1,j) < 40) image.setPixel(i,j,qRgb(0,0,255));
            if(image.pixel(i,j) < tre() && (image.pixel(i + 1, j) >= tre() ||
                                            image.pixel(i, j + 1) >= tre() ||
                                            image.pixel(i - 1, j) >= tre() ||
                                            image.pixel(i, j - 1) >= tre() )) {
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
        if( image.pixel(i, y1) >= tre() ) {
            image.setPixel(i, y1, 0x00ff00);
        }
        if( image.pixel(i, y2) >= tre() ) {
            image.setPixel(i, y2, 0x00ff00);
        }
    }
    for(unsigned i = y1; i != (y1 < y2 ? y2 + 1 : y2 - 1); y1 < y2 ? ++i : --i) {
        if( image.pixel(x1, i) >= tre()) {
            image.setPixel(x1, i, 0x00ff00);
        }
        if( image.pixel(x2, i) >= tre()) {
            image.setPixel(x2, i, 0x00ff00);
        }
    }
    if(d3) {
        painter.setBrush(Qt::black);
        painter.drawRect(0, 0, this->width(), this->height());
        std::sort(points.begin(), points.end(), [](Point a, Point b) { return a.z < b.z; });
        for ( Point p : points) {
            pix.setPixel(p.x + 300, p.y + 500, (p.c >= threshold ? qRgb(0, 255, 0) : qRgb(p.c, p.c, p.c)));
        }
        painter.drawImage(0, 0, pix);
        QLinearGradient fade(30, 90, 10, 500);
        fade.setColorAt(0.0, QColor(255, 255, 255, 255));
        fade.setColorAt(1.0, QColor(0, 0, 0, 255));
        painter.fillRect(30, 90, 10, 500, fade);
        painter.drawText(30,70,QString::number(cc));
    }else{
        painter.drawImage(0, 0, image);
        if(!rect) {
            painter.drawRect(x1, y1, x2 - x1, y2 - y1);
        }

    }
    painter.drawText(30, 30, QString::number(!rect ? abs(x1 - x2) * abs(y1 - y2) : counter)+
                     QString(" x1: ")+QString::number(x1)+
                     QString(" y1: ")+QString::number(y1)+
                     QString(" x2:  ")+QString::number(x2)+
                     QString(" y2: ")+QString::number(y2)+
                     QString(" threshold: ")+QString::number(threshold));
    painter.drawText(30, 50, QString(" minx: ")+QString::number(minx)+
                     QString(" miny: ")+QString::number(miny)+
                     QString(" maxx:  ")+QString::number(maxx)+
                     QString(" maxy: ")+QString::number(maxy));
    e->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton && !d3){
        rect = false;
        x1 = e->x();
        y1 = e->y();
        if(e->x() < 0) x1 = 0;
        if(e->y() < 0) y1 = 0;
        if(e->x() > image.width()) x1 = image.width();
        if(e->y() > image.height()) y1 = image.height();
    }else{
        if(e->button() == Qt::MidButton){
            cc = qGray(pix.pixel(e->x(),e->y()));
            repaint();
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton && !d3){
        x2 = e->x();
        y2 = e->y();
        if(e->x() < 0) x2 = 0;
        if(e->y() < 0) y2 = 0;
        if(e->x() > image.width()) x2 = image.width();
        if(e->y() > image.height()) y2 = image.height();
        rect = true;
        repaint();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(!d3){
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

void MainWindow::wheelEvent(QWheelEvent *e)
{
    qint32 a = threshold + e->delta()/40;
    if( a > 255 ) a = 255;
    if( a < 0 ) a = 0;
    threshold = quint8(a);
    convert(x1,y1,x2,y2);
    repaint();
}

void MainWindow::keyPressEvent(QKeyEvent *e){
    if(e->key() == Qt::Key_D && e->modifiers() == Qt::ControlModifier){
        d3 = !d3;
        convert(x1,y1,x2,y2);
    }
    if(e->key() == Qt::Key_O && e->modifiers() == Qt::ControlModifier){
        const QString fileName = QFileDialog::getOpenFileName( this, tr("Open data file"), "", tr("Image files (*.bmp)"));
        image.load(fileName);
    }
    repaint();
}

MainWindow::~MainWindow()
{
    delete ui;
}
