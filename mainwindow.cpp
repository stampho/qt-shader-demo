#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_rotateSliderTimer(new QTimer(this))
    , m_rotateAnimTimer(new QTimer(this))
    , m_grabbedRotateSlider(0)
{
    m_ui->setupUi(this);

    m_ui->objectListWidget->addItem(QString("Object 1"));
    m_ui->objectListWidget->addItem(QString("Object 2"));
    m_ui->objectListWidget->addItem(QString("Object 3"));

    connect(m_ui->yRotateSlider, SIGNAL(sliderReleased()), this, SLOT(onRotateSliderReleased()));
    connect(m_ui->xRotateSlider, SIGNAL(sliderReleased()), this, SLOT(onRotateSliderReleased()));

    connect(m_ui->yRotateSlider, SIGNAL(sliderMoved(int)), this, SLOT(onRotateSliderMoved()));
    connect(m_ui->xRotateSlider, SIGNAL(sliderMoved(int)), this, SLOT(onRotateSliderMoved()));

    connect(m_rotateSliderTimer, SIGNAL(timeout()), this, SLOT(rotateObject()));
    connect(m_rotateAnimTimer, SIGNAL(timeout()), this, SLOT(rotateObject()));

    connect(m_ui->objectAnimationSlider, SIGNAL(valueChanged(int)), this, SLOT(setAnimationSpeed(int)));
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
        Axis axis = (m_grabbedRotateSlider->orientation() == Qt::Vertical) ? X_AXIS : Y_AXIS;
        m_ui->openGLWidget->rotate(m_grabbedRotateSlider->sliderPosition(), axis);
        return;
    }

    if (timerId == m_rotateAnimTimer->timerId()) {
        m_ui->openGLWidget->rotate(2, Y_AXIS);
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
