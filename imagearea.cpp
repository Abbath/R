#include "imagearea.hpp"
#include "ui_imagearea.h"

#include <QMessageBox>
#include <imageconverter.hpp>

/*!
 * \brief ImageArea::ImageArea
 * \param parent
 */
ImageArea::ImageArea(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ImageArea)
    , rectnotdrawing(true)
{
    setX1(0);
    setX2(ImageStorage::getInstance().getImageWidth() - 1);
    setY1(0);
    setX2(ImageStorage::getInstance().getImageHeight() - 1);
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
    //assert(_bounds.isValid());
    bounds = _bounds.normalized();
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
    if(!ImageStorage::getInstance().isImageNull()){
        if (!rectnotdrawing) {
            painter.drawImage(0, 0, ImageStorage::getInstance().getImage());
            painter.drawRect(bounds);
        } else {
            painter.drawImage(0, 0, ImageStorage::getInstance().getImage());
            
            painter.setPen(Qt::green);
            
            for(auto contourIt = contours.cbegin(); contourIt != contours.cend(); ++contourIt){
                for(auto pointIt = contourIt->cbegin(); pointIt != (contourIt->cend() - 1); ++pointIt){
                    painter.drawLine(pointIt->x, pointIt->y, (pointIt + 1)->x, (pointIt + 1)->y);
                }
                painter.drawLine((contourIt->cend() - 1)->x,
                                 (contourIt->cend() - 1)->y,
                                 (contourIt->cbegin())->x,
                                 (contourIt->cbegin())->y);
            }
            
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
        rectnotdrawing = false;
        setX1(std::max(0, e->x()));
        setY1(std::max(0, e->y()));
        setX1(std::min(e->x(), ImageStorage::getInstance().getImageWidth() - 1));
        setY1(std::min(e->y(), ImageStorage::getInstance().getImageHeight() - 1));
    }
}

/*!
 * \brief ImageArea::mouseReleaseEvent
 * \param e
 */
void ImageArea::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        setX2(std::max(0, e->x()));
        setY2(std::max(0, e->y()));
        setX2(std::min(e->x(), ImageStorage::getInstance().getImageWidth() - 1));
        setY2(std::min(e->y(), ImageStorage::getInstance().getImageHeight() - 1));
        rectnotdrawing = true;
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
        setX2(std::max(0, e->x()));
        setY2(std::max(0, e->y()));
        setX2(std::min(e->x(), ImageStorage::getInstance().getImageWidth() - 1));
        setY2(std::min(e->y(), ImageStorage::getInstance().getImageHeight() - 1));
        update();
    } else {
    }
}

/*!
 * \brief ImageArea::frameChanged
 * \param _image
 */
void ImageArea::frameChanged(QImage image, Contours _contours)
{
    assert(!image.isNull());
    ImageStorage::getInstance().setImage(image);
    contours = _contours;
    rectnotdrawing = true;
    update();
}
