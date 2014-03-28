#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <QtCore>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <QVector4D>


struct Display{
    int maxx;
    int maxy;
    int minx;
    int miny;
    int sum;
    qreal mean;
};


#endif // HELPERS_HPP
