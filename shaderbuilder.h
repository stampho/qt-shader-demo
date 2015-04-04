#ifndef SHADERBUILDER_H
#define SHADERBUILDER_H

#include <QMap>
#include <QObject>
#include <QOpenGLShader>
#include <QStringList>

class ShaderBuilder : public QObject
{
    Q_OBJECT
public:
    explicit ShaderBuilder(const QString &version = QString(), QObject *parent = 0);
    ~ShaderBuilder();

    void setVariables(QOpenGLShader::ShaderType type, QStringList code);
    QStringList getVariables(QOpenGLShader::ShaderType type) const;
    void setMainBody(QOpenGLShader::ShaderType type, QStringList code);
    QStringList getMainBody(QOpenGLShader::ShaderType type) const;

    QStringList getShaderCode(QOpenGLShader::ShaderType type) const;

private:
    QStringList readShaderFile(const QString &path);

    QString m_version;
    QMap<QOpenGLShader::ShaderType, QStringList> m_variables;
    QMap<QOpenGLShader::ShaderType, QStringList> m_mainBody;

    static QStringList m_vertexShaderFunctionsCode;
    static QStringList m_fragmentShaderFunctionsCode;
};

#endif // SHADERBUILDER_H
