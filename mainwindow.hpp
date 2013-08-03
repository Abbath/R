#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QtCore>
#include <QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);
    ~MainWindow();
    QRgb tre() const {return qRgb(threshold,threshold,threshold);}
private:
    struct Point{
        qreal x,y,z,c;
    };
    void convert(int left = 0, int top = 0, int right = 720, int bottom = 480){
        if(left < 0) left = 0;
        if(top < 0) top = 0;
        if(right > image.width()) right = image.width();
        if(bottom > image.height()) bottom = image.height();
        points.clear();
        pix.fill(Qt::black);
        if(left > right){
            left ^= right;
            right ^= left;
            left ^= right;
        }
        if(top > bottom){
            top ^= bottom;
            bottom ^= top;
            top ^= bottom;
        }
        for( int i = left; i < right; ++i){
            for( int j = top; j < bottom; ++j){
                Point p;
                p.x = i;
                p.y = j;
                p.z = qGray(image.pixel(i,j));
                p.c = p.z;
                QVector<int> v;
                if(i!=left) v.push_back(qGray(image.pixel(i-1,j)));
                if(i < right-1) v.push_back(qGray(image.pixel(i+1,j)));
                if(j!=top) v.push_back(qGray(image.pixel(i,j-1)));
                if(j < bottom-1) v.push_back(qGray(image.pixel(i,j+1)));
                if(i!=left && j!= top) v.push_back(qGray(image.pixel(i-1,j-1)));
                if(i < right-1 && j!=top) v.push_back(qGray(image.pixel(i+1,j-1)));
                if(j < bottom-1 && i!=left) v.push_back(qGray(image.pixel(i-1,j+1)));
                if(i < right-1 && j < bottom-1) v.push_back(qGray(image.pixel(i+1,j+1)));
                int min = *(std::min_element(v.begin(),v.end()));
                if(min < qGray(image.pixel(i,j))){
                    for( unsigned k = 0; k < p.c-min; ++k){
                        Point p0;
                        p0.x = i;
                        p0.y = j;
                        p0.z = qGray(image.pixel(i,j))-(k+1);
                        p0.c = qGray(image.pixel(i,j));
                        points.push_back(p0);
                    }
                }
                points.push_back(p);
            }
        }
        origin.x = 0;
        origin.y = 0;
        origin.z = 0.0;
        rotate(3.1415/180*35.2,0,0);
        rotate(0,3.1415/4,0);
        rotate(0,0,-3.1415/4);
    }
    void rotate(double angle_x =0, double angle_y=0, double angle_z=0){ //Поворот
        for(auto it = points.begin();it != points.end();++it){ //Цикл по вершинах
            double xt = it->x;double yt = it->y;double zt = it->z; //Початкові координати
            if(fabs(angle_x)  > std::numeric_limits<double>::epsilon()){ //Якщо є поворот по X
                it->y = cos(angle_x)*(yt-origin.y) - sin(angle_x)*(zt-origin.z); //Нова координата Y
                it->z = sin(angle_x)*(yt-origin.y) + cos(angle_x)*(zt-origin.z); //Нова координата X
            }
            xt = it->x; yt = it->y; zt = it->z;//Початкові координати
            if(fabs(angle_y)  > std::numeric_limits<double>::epsilon()){//Якщо є поворот по Y
                it->x = cos(angle_y)*(xt-origin.x) + sin(angle_y)*(zt-origin.z);//Нова координата X
                it->z = -sin(angle_y)*(xt-origin.x) + cos(angle_y)*(zt-origin.z);//Нова координата Z
            }
            xt = it->x; yt = it->y; zt = it->z; //Початкові координати
            if(fabs(angle_z)  > std::numeric_limits<double>::epsilon()){//Якщо є поворот по Z
                it->x = cos(angle_z)*(xt-origin.x) - sin(angle_z)*(yt-origin.y); //Нова координата Z
                it->y = sin(angle_z)*(xt-origin.x) + cos(angle_z)*(yt-origin.y); //Нова координата Y
            }
        }
    }
    Ui::MainWindow *ui;
    QImage image,pix;
    QVector<Point> points;
    bool rect,d3;
    quint32 x1,y1,x2,y2,cc;
    quint8 threshold;
    Point origin;
};

#endif // MAINWINDOW_HPP
