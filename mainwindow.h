#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTimer;
class QSlider;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onRotateSliderReleased();
    void onRotateSliderMoved();
    void rotateObject();

private:
    Ui::MainWindow *m_ui;
    QTimer *m_rotateSliderTimer;
    QSlider *m_grabbedRotateSlider;

};

#endif // MAINWINDOW_H
