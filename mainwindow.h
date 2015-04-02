#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidgetItem;
class QSlider;
class QTimer;

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
    void setAnimationSpeed(int speed);
    void onObjectSelected(QListWidgetItem *item);

private:
    void initObjectListWidget();
    void createConnections();

    Ui::MainWindow *m_ui;

    QTimer *m_rotateSliderTimer;
    QTimer *m_rotateAnimTimer;

    QSlider *m_grabbedRotateSlider;
};

#endif // MAINWINDOW_H
