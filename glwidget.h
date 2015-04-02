#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "globjectdescriptor.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>

class QWheelEvent;
class QMouseEvent;

enum Axis {
    X_AXIS = 0b100,
    Y_AXIS = 0b010,
    Z_AXIS = 0b001
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void rotate(int angle, Axis axis);
    void loadObjectDescriptor(GLObjectDescriptor::GLObjectId objectId, const QString &textureImagePath = QString());

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    static const char *vertexShaderCode;
    static const char *fragmentShaderCode;

    QMatrix4x4 m_projection;
    QOpenGLShaderProgram m_shaderProgram;

    QOpenGLBuffer m_vertexBuffer;
    QOpenGLTexture m_texture;
    QScopedPointer<GLObjectDescriptor> m_objectDescriptor;

    double m_distance;
    int m_yRotateAngle;
    int m_xRotateAngle;
    double m_xCameraPosition;
    double m_yCameraPosition;

    QPoint m_lastMousePosition;
};

#endif // GLWIDGET_H
