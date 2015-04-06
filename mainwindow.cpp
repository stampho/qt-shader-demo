#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"
#include "globjectdescriptor.h"
#include "shadercodedialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_rotateSliderTimer(new QTimer(this))
    , m_rotateAnimTimer(new QTimer(this))
    , m_grabbedRotateSlider(0)
    , m_textureImagePath(":/images/qt-logo.png")
{
    m_ui->setupUi(this);

    m_ui->loadImageButton->setVisible(false);
    m_ui->triangleCountSB->setVisible(false);

    initObjectListWidget();
    initShaderConfig();
    createConnections();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::onRotateSliderReleased()
{
    QSlider *slider = dynamic_cast<QSlider *>(sender());
    m_rotateSliderTimer->stop();
    m_grabbedRotateSlider = 0;
    slider->setSliderPosition(0);
}

void MainWindow::onRotateSliderMoved()
{
    m_grabbedRotateSlider = dynamic_cast<QSlider *>(sender());
    m_rotateSliderTimer->start(50);
}

void MainWindow::rotateObject()
{
    const int timerId = dynamic_cast<QTimer *>(sender())->timerId();

    if (timerId == m_rotateSliderTimer->timerId()) {
        Axis::Axis axis = (m_grabbedRotateSlider->orientation() == Qt::Vertical) ? Axis::X : Axis::Y;
        m_ui->openGLWidget->rotate(m_grabbedRotateSlider->sliderPosition(), axis);
        return;
    }

    if (timerId == m_rotateAnimTimer->timerId()) {
        m_ui->openGLWidget->rotate(2, Axis::Y);
        return;
    }
}

void MainWindow::setAnimationSpeed(int speed)
{
    if (!speed) {
        m_rotateAnimTimer->stop();
        return;
    }

    int max = m_ui->objectAnimationSlider->maximum();
    m_rotateAnimTimer->start(max - speed);
}

void MainWindow::updateObjectDescriptor(QListWidgetItem *item)
{
    GLObjectDescriptor *objectDescriptor;

    if (!item)
        item = m_ui->objectListWidget->currentItem();

    switch(item->data(Qt::UserRole).toInt()) {
    case GLObjectDescriptor::ConeObject:
        m_ui->loadImageButton->setVisible(false);
        m_ui->triangleCountSB->setVisible(true);
        m_ui->shaderAnimCB->setEnabled(false);
        m_shaderConfig.animEnabled = false;
        m_ui->noneShaderRB->setEnabled(false);
        m_ui->gaussBlurRB->setEnabled(false);
        m_ui->sobelRB->setEnabled(false);
        m_ui->sobelGaussRB->setEnabled(false);
        m_ui->cannyRB->setEnabled(false);
        m_shaderConfig.imageProcessShader = ShaderConfig::None;
        objectDescriptor = GLObjectDescriptor::createConeDescriptor(&m_shaderConfig, m_ui->triangleCountSB->value());
        break;
    case GLObjectDescriptor::CubeObject:
        m_ui->loadImageButton->setVisible(false);
        m_ui->triangleCountSB->setVisible(false);
        m_ui->shaderAnimCB->setEnabled(false);
        m_shaderConfig.animEnabled = false;
        m_ui->noneShaderRB->setEnabled(false);
        m_ui->gaussBlurRB->setEnabled(false);
        m_ui->sobelRB->setEnabled(false);
        m_ui->sobelGaussRB->setEnabled(false);
        m_ui->cannyRB->setEnabled(false);
        m_shaderConfig.imageProcessShader = ShaderConfig::None;
        objectDescriptor = GLObjectDescriptor::createCubeDescriptor(&m_shaderConfig);
        break;
    case GLObjectDescriptor::ImageObject: {
        m_ui->loadImageButton->setVisible(true);
        m_ui->triangleCountSB->setVisible(false);
        m_ui->shaderAnimCB->setEnabled(true);
        m_ui->noneShaderRB->setEnabled(true);
        m_ui->gaussBlurRB->setEnabled(true);
        m_ui->sobelRB->setEnabled(true);
        m_ui->sobelGaussRB->setEnabled(true);
        m_ui->cannyRB->setEnabled(true);
        m_shaderConfig.imageProcessShader = getSelectedIPShader();
        m_shaderConfig.animEnabled = m_ui->shaderAnimCB->isChecked();
        objectDescriptor = GLObjectDescriptor::createImageDescriptor(&m_shaderConfig, m_textureImagePath);
        break;
    }
    case GLObjectDescriptor::None:
    default:
        objectDescriptor = 0;
        break;
    }

    if (objectDescriptor) {
        objectDescriptor->setCullFace(m_ui->cullFaceCB->isChecked());
        objectDescriptor->setPolygonLineMode(m_ui->polygonLineCB->isChecked());
    }

    m_ui->openGLWidget->updateObjectDescriptor(objectDescriptor);

    m_ui->shaderAnimationSlider->setEnabled(m_shaderConfig.animEnabled);
    if (m_shaderConfig.animEnabled)
        m_ui->openGLWidget->resetShaderAnimTimer(50);
}

void MainWindow::showImageBrowser()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("Images (*.bmp *.jpg *.png)");
    if (dialog.exec()) {
        m_textureImagePath = dialog.selectedFiles().first();
        updateObjectDescriptor();
    }
}

