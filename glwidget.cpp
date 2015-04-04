#include "glwidget.h"

#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

#include "globjectdescriptor.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_texture(QOpenGLTexture::Target2D)
    , m_objectDescriptor(0)
    , m_shaderAnimTimer(new QTimer(this))
{
    m_distance = 5.0;
    m_yRotateAngle = 25;
    m_xRotateAngle = -25;
    m_xCameraPosition = 0.0;
    m_yCameraPosition = 0.0;

    connect(m_shaderAnimTimer, SIGNAL(timeout()), this, SLOT(shaderAnimTimerTimeout()));
}

GLWidget::~GLWidget()
{
    makeCurrent();
    m_texture.destroy();
    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    // TODO(pvarga): Add button for the GL_CULL_FACE
    //glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_vertexBuffer.create();
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

    // If object descriptor is not set there is nothing to paint
    if (m_objectDescriptor.isNull())
        return;

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
    if (m_objectDescriptor->hasTextureImage()) {
        m_texture.bind();
        m_shaderProgram.setUniformValue("texture", 0);
    }
    m_shaderProgram.setUniformValue("animProgress", m_shaderAnimProgress);

    int offset = 0;
    int vertexCount = m_objectDescriptor->getVertexCount();

    m_vertexBuffer.bind();
    m_shaderProgram.setAttributeBuffer("vertex", GL_FLOAT, offset, 3, 0);
    m_shaderProgram.enableAttributeArray("vertex");
    offset += vertexCount * 3 * sizeof(GLfloat);

    if (m_objectDescriptor->hasColors()) {
        m_shaderProgram.setAttributeBuffer("color", GL_FLOAT, offset, 3, 0);
        m_shaderProgram.enableAttributeArray("color");
        offset += vertexCount * 3 * sizeof(GLfloat);
    }

    if (m_objectDescriptor->hasTexture()) {
        m_shaderProgram.setAttributeBuffer("textureCoordinate", GL_FLOAT, offset, 2, 0);
        m_shaderProgram.enableAttributeArray("textureCoordinate");
        offset += vertexCount * 2 * sizeof(GLfloat);
    }

    m_vertexBuffer.release();

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    m_shaderProgram.disableAttributeArray("vertex");
    m_shaderProgram.disableAttributeArray("color");

    m_shaderProgram.release();

    if (m_objectDescriptor->hasTextureImage()) {
        Q_ASSERT(m_texture.isBound());
        m_texture.release();
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

void GLWidget::rotate(int angle, Axis::Axis axis)
{
    switch(axis){
    case Axis::Y:
        m_yRotateAngle += angle;
        while (m_yRotateAngle < 0)  m_yRotateAngle += 360;
        while (m_yRotateAngle >= 360) m_yRotateAngle -= 360;
        update();
        break;
    case Axis::X:
        m_xRotateAngle += angle;
        while (m_xRotateAngle < 0)  m_xRotateAngle += 360;
        while (m_xRotateAngle >= 360) m_xRotateAngle -= 360;
        update();
        break;
    default:
        return;
    }
}

void GLWidget::updateObjectDescriptor(GLObjectDescriptor *objectDescriptor)
{
    m_objectDescriptor.reset(objectDescriptor);
    if (!objectDescriptor) {
        update();
        return;
    }

    updateVertexBuffer();
    updateTexture();
    updateShaderProgram();

    update();
}

void GLWidget::resetShaderAnimTimer(int msec)
{
    m_shaderAnimProgress = 0;
    m_shaderAnimTimer->start(msec);
}

void GLWidget::updateVertexBuffer()
{
    int offset = 0;
    int vertexCount = m_objectDescriptor->getVertexCount();

    m_vertexBuffer.bind();
    int elementCount = 3; // vertex
    if (m_objectDescriptor->hasColors())
        elementCount += 3;
    if (m_objectDescriptor->hasTexture())
        elementCount += 2;
    m_vertexBuffer.allocate(vertexCount * elementCount * sizeof(GLfloat));

    m_vertexBuffer.write(offset, m_objectDescriptor->getVertices().constData(), vertexCount * 3 * sizeof(GLfloat));
    offset += vertexCount * 3 * sizeof(GLfloat);

    if (m_objectDescriptor->hasColors()) {
        m_vertexBuffer.write(offset, m_objectDescriptor->getColors().constData(), vertexCount * 3 * sizeof(GLfloat));
        offset += vertexCount * 3 * sizeof(GLfloat);
    }

    if (m_objectDescriptor->hasTexture()) {
        m_vertexBuffer.write(offset, m_objectDescriptor->getTextureCoordinates().constData(), vertexCount * 2 * sizeof(GLfloat));
        offset += vertexCount * 2 * sizeof(GLfloat);
    }

    m_vertexBuffer.release();
}

void GLWidget::updateTexture()
{
    m_texture.destroy();

    if (!m_objectDescriptor->hasTextureImage())
        return;

    m_texture.setData(m_objectDescriptor->getTextureImage()->mirrored());
}

void GLWidget::updateShaderProgram()
{
    m_shaderProgram.removeAllShaders();
    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, m_objectDescriptor->getVertexShaderCode());
    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, m_objectDescriptor->getFragmentShaderCode());
    m_shaderProgram.link();
}

void GLWidget::shaderAnimTimerTimeout()
{
    m_shaderAnimProgress += 5;

    update();

    if (m_shaderAnimProgress >= 100)
        m_shaderAnimTimer->stop();
}
