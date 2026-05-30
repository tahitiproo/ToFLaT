#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSlider>
#include <QPushButton>
#include <QTimer>
#include "VisualWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void applyData();
    void togglePlay();
    void onTick();

private:
    QTextEdit *textInput;
    VisualWidget *visualizer;
    QSlider *slider;
    QPushButton *playBtn;
    QTimer *timer;
};

#endif // MAINWINDOW_H