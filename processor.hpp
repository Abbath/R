#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QRunnable>
#include <QMessageBox>
#include <helpers.hpp>

class Processor : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Processor(QObject* parent = 0);
    void setFilename(QString _filename){filename = _filename;}
    void setThreshold(int _threshold){threshold = _threshold;}
    void setRect(QRect _rect){rect = _rect;}
    void run();
    QImage IplImage2QImage(const IplImage *iplImage);
    QPair<int, double> processImage(QImage _image);
private:
    unsigned int qrgbToGray(QRgb rgb);
signals:
    void frameChanged(QImage frame);
    void graphL(const QVector<int> &v);
    void graphM(const QVector<double> &v);
public slots:
    void stopThis();
private:
    QString filename;
    QRect rect;
    unsigned int threshold = 255;
    volatile bool stop = false;
};

#endif // VIDEOPROCESSOR_H
