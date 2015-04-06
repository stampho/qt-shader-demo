#ifndef GLOBJECTDESCRIPTOR_H
#define GLOBJECTDESCRIPTOR_H

#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QVector2D>
#include <QVector3D>
#include <QVector>

class QImage;
class ShaderConfig;

class GLObjectDescriptor
{
public:
    static GLObjectDescriptor *createConeDescriptor(ShaderConfig* shaderConfig, int triangleCount);
    static GLObjectDescriptor *createCubeDescriptor(ShaderConfig* shaderConfig);
    static GLObjectDescriptor *createImageDescriptor(ShaderConfig* shaderConfig, const QString &imagePath);

    GLObjectDescriptor(const QString &imagePath = QString());
    ~GLObjectDescriptor();

    QVector<QVector3D> getVertices() const { return m_vertices; }

    QVector<QVector3D> getColors() const { return m_colors; }
    bool hasColors() const { return !m_colors.isEmpty(); }

    QVector<QVector2D> getTextureCoordinates() const { return m_textureCoordinates; }
    bool hasTexture() const { return !m_textureCoordinates.isEmpty(); }

    QImage *getTextureImage() { return m_image.data(); }
    bool hasTextureImage() { return !m_image.isNull(); }
    QSize getTextureImageSize();

    int getVertexCount() const { return m_vertices.count(); }

    QString getVertexShaderCode() const { return m_vertexShaderCode.join("\n"); }
    QString getFragmentShaderCode() const { return m_fragmentShaderCode.join("\n"); }

    void setCullFace(bool enabled) { m_cullFaceEnabled = enabled; }
    bool isCullFaceEnabled() const { return m_cullFaceEnabled; }

    enum GLObjectId {
        None,
        ConeObject,
        CubeObject,
        ImageObject
    };

private:
    template<typename T>
    void setVertices(T vertices[][3], int count)
    {
        m_vertices.clear();

        T x, y, z;
        for (int i = 0; i < count; ++i) {
            x = vertices[i][0];
            y = vertices[i][1];
            z = vertices[i][2];
            m_vertices << QVector3D(x, y, z);
        }
    }

    template<typename T>
    void setColors(T colors[][3], int count)
    {
        m_colors.clear();

        T r, g, b;
        for (int i = 0; i < count; ++i) {
            r = colors[i][0];
            g = colors[i][1];
            b = colors[i][2];
            m_colors << QVector3D(r, g, b);
        }
    }

    template<typename T>
    void setTextureCoordinates(T coords[][2], int count)
    {
        m_textureCoordinates.clear();

        T s, t;
        for (int i = 0; i < count; ++i) {
            s = coords[i][0];
            t = coords[i][1];
            m_textureCoordinates << QVector2D(s, t);
        }
    }

    void setVertexShaderCode(const QStringList &code) { m_vertexShaderCode = code; }
    void setFragmentShaderCode(const QStringList &code) { m_fragmentShaderCode = code; }

    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_colors;
    QVector<QVector2D> m_textureCoordinates;

    QScopedPointer<QImage> m_image;

    QStringList m_vertexShaderCode;
    QStringList m_fragmentShaderCode;

    bool m_cullFaceEnabled;
};

#endif // GLOBJECTDESCRIPTOR_H
