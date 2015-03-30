#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

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
};

#endif // GLWIDGET_H
