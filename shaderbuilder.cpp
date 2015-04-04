#include "shaderbuilder.h"

#include <QDebug>
#include <QFile>

QStringList ShaderBuilder::m_vertexShaderFunctionsCode;
QStringList ShaderBuilder::m_fragmentShaderFunctionsCode;

ShaderBuilder::ShaderBuilder(const QString &version, QObject *parent)
    : QObject(parent)
    , m_version(version)
{
#if 0
    // TODO(pvarga): There has been no function implemented for the vertex shader yet
    if (m_vertexShaderFunctionsCode.isEmpty())
        m_vertexShaderFunctionsCode.append(readShaderFile(":/shaders/functions-120.vert"));
#endif

    if (m_fragmentShaderFunctionsCode.isEmpty())
        m_fragmentShaderFunctionsCode.append(readShaderFile(":/shaders/functions-120.frag"));
}

ShaderBuilder::~ShaderBuilder()
{

}

void ShaderBuilder::setVariables(QOpenGLShader::ShaderType type, QStringList code)
{
    m_variables.insert(type, code);
}

QStringList ShaderBuilder::getVariables(QOpenGLShader::ShaderType type) const
{
    if(!m_variables.contains(type))
        return QStringList();
    return m_variables.value(type);
}

void ShaderBuilder::setMainBody(QOpenGLShader::ShaderType type, QStringList code)
{
    m_mainBody.insert(type, code);
}

QStringList ShaderBuilder::getMainBody(QOpenGLShader::ShaderType type) const
{
    if(!m_mainBody.contains(type))
        return QStringList();
    return m_mainBody.value(type);
}

QStringList ShaderBuilder::getShaderCode(QOpenGLShader::ShaderType type) const
{
    QStringList functionsCode;

    switch(type) {
    case QOpenGLShader::Vertex:
        functionsCode.append(m_vertexShaderFunctionsCode);
        break;
    case QOpenGLShader::Fragment:
        functionsCode.append(m_fragmentShaderFunctionsCode);
        break;
    default:
        return QStringList();
    }

    QStringList shaderCode;

    if (!m_version.isEmpty())
        shaderCode.append(QString("#version %0").arg(m_version));

    shaderCode.append(getVariables(type));
    shaderCode.append(functionsCode);

    shaderCode.append("void main(void)");
    shaderCode.append("{");
    foreach (QString mainBodyLine, getMainBody(type)) {
        shaderCode.append(QString("\t%0").arg(mainBodyLine));
    }
    shaderCode.append("}");

    return shaderCode;
}

QStringList ShaderBuilder::readShaderFile(const QString &path)
{
    QFile shaderFile(path);
    if (!shaderFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to read shader file: " << path;
        return QStringList();
    }

    QStringList shaderLines;
    while (!shaderFile.atEnd()) {
        QString line = shaderFile.readLine();
        line.remove(QRegExp("[\\s\n]*$"));
        shaderLines.append(line);
    }

    shaderFile.close();
    return shaderLines;
}
