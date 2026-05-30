#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include "GrammarCore.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void calculate();

private:
    QTextEdit *textInput;
    QSpinBox *spinK;
    QLineEdit *queryInput;
    QTextEdit *textOutput;
    
    Grammar grammar;
};

#endif // MAINWINDOW_H
