#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"
#include "globjectdescriptor.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_rotateSliderTimer(new QTimer(this))
    , m_rotateAnimTimer(new QTimer(this))
    , m_grabbedRotateSlider(0)
{
    m_ui->setupUi(this);

    initObjectListWidget();
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

void MainWindow::onObjectSelected(QListWidgetItem *item)
{
    GLObjectDescriptor *objectDescriptor;

    switch(item->data(Qt::UserRole).toInt()) {
    case GLObjectDescriptor::CubeObject:
        objectDescriptor = GLObjectDescriptor::createCubeDescriptor();
        break;
    case GLObjectDescriptor::ImageObject:
        objectDescriptor = GLObjectDescriptor::createImageDescriptor(":/images/qt-logo.png");
        //objectDescriptor = GLObjectDescriptor::createImageDescriptor(":/images/opengl-logo.png");
        break;
    case GLObjectDescriptor::None:
    default:
        objectDescriptor = 0;
        break;
    }

    m_ui->openGLWidget->updateObjectDescriptor(objectDescriptor);
    m_ui->openGLWidget->resetShaderAnimTimer(50);
}

void MainWindow::initObjectListWidget()
{
    QListWidgetItem *cubeItem = new QListWidgetItem("Cube", m_ui->objectListWidget);
    cubeItem->setData(Qt::UserRole, GLObjectDescriptor::CubeObject);

    QListWidgetItem *imageItem = new QListWidgetItem("Image", m_ui->objectListWidget);
    imageItem->setData(Qt::UserRole, GLObjectDescriptor::ImageObject);
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

    connect(m_ui->objectListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onObjectSelected(QListWidgetItem*)));
}
