#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

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

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    static const char *vertexShaderCode;
    static const char *fragmentShaderCode;

    QMatrix4x4 m_projection;
    QOpenGLShaderProgram m_shaderProgram;

    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_colors;

    double m_distance;
    int m_yRotateAngle;
    int m_xRotateAngle;
};

#endif // GLWIDGET_H
