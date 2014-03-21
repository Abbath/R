#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGLWidget>
#include <QGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QGLShaderProgram>
#include <QImage>

/*!
 * \brief The MainWidget class
 */
class MainWidget : public QGLWidget, protected QGLFunctions
{
  Q_OBJECT

public:
  explicit MainWidget(QWidget *parent = 0);
  ~MainWidget();

  void setImage(const QImage &value);

public:
  float getStep() const;
  float getColorOffset() const;
  QVector4D getSelectColor() const;

public slots:
  void setStep(float value);
  void setColorOffset(float value);
  void setSelectColor(const QVector4D &value);
  void reset();

protected:
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void wheelEvent(QWheelEvent* pe);

  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

  void initShaders();

private:
  QGLShaderProgram program;
  QMatrix4x4 projection;
  QVector2D mousePressPosition;
  QVector3D rotationAxis;
  QQuaternion rotation;
  qreal lengthToTarget;
  QImage image;

  float step;
  float colorOffset;
  QVector4D selectColor;

  const float coordinateCoefficient = 0.005f;
  float widthOffset;
  float heightOffset;
};

#endif // MAINWIDGET_H
