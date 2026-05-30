#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QStringList>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LR(k) EFF Set Calculator (Qt/C++)");
    resize(900, 600);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    QWidget *leftWidget = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(new QLabel("Правила грамматики (левая_часть -> правая_часть | альтернатива):"));
    
    textInput = new QTextEdit(leftWidget);
    // Пример 3.8 из слайда 83
    textInput->setText("S -> A B\nA -> B a | eps\nB -> C b | C\nC -> c | eps");
    textInput->setStyleSheet("font-family: monospace; font-size: 14px;");
    leftLayout->addWidget(textInput);
    
    QHBoxLayout *kLayout = new QHBoxLayout();
    kLayout->addWidget(new QLabel("k="));
    spinK = new QSpinBox();
    spinK->setRange(1, 10);
    spinK->setValue(2);
    kLayout->addWidget(spinK);
    leftLayout->addLayout(kLayout);
    
    QHBoxLayout *qLayout = new QHBoxLayout();
    qLayout->addWidget(new QLabel("Цепочка α для EFF(α):"));
    queryInput = new QLineEdit("S");
    qLayout->addWidget(queryInput);
    leftLayout->addLayout(qLayout);
    
    QPushButton *calcBtn = new QPushButton("Вычислить EFF", leftWidget);
    calcBtn->setStyleSheet("padding: 8px; font-weight: bold; font-size: 14px;");
    leftLayout->addWidget(calcBtn);
    
    QWidget *rightWidget = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->addWidget(new QLabel("Результаты вычислений:"));
    
    textOutput = new QTextEdit(rightWidget);
    textOutput->setReadOnly(true);
    textOutput->setStyleSheet("font-family: monospace; font-size: 14px;");
    rightLayout->addWidget(textOutput);
    
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes({350, 550});
    
    setCentralWidget(splitter);

    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::calculate);

    // Вычисляем начальный пример при старте
    calculate();
}

void MainWindow::calculate() {
    QString text = textInput->toPlainText();
    
    grammar.k = spinK->value();
    grammar.parseRules(text.toStdString());
    grammar.computeFirstK();
    grammar.computeEffK();
    
    QString out = QString("k = %1\n\n").arg(grammar.k);
    
    out += "=== Множества FIRST_k ===\n";
    for (const auto& nt : grammar.nonTerminals) {
        out += QString::fromStdString(nt + " : " + formatWordSet(grammar.first_k[nt])) + "\n";
    }
    
    out += "\n=== Множества EFF_k (для нетерминалов) ===\n";
    for (const auto& nt : grammar.nonTerminals) {
        out += QString::fromStdString(nt + " : " + formatWordSet(grammar.eff_k[nt])) + "\n";
    }
    
    out += "\n=== Вычисление EFF_k(α) ===\n";
    QString qstr = queryInput->text().trimmed();
    if (!qstr.isEmpty()) {
        std::vector<std::string> seq;
        QStringList tokens = qstr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for(auto t : tokens) seq.push_back(t.toStdString());
        
        WordSet res = grammar.getEffKOfSeq(seq);
        out += QString::fromStdString("EFF_" + std::to_string(grammar.k) + "(" + qstr.toStdString() + ") = " + formatWordSet(res)) + "\n";
    }
    
    textOutput->setText(out);
}
