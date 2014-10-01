#ifndef IMAGEAREA_HPP
#define IMAGEAREA_HPP

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "imagestorage.hpp"

namespace Ui {
class ImageArea;
}

/*!
 * \brief The ImageArea class
 */
class ImageArea : public QWidget
{
    Q_OBJECT
    typedef std::vector<cv::Point> Contour;
    typedef std::vector<Contour> Contours;
public:
    explicit ImageArea(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    QRect getBounds() const { return bounds;}
    void setBounds(const QRect& _bounds){ bounds = _bounds; }
    void clearContours(){ contours.clear(); }
 
    int x1() const { return bounds.left(); }
    int x2() const { return bounds.right(); }
    int y1() const { return bounds.top(); }
    int y2() const { return bounds.bottom(); }
    
    void setX1(int _x1){ bounds.setLeft(_x1); }
    void setX2(int _x2){ bounds.setRight(_x2); }
    void setY1(int _y1){ bounds.setTop(_y1); }
    void setY2(int _y2){ bounds.setBottom(_y2); }
    
    ~ImageArea();

private:
    Ui::ImageArea *ui;
    Contours contours;
    bool rectnotdrawing;
    QRect bounds;

public slots:
    void boundsChanged(QRect _bounds);
    void frameChanged(QImage image, Contours _contours);

signals:
    void rectChanged(QRect r);
    void graph(const QVector<int> &v);
    void graph(const QVector<double> &v);
};

#endif // IMAGEAREA_HPP
