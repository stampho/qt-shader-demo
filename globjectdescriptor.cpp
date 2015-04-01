#include "globjectdescriptor.h"

#include <QDebug>
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

    return cube;
}

GLObjectDescriptor *GLObjectDescriptor::createTextureDescriptor(const QString &imagePath)
{
    GLObjectDescriptor *texture = new GLObjectDescriptor();

    // TODO(pvarga): Store image in the object
    QImage image(imagePath);
    if (image.isNull()) {
        qWarning() << "Cannot load image: %s" << imagePath;
    }

    QSize imageSize = image.size();

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

    return texture;
}
