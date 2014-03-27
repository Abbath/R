#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QRunnable>
#include <QMessageBox>
#include <QtCore>
#include <helpers.hpp>

class VideoProcessor : public QObject, public QRunnable
{
    Q_OBJECT
    QString filename;
    QRect rect;
    unsigned int threshold = 255;
    volatile bool stop = false;
    unsigned int qrgbToGray(QRgb rgb);
public:
    explicit VideoProcessor(QObject* parent = 0);
    void setFilename(QString _filename){filename = _filename;}
    void setThreshold(int _threshold){threshold = _threshold;}
    void setRect(QRect _rect){rect = _rect;}
    void run();
    QImage IplImage2QImage(const IplImage *iplImage);
private:
    QPair<int, double> processImage(QImage _image);
signals:
    void frameChanged(QImage frame);
    void graphL(const QVector<int> &v);
    void graphM(const QVector<double> &v);
public slots:
    void stopThis();
};

#endif // VIDEOPROCESSOR_H
