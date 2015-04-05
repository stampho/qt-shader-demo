#include "shaderbuilder.h"

#include <math.h>
#include <QDebug>
#include <QFile>

QStringList ShaderBuilder::m_vertexShaderFunctionsCode;
QStringList ShaderBuilder::m_fragmentShaderFunctionsCode;

int ShaderBuilder::m_kernelRadius = 4;
QVector<float> ShaderBuilder::m_gaussianKernel;

QVector<float> ShaderBuilder::computeGaussianKernel(int kernelRadius, float sigma)
{
    const int kernelSize = kernelRadius * 2 + 1;
    QVector<float> kernel(kernelSize * kernelSize);

    float a = 1 / (2 * M_PI * sigma * sigma);

    // sum is used for normalization
    float sum = 0.0;

    float distance, b, value;
    int i, j;
    for (int x = -kernelRadius; x <= kernelRadius; ++x) {
        for (int y = -kernelRadius; y <= kernelRadius; ++y) {
            distance = ((x * x) + (y * y)) / (2 * sigma * sigma);
            b = exp(-distance);
            value = a * b;
            sum += value;

            i = x + kernelRadius;
            j = y + kernelRadius;
            kernel[j + (i * kernelSize)] = value;
        }
    }

    // Normalization
    for (int i = 0; i < kernelSize * kernelSize; ++i)
        kernel[i] /= sum;

    return kernel;
}

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

    if (m_gaussianKernel.isEmpty())
        m_gaussianKernel = computeGaussianKernel(m_kernelRadius, 3.5);
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

    shaderCode.append(generateConstants(type));
    shaderCode.append(functionsCode);
    shaderCode.append("\n");
    shaderCode.append(getVariables(type));
    shaderCode.append("\n");

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
        if (line.contains("WILL BE GENERATED"))
            continue;
        line.remove(QRegExp("[\\s\n]*$"));
        shaderLines.append(line);
    }

    shaderFile.close();
    return shaderLines;
}

QStringList ShaderBuilder::generateConstants(QOpenGLShader::ShaderType type) const
{
    if (type == QOpenGLShader::Vertex)
        return QStringList();

    QStringList constants;
    constants.append(QString("const int GaussianKernelRadius = %0;").arg(QString::number(m_kernelRadius)));

    const int kernelSize = m_kernelRadius * 2 + 1;
    constants.append(QString("const vec4 GaussianKernel[%0] = vec4[%0](").arg(QString::number(kernelSize * kernelSize)));
    QStringList elements;
    foreach (float coef, m_gaussianKernel)
        elements.append(QString("vec4(%0)").arg(coef));
    constants.append(QString("\t%0);").arg(elements.join(", ")));

    constants.append(QString("const float pi = %0;").arg(QString::number(M_PI)));

    return constants;
}
