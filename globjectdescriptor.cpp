#include "globjectdescriptor.h"
#include "shaderbuilder.h"

#include "math.h"
#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QSize>

GLObjectDescriptor *GLObjectDescriptor::createConeDescriptor(ShaderConfig *shaderConfig, int triangleCount)
{
    GLObjectDescriptor *cone = new GLObjectDescriptor();

    float angleStep = 2.0 * M_PI / triangleCount;
    float angle = 0.0;
    float radius = 1.0;

    int vertexCount = triangleCount * 3;
    float coneVertices[vertexCount][3];
    int coneColors[vertexCount][3];

    for (int i = 0; i < vertexCount; i += 3) {
        // Cone top
        coneVertices[i][0] = 0.0;
        coneVertices[i][1] = 1.0;
        coneVertices[i][2] = 0.0;

        coneColors[i][0] = 1;
        coneColors[i][1] = 0;
        coneColors[i][2] = 0;

        for (int j = 1; j <= 2; ++j) {
            coneVertices[i + j][0] = radius * sin(angle + (j - 1) * angleStep);
            coneVertices[i + j][1] = -1.0;
            coneVertices[i + j][2] = radius * cos(angle + (j - 1) * angleStep);

            int odd = (i + j) % 2;
            coneColors[i + j][0] = 0;
            coneColors[i + j][1] = odd;
            coneColors[i + j][2] = !odd;
        }

        angle += angleStep;
    }

    cone->setVertices(coneVertices, vertexCount);
    cone->setColors(coneColors, vertexCount);

    ShaderBuilder shaderBuilder("120");
    QStringList vertexVariables;
    vertexVariables.append("uniform mat4 mvpMatrix;");
    vertexVariables.append("attribute vec4 vertex;");
    vertexVariables.append("attribute vec4 color;");
    vertexVariables.append("varying vec4 varyingColor;");
    shaderBuilder.setVariables(QOpenGLShader::Vertex, vertexVariables);

    QStringList vertexMain;
    vertexMain.append("varyingColor = color;");
    vertexMain.append("gl_Position = mvpMatrix * vertex;");
    shaderBuilder.setMainBody(QOpenGLShader::Vertex, vertexMain);

    QStringList fragmentVariables;
    fragmentVariables.append("uniform int animProgress;");
    fragmentVariables.append("varying vec4 varyingColor;");
    shaderBuilder.setVariables(QOpenGLShader::Fragment, fragmentVariables);

    QStringList fragmentMain;
    fragmentMain.append("gl_FragColor = varyingColor;");
    shaderBuilder.setMainBody(QOpenGLShader::Fragment, fragmentMain);

    shaderBuilder.setShaderConfig(shaderConfig);

    cone->setVertexShaderCode(shaderBuilder.getShaderCode(QOpenGLShader::Vertex));
    cone->setFragmentShaderCode(shaderBuilder.getShaderCode(QOpenGLShader::Fragment));

    return cone;
}

