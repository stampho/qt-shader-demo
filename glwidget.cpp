#include "glwidget.h"

#include <QMouseEvent>
#include <QWheelEvent>

const char *GLWidget::vertexShaderCode =
        "#version 330\n"

        "uniform mat4 mvpMatrix;\n"

        "in vec4 vertex;\n"
        "in vec4 color;\n"

        "out vec4 varyingColor;\n"

        "void main(void) {\n"
        "   varyingColor = color;\n"
        "   gl_Position = mvpMatrix * vertex;\n"
        "}";

const char *GLWidget::fragmentShaderCode =
        "#version 330\n"

        "in vec4 varyingColor;\n"

        "out vec4 fragColor;\n"

        "void main(void) {\n"
        "   fragColor = varyingColor;\n"
        "}";

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_distance = 5.0;
    m_yRotateAngle = 25;
    m_xRotateAngle = -25;
    m_xCameraPosition = 0.0;
    m_yCameraPosition = 0.0;
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, GLWidget::vertexShaderCode);
    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, GLWidget::fragmentShaderCode);
    m_shaderProgram.link();

    const int cubeVertices[][3] = {
        // Bottom
        {-1, -1,  1}, {-1, -1, -1}, { 1, -1,  1},
        { 1, -1,  1}, { 1, -1, -1}, {-1, -1, -1},

        // Top
        {-1,  1,  1}, {-1,  1, -1}, { 1,  1,  1},
        { 1,  1,  1}, { 1,  1, -1}, {-1,  1, -1},

        // Left
        {-1, -1,  1}, {-1,  1,  1}, {-1,  1, -1},
        {-1,  1, -1}, {-1, -1, -1}, {-1, -1,  1},

        // Right
        { 1, -1,  1}, { 1,  1,  1}, { 1,  1, -1},
        { 1,  1, -1}, { 1, -1, -1}, { 1, -1,  1},

        // Back
        {-1, -1, -1}, {-1,  1, -1}, {  1, -1, -1},
        { 1, -1, -1}, { 1,  1, -1}, { -1,  1, -1},

        // Front
        {-1, -1,  1}, {-1,  1,  1}, {  1, -1,  1},
        { 1, -1,  1}, { 1,  1,  1}, { -1,  1,  1},
    };

    const int cubeColors[][3] = {
        // Bottom
        {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
        {1, 0, 0}, {1, 0, 0}, {1, 0, 0},

        // Top
        {0, 1, 0}, {0, 1, 0}, {0, 1, 0},
        {0, 1, 0}, {0, 1, 0}, {0, 1, 0},

        // Left
        {1, 1, 0}, {1, 1, 0}, {1, 1, 0},
        {1, 1, 0}, {1, 1, 0}, {1, 1, 0},

        // Right
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1},

        // Back
        {1, 0, 1}, {1, 0, 1}, {1, 0, 1},
        {1, 0, 1}, {1, 0, 1}, {1, 0, 1},

        // Front
        {0, 1, 1}, {0, 1, 1}, {0, 1, 1},
        {0, 1, 1}, {0, 1, 1}, {0, 1, 1},

    };

    int x, y, z;
    int r, g, b;
    int vertexCount = sizeof(cubeVertices) / (3 * sizeof(int));
    for (int i = 0; i < vertexCount; ++i) {
        x = cubeVertices[i][0];
        y = cubeVertices[i][1];
        z = cubeVertices[i][2];
        m_vertices << QVector3D(x, y, z);

        r = cubeColors[i][0];
        g = cubeColors[i][1];
        b = cubeColors[i][2];
        m_colors << QVector3D(r, g, b);
    }
}

void GLWidget::resizeGL(int width, int height)
{
    if (height == 0)
        height = 1;

    m_projection.setToIdentity();
    m_projection.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 mMatrix;
    QMatrix4x4 vMatrix;

    QMatrix4x4 rotationMatrix;
    rotationMatrix.rotate(m_yRotateAngle, 0, 1, 0);
    rotationMatrix.rotate(m_xRotateAngle, 1, 0, 0);
    mMatrix *= rotationMatrix;

    QMatrix4x4 translationMatrix;
    translationMatrix.translate(m_xCameraPosition, m_yCameraPosition, 0);
    QVector3D eye = translationMatrix * QVector3D(0, 0, m_distance);
    QVector3D center = translationMatrix * QVector3D(0, 0, 0);
    QVector3D up = QVector3D(0, 1, 0);
    vMatrix.lookAt(eye, center, up);

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("mvpMatrix", m_projection * vMatrix * mMatrix);

    m_shaderProgram.setAttributeArray("vertex", m_vertices.constData());
    m_shaderProgram.enableAttributeArray("vertex");

    m_shaderProgram.setAttributeArray("color", m_colors.constData());
    m_shaderProgram.enableAttributeArray("color");

    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

    m_shaderProgram.disableAttributeArray("vertex");
    m_shaderProgram.disableAttributeArray("color");

    m_shaderProgram.release();
}

void GLWidget::rotate(int angle, Axis axis)
{
    switch(axis){
    case Y_AXIS:
        m_yRotateAngle += angle;
        while (m_yRotateAngle < 0)  m_yRotateAngle += 360;
        while (m_yRotateAngle >= 360) m_yRotateAngle -= 360;
        update();
        break;
    case X_AXIS:
        m_xRotateAngle += angle;
        while (m_xRotateAngle < 0)  m_xRotateAngle += 360;
        while (m_xRotateAngle >= 360) m_xRotateAngle -= 360;
        update();
        break;
    default:
        return;
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastMousePosition = event->pos();
    event->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int deltaX = event->x() - m_lastMousePosition.x();
    int deltaY = event->y() - m_lastMousePosition.y();

    if (event->buttons() & Qt::LeftButton) {
        // TODO(pvarga): The step of movement should depend on m_distance too.
        m_xCameraPosition -= (double)deltaX / 100.0;
        m_yCameraPosition += (double)deltaY / 100.0;
        update();
    }

    m_lastMousePosition = event->pos();
    event->accept();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (event->orientation() == Qt::Vertical) {
        if (delta < 0)
            m_distance *= 1.1;
        else if (delta > 0)
            m_distance *= 0.9;

        update();
    }

    event->accept();
}
