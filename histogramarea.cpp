#include "histogramarea.hpp"
#include "ui_histogramarea.h"

HistogramArea::HistogramArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistogramArea)
{
    ui->setupUi(this);
}

HistogramArea::~HistogramArea()
{
    delete ui;
}

void HistogramArea::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    if(!histogram.isNull()){
        histogram = histogram.scaled(this->size(), Qt::KeepAspectRatio);
        painter.drawImage(0, 0, histogram);
    }
    e->accept();
}

void HistogramArea::mouseMoveEvent(QMouseEvent *e)
{
    
}

QImage HistogramArea::getHistogram() const
{
    return histogram;
}

void HistogramArea::setHistogram(const QImage &value)
{
    histogram = value;
}

void HistogramArea::histChanged(QImage image, int w, double coef)
{
    if(!image.isNull()){
        histogram = image;
        QPainter p(&histogram);
        p.setPen(Qt::green);
        p.drawText(0, histogram.height()-2, "0");
        p.drawText(w, histogram.height()-2, "255");
        p.drawText(0, 12, QString::number(100*coef)+"%");
        p.drawText(0, histogram.height()/2, QString::number(100*coef/2)+"%");        
        update();
    }
}

