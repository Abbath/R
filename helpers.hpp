#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <QtCore>
#include <opencv/cv.h>
#include <opencv/highgui.h>

struct Point{
    qreal x, y, z, c;
};

struct Display{
     quint32 maxx, maxy, minx, miny;
     quint32 sum;
     qreal mean;
};

struct Image{
int x1, y1, x2, y2, square;
int width(){ return abs(x1 - x2); }
int height(){ return abs(y1 - y2); }
int histogram[256];
Image(){
    x1 = x2 = y1 = y2 = square = 0;
    for(auto & x: histogram){
        x = 0;
    }
}
void flush(){
    x1 = x2 = y1 = y2 = square = 0;
    for(auto & x: histogram){
        x = 0;
    }
}
};
#endif // HELPERS_HPP