void MainWindow::showShaderCode()
{
    GLObjectDescriptor *objectDescriptor = m_ui->openGLWidget->getObjectDescriptor();

    ShaderCodeDialog dialog(this);
    QString code;

    if (sender() == m_ui->showVertexCodeButton) {
        dialog.setWindowTitle("Vertex CODE");
        if (objectDescriptor)
            code = objectDescriptor->getVertexShaderCode();
    } else {
        dialog.setWindowTitle("Fragment CODE");
        if (objectDescriptor)
            code = objectDescriptor->getFragmentShaderCode();
    }

    dialog.setContent(code);
    dialog.exec();
}

void MainWindow::updateShaderConfig()
{
    if (sender() == m_ui->shaderAnimCB) {
        m_shaderConfig.animEnabled = m_ui->shaderAnimCB->isChecked();
    } else if (sender() == m_ui->shaderGrayCB) {
        m_shaderConfig.gray = m_ui->shaderGrayCB->isChecked();
    } else if (sender() == m_ui->shaderInvertCB) {
        m_shaderConfig.invert = m_ui->shaderInvertCB->isChecked();
    } else if (sender() == m_ui->shaderThresholdCB) {
        m_shaderConfig.threshold = m_ui->shaderThresholdCB->isChecked();
    } else if(sender() == m_ui->shaderButtonGroup) {
        m_shaderConfig.imageProcessShader = getSelectedIPShader();
    }

    updateObjectDescriptor();
}

void MainWindow::initObjectListWidget()
{
    QListWidgetItem *coneItem = new QListWidgetItem("Cone", m_ui->objectListWidget);
    coneItem->setData(Qt::UserRole, GLObjectDescriptor::ConeObject);

    QListWidgetItem *cubeItem = new QListWidgetItem("Cube", m_ui->objectListWidget);
    cubeItem->setData(Qt::UserRole, GLObjectDescriptor::CubeObject);

    QListWidgetItem *imageItem = new QListWidgetItem("Image", m_ui->objectListWidget);
    imageItem->setData(Qt::UserRole, GLObjectDescriptor::ImageObject);

    connect(m_ui->objectListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateObjectDescriptor(QListWidgetItem*)));
}

