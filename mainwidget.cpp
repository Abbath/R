#include "mainwidget.hpp"

#include <QMouseEvent>

#include <math.h>
#include <locale.h>

#include <cassert>

/*!
 * \brief MainWidget::MainWidget
 * \param parent
 */
MainWidget::MainWidget(QWidget *parent) :
  QGLWidget(parent),
  lengthToTarget(-5.0),
  step(0.99f),
  colorOffset(0.0f),
  selectColor(1.0f, 0.0f, 0.0f, 1.0f),
  widthOffset(0.0f),
  heightOffset(0.0f)
{
  setWindowState(Qt::WindowMaximized);
}

/*!
 * \brief MainWidget::~MainWidget
 */
MainWidget::~MainWidget()
{
}

/*!
 * \brief MainWidget::mouseMoveEvent
 * \param e
 */
void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
  const QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
  rotationAxis = QVector3D(diff.y(), diff.x(), 0.0);
  rotation = QQuaternion::fromAxisAndAngle(rotationAxis, 2.5f) * rotation;
  mousePressPosition = QVector2D( e->localPos() );
  updateGL();
}

/*!
 * \brief MainWidget::mousePressEvent
 * \param e
 */
void MainWidget::mousePressEvent(QMouseEvent *e)
{
  mousePressPosition = QVector2D(e->localPos());
}

/*!
 * \brief MainWidget::wheelEvent
 * \param pe
 */
void MainWidget::wheelEvent(QWheelEvent *pe)
{
  const auto delta = pe->delta()/120;
  if ( delta > 0 ){
    lengthToTarget *= 0.8f;
    lengthToTarget = std::min( lengthToTarget, -1.0 );
  }
  else{
    lengthToTarget /= 0.8f;
    lengthToTarget = std::max( lengthToTarget, -100.0 );
  }
  updateGL();
}

/*!
 * \brief MainWidget::initializeGL
 */
void MainWidget::initializeGL()
{
  initializeGLFunctions();
  qglClearColor(Qt::black);
  initShaders();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

/*!
 * \brief MainWidget::initShaders
 */
void MainWidget::initShaders()
{
  setlocale(LC_NUMERIC, "C");
  if ( !program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl") ) {
    close();
  }
  if ( ! program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl") ) {
    close();
  }
  if ( !program.link() ) {
    close();
  }
  if ( ! program.bind() ) {
    close();
  }
  setlocale(LC_ALL, "");
}

/*!
 * \brief MainWidget::getSelectColor
 * \return
 */
QVector4D MainWidget::getSelectColor() const
{
  return selectColor;
}

/*!
 * \brief MainWidget::setSelectColor
 * \param value
 */
void MainWidget::setSelectColor(const QVector4D &value)
{
  selectColor = value;
  updateGL();
}

/*!
 * \brief MainWidget::reset
 */
void MainWidget::reset()
{
  rotation = QQuaternion();
  lengthToTarget = -5.0;
  updateGL();
}

/*!
 * \brief MainWidget::getColorOffset
 * \return
 */
float MainWidget::getColorOffset() const
{
  return colorOffset;
}

/*!
 * \brief MainWidget::setColorOffset
 * \param value
 */
void MainWidget::setColorOffset(float value)
{
  colorOffset = value;
  updateGL();
}

/*!
 * \brief MainWidget::getStep
 * \return
 */
float MainWidget::getStep() const
{
  return step;
}

/*!
 * \brief MainWidget::setStep
 * \param value
 */
void MainWidget::setStep(float value)
{
  step = value;
  updateGL();
}

/*!
 * \brief MainWidget::setImage
 * \param value
 */
void MainWidget::setImage(const QImage &value)
{
  image = value.scaled(600, 600, Qt::KeepAspectRatio);
  widthOffset = image.size().width()/2 * coordinateCoefficient;
  heightOffset = image.size().height()/2 * coordinateCoefficient;
  updateGL();
}

/*!
 * \brief MainWidget::resizeGL
 * \param w
 * \param h
 */
void MainWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);

  const qreal aspect = qreal(w) / qreal(h ? h : 1);

  constexpr qreal zNear = 1.0;
  constexpr qreal zFar = 100.0;
  constexpr qreal fov = 25.0;

  projection.setToIdentity();

  projection.perspective(fov, aspect, zNear, zFar);
}

/*!
 * \brief MainWidget::paintGL
 */
void MainWidget::paintGL()
{
    // Clear color and depth buffer
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
     if ( (image.size().width() <= 0) || (image.size().height() <= 0) ) { return; }
   
     assert(image.size().width() <= 1024);
     assert(image.size().height() <= 1024);
   
     // Calculate model view transformation
     QMatrix4x4 matrix;
     matrix.translate(0.0, 0.0, lengthToTarget);
     matrix.rotate(rotation);
   
     // Set modelview-projection matrix
     program.setUniformValue("mvp_matrix", projection * matrix);
   
     // Coloring options
     program.setUniformValue("colorOffset", colorOffset);
     program.setUniformValue("step", step);
     program.setUniformValue("selectColor", selectColor);
     // End coloring options
   
     glBegin(GL_LINES);
     for ( auto i = 0; i < image.size().width(); ++ i ) {
       for ( auto j = 0; j < image.size().height(); ++ j ) {
         const float zVal = float( qGray( image.pixel( i, j ) ) )/255;
         glVertex3f(-widthOffset + i*coordinateCoefficient, -heightOffset + j*coordinateCoefficient, 0.0f);
         if ( zVal > step ) {
           glVertex3f(-widthOffset + i*coordinateCoefficient, -heightOffset + j*coordinateCoefficient, step - 0.001f);
           glVertex3f(-widthOffset + i*coordinateCoefficient, -heightOffset + j*coordinateCoefficient, step);
         }
         glVertex3f(-widthOffset + i*coordinateCoefficient, -heightOffset + j*coordinateCoefficient, zVal);
       }
     }
     glEnd();
   
  

}
