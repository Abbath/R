#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <helpers.hpp>
#include <QtConcurrent/QtConcurrent>
class Converter
{
public:
    Converter();
    void rotate(QVector<Point> &points, Point origin, double angle_x =0, double angle_y =0, double angle_z = 0);
    QVector<Point> convert(QImage &image, int left = 0, int top = 0, int right = 720, int bottom = 480);
    int processVideo(QString s);
    QImage IplImage2QImage(const IplImage *iplImage);
};

#endif // CONVERTER_HPP
