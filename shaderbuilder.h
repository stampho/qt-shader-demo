#ifndef SHADERBUILDER_H
#define SHADERBUILDER_H

#include <QMap>
#include <QObject>
#include <QOpenGLShader>
#include <QStringList>
#include <QVector>

struct ShaderConfig {
    enum IPShader {
        None = 0,
        Gauss,
        Sobel,
        SobelGauss,
        Canny
    };

    bool animEnabled;

    bool gray;
    bool invert;
    bool threshold;

    IPShader imageProcessShader;
};

class ShaderBuilder : public QObject
{
    Q_OBJECT
public:
    explicit ShaderBuilder(const QString &version = QString(), QObject *parent = 0);
    ~ShaderBuilder();

    void setVariables(QOpenGLShader::ShaderType type, QStringList code);
    void setMainBody(QOpenGLShader::ShaderType type, QStringList code);
    void setShaderConfig(ShaderConfig *shaderConfig);

    QStringList getShaderCode(QOpenGLShader::ShaderType type) const;

private:
    QStringList readShaderFile(const QString &path);
    QStringList generateConstants(QOpenGLShader::ShaderType type) const;
    QStringList getVariables(QOpenGLShader::ShaderType type) const;
    QStringList getMainBody(QOpenGLShader::ShaderType type) const;

    static QVector<float> computeGaussianKernel(int kernelRadius, float sigma);

    QString m_version;
    QMap<QOpenGLShader::ShaderType, QStringList> m_variables;
    QMap<QOpenGLShader::ShaderType, QStringList> m_mainBody;

    ShaderConfig *m_shaderConfig;

    static QStringList m_vertexShaderFunctionsCode;
    static QStringList m_fragmentShaderFunctionsCode;
    static int m_kernelRadius;
    static QVector<float> m_gaussianKernel;
};

#endif // SHADERBUILDER_H
