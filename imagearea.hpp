#ifndef IMAGEAREA_HPP
#define IMAGEAREA_HPP

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <helpers.hpp>
#include <converter.hpp>
#include <QtConcurrent/QtConcurrent>
#include <QFileDialog>



namespace Ui {
class ImageArea;
}

class ImageArea : public QWidget
{
    Q_OBJECT

public:
    explicit ImageArea(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void open(QString filename);
    int openVideo();

    void set3D();
    void setThreshold(int v);
    void run();
    QSize getSize(){ return image.size(); }
    void saveBounds();
    void saveResults();
    void getFrame(int n);
    int getThreshold(){return (int)threshold;}
    QImage getImage() { return image;}
    QRect getRect(){return QRect(x1<x2 ? x1 : x2, y1<y2 ? y1 : y2,abs(x1-x2), abs(y1-y2));}
    ~ImageArea();

private:
    QVector<int> res;
    QVector<double> resm;
    QRgb tre() const { return qRgb(threshold, threshold, threshold); }
    int scan(QVector<Point>& v);
    Ui::ImageArea *ui;
    QImage image, pix;
    QVector<Point> points;
    bool rect, d3, vid = false;

    quint32 x1=0, y1=0, x2=0, y2=0, cc;
    quint8 threshold;

    Image im;
    Converter converter;
    QString fileNameV;
    QVector<QString> fileNames;
    int frame_num, counter;
    qreal mean;
public:
    void setX1(quint32 x){ x1 = x; repaint();}
    void setY1(quint32 y){ y1 = y; repaint();}
    void setX2(quint32 x){ x2 = x; repaint();}
    void setY2(quint32 y){ y2 = y; repaint();}
public slots:
    void frameChanged(QImage _image);
signals:
    void rectChanged(QRect r);
    void graph(const QVector<int> &v);
    void graph(const QVector<double> &v);
    void displayChanged(Display dis);
};

#endif // IMAGEAREA_HPP
