#include "imagearea.hpp"
#include "ui_imagearea.h"

#include <QMessageBox>

/*!
 * \brief ImageArea::ImageArea
 * \param parent
 */
ImageArea::ImageArea(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ImageArea)
    , rectdrawing(true)
{
    setX1(0);
    setX2(image.width() - 1);
    setY1(0);
    setX2(image.height() - 1);
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
 * \brief ImageArea::rectRecv
 * \param rect
 */
void ImageArea::boundsChanged(QRect _bounds)
{
    bounds = _bounds;
    update();
}

/*!
 * \brief ImageArea::paintEvent
 * \param e
 */
void ImageArea::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setPen(Qt::green);
  
    if (!rectdrawing) {
        if(!image.isNull()){
            painter.drawImage(0, 0, image);
            painter.drawRect(bounds);
        }
    } else {
        if(!tmpimage.isNull()){
            painter.drawImage(0, 0, tmpimage);
            
            painter.setPen(Qt::red);
            
            painter.drawLine(x1() - 4, y1(), x1() + 4, y1());
            painter.drawLine(x1(), y1() - 4, x1(), y1() + 4);
            
            painter.drawLine(x2() - 4, y2(), x2() + 4, y2());
            painter.drawLine(x2(), y2() - 4, x2(), y2() + 4);
            
            painter.drawLine(x2() - 4, y1(), x2() + 4, y1());
            painter.drawLine(x2(), y1() - 4, x2(), y1() + 4);
            
            painter.drawLine(x1() - 4, y2(), x1() + 4, y2());
            painter.drawLine(x1(), y2() - 4, x1(), y2() + 4);
        }
    }
    
    e->accept();
}

/*!
 * \brief ImageArea::mousePressEvent
 * \param e
 */
void ImageArea::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        rectdrawing = false;
        setX1(e->x());
        setY1(e->y());
        if (e->x() < 0)
            setX1(0);
        if (e->y() < 0)
            setY1(0);
        if (e->x() >= image.width())
            setX1(image.width() - 1);
        if (e->y() >= image.height())
            setY1(image.height() - 1);
    }
}

/*!
 * \brief ImageArea::mouseReleaseEvent
 * \param e
 */
void ImageArea::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        setX2(e->x());
        setY2(e->y());
        if (e->x() < 0)
            setX2(0);
        if (e->y() < 0)
            setY2(0);
        if (e->x() >= image.width())
            setX2(image.width() - 1);
        if (e->y() >= image.height())
            setY2(image.height() - 1);
        rectdrawing = true;
        emit rectChanged(bounds);
        update();
    }
}

/*!
 * \brief ImageArea::mouseMoveEvent
 * \param e
 */
void ImageArea::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton) {
        setX2(e->x());
        setY2(e->y());
        if (e->x() < 0)
            setX2(0);
        if (e->y() < 0)
            setY2(0);
        if (e->x() > image.width())
            setX2(image.width());
        if (e->y() > image.height())
            setY2(image.height());
        update();
    } else {
    }
}

/*!
 * \brief ImageArea::open
 * \param filename
 */
void ImageArea::open(QString filename)
{
    image.load(filename);
    tmpimage = image;
    update();
}

/*!
 * \brief ImageArea::loadImage
 * \param _image
 */
void ImageArea::loadImage(QImage _image)
{
    image = _image;
    tmpimage = _image;
    update();
}

/*!
 * \brief ImageArea::readConfig
 * \param confname
 */
void ImageArea::readConfig(QString confname)
{
    QFile file(confname);
    if (file.open(QFile::ReadOnly)) {
        QTextStream str(&file);
        int x1;
        int y1;
        int x2;
        int y2;
        str >> x1 >> y1 >> x2 >> y2;
        setX1(x1);
        setY1(y1);
        setX2(x2);
        setY2(y2);
    }
}

/*!
 * \brief ImageArea::frameChanged
 * \param _image
 */
void ImageArea::frameChanged(QImage _image)
{
    qDebug() << "I'm in ImageArea::frameChanged";
    tmpimage = _image;
    rectdrawing = true;
    repaint();
}
