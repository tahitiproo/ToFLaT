#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QStringList>
#include <QRegularExpression>
#include <QScrollArea>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Спутники и группировки (Qt/C++)");
    resize(1200, 800);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    // ЛЕВАЯ ПАНЕЛЬ: Ввод данных
    QWidget *leftWidget = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(new QLabel("Формат ввода:\nT=100\nn=4\nm=5\n<i j len start end>"));
    
    textInput = new QTextEdit(leftWidget);
    textInput->setText("T=100\nn=4\nm=5\n1 2 20 10 30\n1 3 20 20 40\n2 4 30 30 60\n3 4 10 50 60\n1 4 20 70 90");
    textInput->setStyleSheet("font-family: monospace; font-size: 14px;");
    leftLayout->addWidget(textInput);
    
    QPushButton *applyBtn = new QPushButton("Применить и обновить", leftWidget);
    applyBtn->setStyleSheet("padding: 8px; font-weight: bold;");
    leftLayout->addWidget(applyBtn);
    
    // ПРАВАЯ ПАНЕЛЬ: Управление и Визуализация
    QWidget *rightWidget = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    
    QHBoxLayout *controlLayout = new QHBoxLayout();
    playBtn = new QPushButton("▶ Play", rightWidget);
    playBtn->setFixedWidth(100);
    
    slider = new QSlider(Qt::Horizontal, rightWidget);
    slider->setRange(0, 100);
    
    controlLayout->addWidget(playBtn);
    controlLayout->addWidget(slider);
    rightLayout->addLayout(controlLayout);
    
    // Вертикальный ролл для двух визуализаторов
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical, rightWidget);
    
    // Добавляем QScrollArea для диаграммы Ганта, чтобы она могла расширяться при обилии пар!
    QScrollArea *scrollArea = new QScrollArea(rightSplitter);
    scrollArea->setWidgetResizable(true);
    ganttWidget = new GanttWidget(scrollArea);
    scrollArea->setWidget(ganttWidget);
    
    graphWidget = new GraphWidget(rightSplitter);
    
    rightSplitter->addWidget(scrollArea);
    rightSplitter->addWidget(graphWidget);
    rightSplitter->setSizes({400, 400}); // Распределяем пополам по высоте
    
    rightLayout->addWidget(rightSplitter, 1);
    
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes({300, 900}); // Ширина панелей
    
    setCentralWidget(splitter);

    // Таймер сигналы и слоты
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTick);
    connect(playBtn, &QPushButton::clicked, this, &MainWindow::togglePlay);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::applyData);
    connect(slider, &QSlider::valueChanged, ganttWidget, &GanttWidget::setCurrentTime);
    connect(slider, &QSlider::valueChanged, graphWidget, &GraphWidget::setCurrentTime);

    applyData();
}

void MainWindow::applyData() {
    QString text = textInput->toPlainText();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    int T = 100, n = 0, m = 0;
    std::vector<Interval> intervals;

    for (const QString& line : lines) {
        QString cleanLine = line.trimmed();
        
        // Удаляем угловые скобки (на случай если пользователь их скопипастил в форму)
        cleanLine.remove('<').remove('>');

        if (cleanLine.startsWith("T", Qt::CaseInsensitive) && cleanLine.contains("=")) {
            T = cleanLine.split("=").last().toInt();
        } else if (cleanLine.startsWith("n", Qt::CaseInsensitive) && cleanLine.contains("=")) {
            n = cleanLine.split("=").last().toInt();
        } else if (cleanLine.startsWith("m", Qt::CaseInsensitive) && cleanLine.contains("=")) {
            m = cleanLine.split("=").last().toInt();
        } else {
            QStringList parts = cleanLine.split(QRegularExpression("\\s+"));
            if (parts.size() >= 5) {
                Interval in;
                in.i = parts[0].toInt();
                in.j = parts[1].toInt();
                in.len = parts[2].toInt();
                in.start = parts[3].toInt();
                in.end = parts[4].toInt();
                intervals.push_back(in);
            }
        }
    }

    timer->stop();
    playBtn->setText("▶ Play");
    slider->setRange(0, T);
    slider->setValue(0);
    ganttWidget->setData(T, n, intervals);
    graphWidget->setData(T, n, intervals);
}

void MainWindow::togglePlay() {
    if (timer->isActive()) {
        timer->stop();
        playBtn->setText("▶ Play");
    } else {
        if (slider->value() >= slider->maximum()) slider->setValue(0);
        timer->start(350); // Вызываем тик каждые 350мс
        playBtn->setText("⏸ Pause");
    }
}

void MainWindow::onTick() {
    int current = slider->value();
    if (current >= slider->maximum()) {
        timer->stop();
        playBtn->setText("▶ Play");
    } else {
        slider->setValue(current + 2); // Можно менять шаг движения времени
    }
}