GLObjectDescriptor *GLObjectDescriptor::createCubeDescriptor(ShaderConfig *shaderConfig)
{
    GLObjectDescriptor *cube = new GLObjectDescriptor();

    int cubeVertices[][3] = {
        // Bottom
        { 1, -1,  1}, {-1, -1,  1}, {-1, -1, -1},
        {-1, -1, -1}, { 1, -1, -1}, { 1, -1,  1},

        // Top
        {-1,  1, -1}, {-1,  1,  1}, { 1,  1,  1},
        { 1,  1,  1}, { 1,  1, -1}, {-1,  1, -1},

        // Left
        {-1, -1,  1}, {-1,  1,  1}, {-1,  1, -1},
        {-1,  1, -1}, {-1, -1, -1}, {-1, -1,  1},

        // Right
        { 1,  1, -1}, { 1,  1,  1}, { 1, -1,  1},
        { 1, -1,  1}, { 1, -1, -1}, { 1,  1, -1},

        // Back
        {-1, -1, -1}, {-1,  1, -1}, {  1,  1, -1},
        { 1,  1, -1}, { 1, -1, -1}, { -1, -1, -1},

        // Front
        {-1, -1,  1}, { 1, -1,  1}, {  1,  1,  1},
        { 1,  1,  1}, {-1,  1,  1}, { -1, -1,  1},
    };

    int cubeColors[][3] = {
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

    int vertexCount = sizeof(cubeVertices) / (3 * sizeof(int));
    cube->setVertices(cubeVertices, vertexCount);
    cube->setColors(cubeColors, vertexCount);

    ShaderBuilder shaderBuilder("120");
    QStringList vertexVariables;
    vertexVariables.append("uniform mat4 mvpMatrix;");
    vertexVariables.append("attribute vec4 vertex;");
    vertexVariables.append("attribute vec4 color;");
    vertexVariables.append("varying vec4 varyingColor;");
    shaderBuilder.setVariables(QOpenGLShader::Vertex, vertexVariables);

    QStringList vertexMain;
    vertexMain.append("varyingColor = color;");
    vertexMain.append("gl_Position = mvpMatrix * vertex;");
    shaderBuilder.setMainBody(QOpenGLShader::Vertex, vertexMain);

    QStringList fragmentVariables;
    fragmentVariables.append("uniform int animProgress;");
    fragmentVariables.append("varying vec4 varyingColor;");
    shaderBuilder.setVariables(QOpenGLShader::Fragment, fragmentVariables);

    QStringList fragmentMain;
    fragmentMain.append("gl_FragColor = varyingColor;");
    shaderBuilder.setMainBody(QOpenGLShader::Fragment, fragmentMain);

    shaderBuilder.setShaderConfig(shaderConfig);

    cube->setVertexShaderCode(shaderBuilder.getShaderCode(QOpenGLShader::Vertex));
    cube->setFragmentShaderCode(shaderBuilder.getShaderCode(QOpenGLShader::Fragment));

    return cube;
}

GLObjectDescriptor *GLObjectDescriptor::createImageDescriptor(ShaderConfig *shaderConfig, const QString &imagePath)
{
    GLObjectDescriptor *image = new GLObjectDescriptor(imagePath);
    if (!image->hasTextureImage()) {
        qWarning() << "Unable to load image: " << imagePath;
        return 0;
    }

    QSize imageSize = image->getTextureImageSize();
    if (imageSize.width() == 0) {
        qWarning() << "Invalid image size: " << imageSize;
        return 0;
    }

    // Canvas Height
    double ch = (double)imageSize.height() / (double)imageSize.width();
    double canvasVertices[][3] = {
        {-1.0,  ch,  0.0}, {-1.0, -ch,  0.0}, { 1.0, -ch,  0.0},
        { 1.0, -ch,  0.0}, { 1.0,  ch,  0.0}, {-1.0,  ch,  0.0},
    };

    int textureCoodinates[][2] = {
        {0, 1}, {0, 0}, {1, 0},
        {1, 0}, {1, 1}, {0, 1},
    };

    int vertexCount = sizeof(canvasVertices) / (3 * sizeof(double));
    image->setVertices(canvasVertices, vertexCount);
    image->setTextureCoordinates(textureCoodinates, vertexCount);

    ShaderBuilder shaderBuilder("120");
    QStringList vertexVariables;
    vertexVariables.append("uniform mat4 mvpMatrix;");
    vertexVariables.append("attribute vec4 vertex;");
    vertexVariables.append("attribute vec2 textureCoordinate;");
    vertexVariables.append("varying vec2 varyingTextureCoordinate;");
    shaderBuilder.setVariables(QOpenGLShader::Vertex, vertexVariables);

    QStringList vertexMain;
    vertexMain.append("varyingTextureCoordinate = textureCoordinate;");
    vertexMain.append("gl_Position = mvpMatrix * vertex;");
    shaderBuilder.setMainBody(QOpenGLShader::Vertex, vertexMain);

    QStringList fragmentVariables;
    fragmentVariables.append("uniform int animProgress;");
    fragmentVariables.append("uniform sampler2D texture;");
    fragmentVariables.append("uniform vec2 textureSize;");
    fragmentVariables.append("varying vec2 varyingTextureCoordinate;");
    shaderBuilder.setVariables(QOpenGLShader::Fragment, fragmentVariables);

    QStringList fragmentMain;
    fragmentMain.append("gl_FragColor = texture2D(texture, varyingTextureCoordinate);");
    shaderBuilder.setMainBody(QOpenGLShader::Fragment, fragmentMain);

    shaderBuilder.setShaderConfig(shaderConfig);

    image->setVertexShaderCode(shaderBuilder.getShaderCode(QOpenGLShader::Vertex));
    image->setFragmentShaderCode(shaderBuilder.getShaderCode(QOpenGLShader::Fragment));

    return image;
}

GLObjectDescriptor::GLObjectDescriptor(const QString &imagePath)
    : m_image(0)
    , m_cullFaceEnabled(false)
    , m_polygonLineModeEnabled(false)
{
    QFileInfo imageFile(imagePath);
    if (imageFile.exists() && imageFile.isFile())
        m_image.reset(new QImage(imagePath));
}


GLObjectDescriptor::~GLObjectDescriptor()
{
}

QSize GLObjectDescriptor::getTextureImageSize()
{
    if (m_image.isNull())
        return QSize(0, 0);

    return m_image->size();
}
