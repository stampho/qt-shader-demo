#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_ui->objectListWidget->addItem(QString("Object 1"));
    m_ui->objectListWidget->addItem(QString("Object 2"));
    m_ui->objectListWidget->addItem(QString("Object 3"));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
