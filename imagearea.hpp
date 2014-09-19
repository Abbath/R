#ifndef IMAGEAREA_HPP
#define IMAGEAREA_HPP

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>

namespace Ui {
class ImageArea;
}
/*!
 * \brief The ImageArea class
 */
class ImageArea : public QWidget
{
    Q_OBJECT

public:
    explicit ImageArea(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void open(QString filename);
    void loadImage(QImage _image);
    void readConfig(QString confname);

    QImage getImage() { if(image.isNull()) return tmpimage; else return image;}
    QRect getBounds(){ return bounds;}
    void setBounds(QRect _bounds){ bounds = _bounds; }
 
    int x1(){ return bounds.left(); }
    int x2(){ return bounds.right(); }
    int y1(){ return bounds.top(); }
    int y2(){ return bounds.bottom(); }
    
    void setX1(int _x1){ bounds.setLeft(_x1); }
    void setX2(int _x2){ bounds.setRight(_x2); }
    void setY1(int _y1){ bounds.setTop(_y1); }
    void setY2(int _y2){ bounds.setBottom(_y2); }
    
    ~ImageArea();

private:
    Ui::ImageArea *ui;
    QImage image, tmpimage;
    bool rectdrawing;
    QRect bounds;

public slots:
    void boundsChanged(QRect _bounds);
    void frameChanged(QImage _image);

signals:
    void rectChanged(QRect r);
    void graph(const QVector<int> &v);
    void graph(const QVector<double> &v);
};

#endif // IMAGEAREA_HPP
