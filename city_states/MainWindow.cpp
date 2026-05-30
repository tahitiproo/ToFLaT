#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QStringList>
#include <QRegularExpression>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Города и дороги: Распределение по государствам (Qt/C++)");
    resize(1200, 800);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    // ЛЕВАЯ ПАНЕЛЬ: Ввод данных
    QWidget *leftWidget = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(new QLabel("Формат ввода:\nn m\nсписок дорог (u v len)\nk\nсписок столиц\n"));
    
    textInput = new QTextEdit(leftWidget);
    textInput->setText("6 7\n1 2 10\n1 3 20\n2 4 15\n3 4 10\n4 5 5\n5 6 12\n3 6 8\n2\n1 6");
    textInput->setStyleSheet("font-family: monospace; font-size: 14px;");
    leftLayout->addWidget(textInput);
    
    QPushButton *calcBtn = new QPushButton("Распределить города", leftWidget);
    calcBtn->setStyleSheet("padding: 8px; font-weight: bold; font-size: 14px;");
    leftLayout->addWidget(calcBtn);
    
    leftLayout->addWidget(new QLabel("Результаты распределения:"));
    textOutput = new QTextEdit(leftWidget);
    textOutput->setReadOnly(true);
    leftLayout->addWidget(textOutput);
    
    // ПРАВАЯ ПАНЕЛЬ: Визуализация
    QWidget *rightWidget = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical, rightWidget);
    
    graphWidget = new GraphWidget(rightSplitter);
    
    QWidget *matrixContainer = new QWidget(rightSplitter);
    QVBoxLayout *matrixLayout = new QVBoxLayout(matrixContainer);
    matrixLayout->addWidget(new QLabel("Матрица смежности (расстояния):"));
    matrixTable = new QTableWidget(matrixContainer);
    matrixLayout->addWidget(matrixTable);
    
    rightSplitter->addWidget(graphWidget);
    rightSplitter->addWidget(matrixContainer);
    rightSplitter->setSizes({500, 300}); 
    
    rightLayout->addWidget(rightSplitter, 1);
    
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes({300, 900});
    
    setCentralWidget(splitter);

    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::calculate);

    calculate();
}

void MainWindow::calculate() {
    QString text = textInput->toPlainText();
    QStringList tokens = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    
    if (tokens.size() < 2) return;
    
    appData = AppData();
    
    int ptr = 0;
    appData.n = tokens[ptr++].toInt();
    appData.m = tokens[ptr++].toInt();
    
    if (appData.n <= 0) return;
    
    for (int i = 0; i < appData.m; ++i) {
        if (ptr + 2 >= tokens.size()) break;
        Edge e;
        e.u = tokens[ptr++].toInt();
        e.v = tokens[ptr++].toInt();
        e.len = tokens[ptr++].toInt();
        appData.edges.push_back(e);
    }
    
    if (ptr < tokens.size()) {
        appData.k = tokens[ptr++].toInt();
        for (int i = 0; i < appData.k; ++i) {
            if (ptr < tokens.size()) {
                appData.capitals.push_back(tokens[ptr++].toInt());
            }
        }
    }
    
    // Алгоритм распределения
    int n = appData.n;
    int k = appData.k;
    appData.city_state.assign(n + 1, 0);
    
    std::vector<std::vector<std::pair<int, int>>> adj(n + 1);
    std::vector<std::vector<int>> matrix(n + 1, std::vector<int>(n + 1, -1));
    
    for (const auto& e : appData.edges) {
        adj[e.u].push_back({e.v, e.len});
        adj[e.v].push_back({e.u, e.len});
        matrix[e.u][e.v] = e.len;
        matrix[e.v][e.u] = e.len;
    }
    
    for (int i = 1; i <= n; ++i) matrix[i][i] = 0;
    
    std::vector<std::vector<int>> state_cities(k + 1);
    for (int i = 0; i < k && i < appData.capitals.size(); ++i) {
        int cap = appData.capitals[i];
        if (cap >= 1 && cap <= n) {
            appData.city_state[cap] = i + 1;
            state_cities[i + 1].push_back(cap);
        }
    }
    
    int unassigned_count = n - k;
    
    while(unassigned_count > 0) {
        bool any = false;
        for (int state = 1; state <= k; ++state) {
            int best_city = -1;
            int min_dist = 1e9;
            
            for (int u : state_cities[state]) {
                for (const auto& edge : adj[u]) {
                    int v = edge.first;
                    int len = edge.second;
                    if (appData.city_state[v] == 0) { // unassigned
                        if (len < min_dist) {
                            min_dist = len;
                            best_city = v;
                        }
                    }
                }
            }
            
            if (best_city != -1) {
                appData.city_state[best_city] = state;
                state_cities[state].push_back(best_city);
                unassigned_count--;
                any = true;
            }
            
            if (unassigned_count == 0) break;
        }
        
        if (!any && unassigned_count > 0) {
            break;
        }
    }
    
    // Вывод текста
    QString resultText = "";
    for (int state = 1; state <= k; ++state) {
        resultText += QString("Государство %1 (столица %2):\n").arg(state).arg(appData.capitals.size() >= state ? appData.capitals[state-1] : 0);
        resultText += "Города: ";
        for (int i = 0; i < state_cities[state].size(); ++i) {
            resultText += QString::number(state_cities[state][i]);
            if (i < state_cities[state].size() - 1) resultText += ", ";
        }
        resultText += "\n\n";
    }
    if (unassigned_count > 0) {
        resultText += "Внимание: некоторые города не связаны с государствами и остались нераспределенными!\n";
    }
    textOutput->setText(resultText);
    
    // Заполнение матрицы смежности
    matrixTable->setRowCount(n);
    matrixTable->setColumnCount(n);
    
    QStringList headers;
    for (int i = 1; i <= n; ++i) {
        headers << QString::number(i);
    }
    matrixTable->setHorizontalHeaderLabels(headers);
    matrixTable->setVerticalHeaderLabels(headers);
    
    QColor colors[] = {QColor("#ffffff"), QColor("#fecaca"), QColor("#bfdbfe"), QColor("#bbf7d0"), QColor("#fde68a"), QColor("#ddd6fe"), QColor("#fbcfe8"), QColor("#a5f3fc")};
    
    for(int i = 1; i <= n; ++i) {
        for(int j = 1; j <= n; ++j) {
            QString val = matrix[i][j] == -1 ? "-" : QString::number(matrix[i][j]);
            QTableWidgetItem *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            if (matrix[i][j] == -1 && i != j) {
                item->setForeground(QBrush(Qt::gray));
            }
            
            if (appData.city_state[i] != 0 && appData.city_state[i] == appData.city_state[j] && i != j) {
                int state = appData.city_state[i];
                QColor bg = (state > 0 && state < 8) ? colors[state] : Qt::white;
                item->setBackground(bg);
            }

            matrixTable->setItem(i - 1, j - 1, item);
        }
    }
    matrixTable->resizeColumnsToContents();
    
    graphWidget->setData(appData);
}
