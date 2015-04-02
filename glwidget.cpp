#include "glwidget.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include "globjectdescriptor.h"

const char *GLWidget::vertexShaderCode =
        "#version 330\n"

        "uniform mat4 mvpMatrix;\n"

        "in vec4 vertex;\n"
        //"in vec4 color;\n"
        "in vec2 textureCoordinate;\n"

        //"out vec4 varyingColor;\n"
        "out vec2 varyingTextureCoordinate;\n"

        "void main(void) {\n"
        //"   varyingColor = color;\n"
        "   varyingTextureCoordinate = textureCoordinate;\n"
        "   gl_Position = mvpMatrix * vertex;\n"
        "}";

const char *GLWidget::fragmentShaderCode =
        "#version 330\n"

        "uniform sampler2D texture;\n"

        //"in vec4 varyingColor;\n"
        "in vec2 varyingTextureCoordinate;\n"

        "out vec4 fragColor;\n"

        "void main(void) {\n"
        //"   fragColor = varyingColor;\n"
        "   fragColor = texture2D(texture, varyingTextureCoordinate);\n"
        "}";

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_texture(QOpenGLTexture::Target2D)
    , m_objectDescriptor(0)
{
    m_distance = 5.0;
    m_yRotateAngle = 25;
    m_xRotateAngle = -25;
    m_xCameraPosition = 0.0;
    m_yCameraPosition = 0.0;
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

    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, GLWidget::vertexShaderCode);
    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, GLWidget::fragmentShaderCode);
    m_shaderProgram.link();

    m_vertexBuffer.create();
    m_vertexBuffer.bind();

    // Paint nothing per default
    //loadObjectDescriptor(GLObjectDescriptor::None);
    loadObjectDescriptor(GLObjectDescriptor::ImageObject, ":/images/qt-logo.png");
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

    if (m_texture.isBound())
        m_texture.release();
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

void GLWidget::loadObjectDescriptor(GLObjectDescriptor::GLObjectId objectId, const QString &textureImagePath)
{
    switch(objectId) {
    case GLObjectDescriptor::CubeObject:
        m_objectDescriptor.reset(GLObjectDescriptor::createCubeDescriptor());
        break;
    case GLObjectDescriptor::ImageObject:
        m_objectDescriptor.reset(GLObjectDescriptor::createImageDescriptor(textureImagePath));
        break;
    case GLObjectDescriptor::None:
    default:
        m_objectDescriptor.reset(0);
        return;
    }

    if (m_objectDescriptor.isNull()) {
        qWarning("An error has occured while loading object descriptor");
        return;
    }

    int vertexCount = m_objectDescriptor->getVertexCount();

    m_vertexBuffer.allocate(vertexCount * (3 + 3) * sizeof(GLfloat));

    int offset = 0;

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

    if (m_objectDescriptor->hasTextureImage())
        m_texture.setData(m_objectDescriptor->getTextureImage()->mirrored());

    m_vertexBuffer.release();
}

