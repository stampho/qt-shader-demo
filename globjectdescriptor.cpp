#include "globjectdescriptor.h"

#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QSize>

GLObjectDescriptor *GLObjectDescriptor::createCubeDescriptor()
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

    QStringList vertexShaderCode;
    vertexShaderCode.append("#version 330");
    vertexShaderCode.append("uniform mat4 mvpMatrix;");
    vertexShaderCode.append("in vec4 vertex;");
    vertexShaderCode.append("in vec4 color;");
    vertexShaderCode.append("out vec4 varyingColor;");
    vertexShaderCode.append("void main(void) {");
    vertexShaderCode.append("   varyingColor = color;");
    vertexShaderCode.append("   gl_Position = mvpMatrix * vertex;");
    vertexShaderCode.append("}");

    QStringList fragmentShaderCode;
    fragmentShaderCode.append("#version 330");
    fragmentShaderCode.append("uniform int animProgress;");
    fragmentShaderCode.append("in vec4 varyingColor;");
    fragmentShaderCode.append("out vec4 fragColor;");
    fragmentShaderCode.append("void main(void) {");
    fragmentShaderCode.append(" fragColor = varyingColor;");
    fragmentShaderCode.append("}");

    cube->setVertexShaderCode(vertexShaderCode);
    cube->setFragmentShaderCode(fragmentShaderCode);

    return cube;
}

GLObjectDescriptor *GLObjectDescriptor::createImageDescriptor(const QString &imagePath)
{
    GLObjectDescriptor *texture = new GLObjectDescriptor(imagePath);
    if (!texture->hasTextureImage()) {
        qWarning() << "Unable to load image: " << imagePath;
        return 0;
    }

    QSize imageSize = texture->getTextureImageSize();
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
    texture->setVertices(canvasVertices, vertexCount);
    texture->setTextureCoordinates(textureCoodinates, vertexCount);

    QStringList vertexShaderCode;
    vertexShaderCode.append("#version 330");
    vertexShaderCode.append("uniform mat4 mvpMatrix;");
    vertexShaderCode.append("in vec4 vertex;");
    vertexShaderCode.append("in vec2 textureCoordinate;");
    vertexShaderCode.append("out vec2 varyingTextureCoordinate;");
    vertexShaderCode.append("void main(void) {;");
    vertexShaderCode.append("   varyingTextureCoordinate = textureCoordinate;");
    vertexShaderCode.append("   gl_Position = mvpMatrix * vertex;");
    vertexShaderCode.append("}");

    QStringList fragmentShaderCode;
    fragmentShaderCode.append("#version 330");
    fragmentShaderCode.append("uniform sampler2D texture;");
    fragmentShaderCode.append("uniform int animProgress;");
    fragmentShaderCode.append("in vec2 varyingTextureCoordinate;");
    fragmentShaderCode.append("out vec4 fragColor;");

    fragmentShaderCode.append("float lightness(vec4 color) {");
    fragmentShaderCode.append(" float cmax = max(color[0], max(color[1], color[2]));");
    fragmentShaderCode.append(" float cmin = min(color[0], min(color[1], color[2]));");
    fragmentShaderCode.append(" return (cmax + cmin) / 2;");
    fragmentShaderCode.append("}");

    fragmentShaderCode.append("vec4 gray(vec4 color) {");
    fragmentShaderCode.append(" float l = lightness(color);");
    fragmentShaderCode.append(" return vec4(l, l, l, 1.0);");
    fragmentShaderCode.append("}");

    fragmentShaderCode.append("vec4 invert(vec4 color) {");
    fragmentShaderCode.append(" return vec4(1.0 - color[0], 1.0 - color[1], 1.0 - color[2], color[3]);");
    fragmentShaderCode.append("}");

    fragmentShaderCode.append("void main(void) {");
    fragmentShaderCode.append(" fragColor = texture2D(texture, varyingTextureCoordinate);");
    fragmentShaderCode.append(" float progress = clamp(animProgress / 100.0, 0.0, 1.0);");
    fragmentShaderCode.append(" if (varyingTextureCoordinate[1] > (1.0 - progress)) {");
    fragmentShaderCode.append("     fragColor = gray(fragColor);");
    fragmentShaderCode.append("     fragColor = invert(fragColor);");
    fragmentShaderCode.append(" }");
    fragmentShaderCode.append("}");

    texture->setVertexShaderCode(vertexShaderCode);
    texture->setFragmentShaderCode(fragmentShaderCode);

    return texture;
}

GLObjectDescriptor::GLObjectDescriptor(const QString &imagePath)
    : m_image(0)
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
