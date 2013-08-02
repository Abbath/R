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
    void convert(){
        for( int i = 0; i < image.width(); ++i){
            for( int j = 0; j < image.height(); ++j){
                Point p;
                p.x = i;
                p.y = j;
                p.z = qGray(image.pixel(i,j));
                p.c = p.z;
                QVector<int> v;
                if(i!=0) v.push_back(qGray(image.pixel(i-1,j)));
                if(i < image.width()-1) v.push_back(qGray(image.pixel(i+1,j)));
                if(j!=0) v.push_back(qGray(image.pixel(i,j-1)));
                if(j < image.height()-1) v.push_back(qGray(image.pixel(i,j+1)));
                if(i!=0 && j!= 0) v.push_back(qGray(image.pixel(i-1,j-1)));
                if(i < image.width()-1 && j!=0) v.push_back(qGray(image.pixel(i+1,j-1)));
                if(j < image.height()-1 && i!=0) v.push_back(qGray(image.pixel(i-1,j+1)));
                if(i < image.width()-1 && j < image.height()-1) v.push_back(qGray(image.pixel(i+1,j+1)));
                int min = *(std::min_element(v.begin(),v.end()));
                //qDebug() << min;
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
    quint32 x1,y1,x2,y2;
    quint8 threshold;
    Point origin;
};

#endif // MAINWINDOW_HPP
