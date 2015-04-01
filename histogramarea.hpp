#ifndef HISTOGRAMAREA_HPP
#define HISTOGRAMAREA_HPP

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

namespace Ui {
class HistogramArea;
}

class HistogramArea : public QWidget
{
    Q_OBJECT
    
public:
    explicit HistogramArea(QWidget *parent = 0);
    ~HistogramArea();
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    QImage getHistogram() const;
    void setHistogram(const QImage &value);
    
private:
    QImage histogram;
    Ui::HistogramArea *ui;

public slots:
    void histChanged(QImage image, int w, double coef);
};

#endif // HISTOGRAMAREA_HPP