void MainWindow::initShaderConfig()
{
    m_shaderConfig.animEnabled = false;
    m_ui->shaderAnimCB->setChecked(m_shaderConfig.animEnabled);
    m_ui->shaderAnimCB->setEnabled(false);
    connect(m_ui->shaderAnimCB, SIGNAL(toggled(bool)), this, SLOT(updateShaderConfig()));

    m_shaderConfig.gray = false;
    m_ui->shaderGrayCB->setChecked(m_shaderConfig.gray);
    m_ui->shaderGrayCB->setEnabled(true);
    connect(m_ui->shaderGrayCB, SIGNAL(toggled(bool)), this, SLOT(updateShaderConfig()));

    m_shaderConfig.invert = false;
    m_ui->shaderInvertCB->setChecked(m_shaderConfig.invert);
    m_ui->shaderInvertCB->setEnabled(true);
    connect(m_ui->shaderInvertCB, SIGNAL(toggled(bool)), this, SLOT(updateShaderConfig()));

    m_shaderConfig.threshold = false;
    m_ui->shaderThresholdCB->setChecked(m_shaderConfig.threshold);
    m_ui->shaderThresholdCB->setEnabled(true);
    connect(m_ui->shaderThresholdCB, SIGNAL(toggled(bool)), this, SLOT(updateShaderConfig()));

    m_shaderConfig.imageProcessShader = ShaderConfig::None;
    m_ui->noneShaderRB->setChecked(true);
    m_ui->noneShaderRB->setEnabled(false);
    m_ui->gaussBlurRB->setChecked(false);
    m_ui->gaussBlurRB->setEnabled(false);
    m_ui->sobelRB->setChecked(false);
    m_ui->sobelRB->setEnabled(false);
    m_ui->sobelGaussRB->setChecked(false);
    m_ui->sobelGaussRB->setEnabled(false);
    m_ui->cannyRB->setChecked(false);
    m_ui->cannyRB->setEnabled(false);
    connect(m_ui->shaderButtonGroup, SIGNAL(buttonToggled(QAbstractButton*,bool)), this, SLOT(updateShaderConfig()));
}

void MainWindow::createConnections()
{
    connect(m_ui->yRotateSlider, SIGNAL(sliderReleased()), this, SLOT(onRotateSliderReleased()));
    connect(m_ui->xRotateSlider, SIGNAL(sliderReleased()), this, SLOT(onRotateSliderReleased()));

    connect(m_ui->yRotateSlider, SIGNAL(sliderMoved(int)), this, SLOT(onRotateSliderMoved()));
    connect(m_ui->xRotateSlider, SIGNAL(sliderMoved(int)), this, SLOT(onRotateSliderMoved()));

    connect(m_rotateSliderTimer, SIGNAL(timeout()), this, SLOT(rotateObject()));
    connect(m_rotateAnimTimer, SIGNAL(timeout()), this, SLOT(rotateObject()));

    connect(m_ui->objectAnimationSlider, SIGNAL(valueChanged(int)), this, SLOT(setAnimationSpeed(int)));

    connect(m_ui->loadImageButton, SIGNAL(pressed()), this, SLOT(showImageBrowser()));
    connect(m_ui->showVertexCodeButton, SIGNAL(pressed()), this, SLOT(showShaderCode()));
    connect(m_ui->showFragmentCodeButton, SIGNAL(pressed()), this, SLOT(showShaderCode()));

    connect(m_ui->openGLWidget, SIGNAL(timerChangedShaderAnimProgress(int)), m_ui->shaderAnimationSlider, SLOT(setValue(int)));
    connect(m_ui->shaderAnimationSlider, SIGNAL(sliderMoved(int)), m_ui->openGLWidget, SLOT(setShaderAnimProgress(int)));

    connect(m_ui->cullFaceCB, SIGNAL(toggled(bool)), this, SLOT(updateObjectDescriptor()));
    connect(m_ui->polygonLineCB, SIGNAL(toggled(bool)), this, SLOT(updateObjectDescriptor()));
    connect(m_ui->triangleCountSB, SIGNAL(valueChanged(int)), this, SLOT(updateObjectDescriptor()));
}

ShaderConfig::IPShader MainWindow::getSelectedIPShader() const
{
    QAbstractButton *selected = m_ui->shaderButtonGroup->checkedButton();
    if (selected == m_ui->noneShaderRB)
        return ShaderConfig::None;
    if (selected == m_ui->gaussBlurRB)
        return ShaderConfig::Gauss;
    if (selected == m_ui->sobelRB)
        return ShaderConfig::Sobel;
    if (selected == m_ui->sobelGaussRB)
        return ShaderConfig::SobelGauss;
    if (selected == m_ui->cannyRB)
        return ShaderConfig::Canny;

    return ShaderConfig::None;
}
