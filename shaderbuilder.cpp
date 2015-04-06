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

void ShaderBuilder::setShaderConfig(ShaderConfig *shaderConfig)
{
    m_shaderConfig = shaderConfig;
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

    QString indent("\t");
    shaderCode.append("void main(void)");
    shaderCode.append("{");
    foreach (QString mainBodyLine, getMainBody(type)) {
        shaderCode.append(QString("%0%1").arg(indent, mainBodyLine));
    }

    if (type == QOpenGLShader::Fragment && m_shaderConfig) {
        if (m_shaderConfig->animEnabled) {
            shaderCode.append(QString("%0float progress = clamp(animProgress / 100.0, 0.0, 1.0);").arg(indent));
            shaderCode.append(QString("%0if (varyingTextureCoordinate.y > (1.0 - progress)) {").arg(indent));
            indent += "\t";
        }

        switch(m_shaderConfig->imageProcessShader) {
        case ShaderConfig::Gauss:
            shaderCode.append(QString("%0gl_FragColor = gaussBlur(texture, textureSize, varyingTextureCoordinate);").arg(indent));
            break;
        case ShaderConfig::Sobel:
            shaderCode.append(QString("%0gl_FragColor = sobel(texture, textureSize, varyingTextureCoordinate, false);").arg(indent));
            break;
        case ShaderConfig::SobelGauss:
            shaderCode.append(QString("%0gl_FragColor = sobel(texture, textureSize, varyingTextureCoordinate, true);").arg(indent));
            break;
        case ShaderConfig::Canny:
            shaderCode.append(QString("%0gl_FragColor = canny(texture, textureSize, varyingTextureCoordinate);").arg(indent));
            break;
        case ShaderConfig::None:
        default:
            break;
        }

        if (m_shaderConfig->gray)
            shaderCode.append(QString("%0gl_FragColor = gray(gl_FragColor);").arg(indent));

        if (m_shaderConfig->invert)
            shaderCode.append(QString("%0gl_FragColor = invert(gl_FragColor);").arg(indent));

        if (m_shaderConfig->threshold)
            shaderCode.append(QString("%0gl_FragColor = threshold(gl_FragColor, 0.5);").arg(indent));

    }
    if (type == QOpenGLShader::Fragment && m_shaderConfig && m_shaderConfig->animEnabled)
        shaderCode.append(QString("%0}").arg(indent));

    shaderCode.append("}"); // close main

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
