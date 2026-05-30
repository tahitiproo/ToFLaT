#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include "GraphWidget.h"
#include "types.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void calculate();

private:
    QTextEdit *textInput;
    QTextEdit *textOutput;
    GraphWidget *graphWidget;
    
    AppData appData;
};

#endif // MAINWINDOW_H
