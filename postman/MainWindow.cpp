#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QStringList>
#include <QRegularExpression>
#include <set>
#include <vector>
#include <stack>
#include <algorithm>

const int INF = 1e9;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Задача о китайском почтальоне (Qt/C++)");
    resize(1200, 700);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    
    QWidget *leftWidget = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(new QLabel("Формат ввода:\nn m\nсписок дорог (u v w)\n"));
    
    textInput = new QTextEdit(leftWidget);
    // Простой пример ненаправленного графа с нечетными узлами
    textInput->setText("4 5\n1 2 10\n1 3 15\n2 3 5\n3 4 8\n2 4 12"); 
    textInput->setStyleSheet("font-family: monospace; font-size: 14px;");
    leftLayout->addWidget(textInput);
    
    QPushButton *calcBtn = new QPushButton("Решить задачу", leftWidget);
    calcBtn->setStyleSheet("padding: 8px; font-weight: bold; font-size: 14px;");
    leftLayout->addWidget(calcBtn);
    
    leftLayout->addWidget(new QLabel("Результат:"));
    textOutput = new QTextEdit(leftWidget);
    textOutput->setReadOnly(true);
    leftLayout->addWidget(textOutput);
    
    QWidget *rightWidget = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    graphWidget = new GraphWidget(rightWidget);
    rightLayout->addWidget(graphWidget);
    
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setSizes({350, 850});
    
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
    
    std::vector<int> degree(appData.n + 1, 0);
    std::vector<std::vector<int>> dist(appData.n + 1, std::vector<int>(appData.n + 1, INF));
    std::vector<std::vector<int>> next_node(appData.n + 1, std::vector<int>(appData.n + 1, 0));
    
    for (int i = 1; i <= appData.n; ++i) {
        dist[i][i] = 0;
        next_node[i][i] = i;
    }
    
    for (int i = 0; i < appData.m; ++i) {
        if (ptr + 2 >= tokens.size()) break;
        Edge e;
        e.u = tokens[ptr++].toInt();
        e.v = tokens[ptr++].toInt();
        e.w = tokens[ptr++].toInt();
        appData.original_edges.push_back(e);
        appData.total_weight += e.w;
        
        degree[e.u]++;
        degree[e.v]++;
        
        if (e.w < dist[e.u][e.v]) {
            dist[e.u][e.v] = e.w;
            dist[e.v][e.u] = e.w;
            next_node[e.u][e.v] = e.v;
            next_node[e.v][e.u] = e.u;
        }
    }
    
    // Алгоритм Флойда-Уоршелла для поиска кратчайших путей
    for (int k = 1; k <= appData.n; ++k) {
        for (int i = 1; i <= appData.n; ++i) {
            for (int j = 1; j <= appData.n; ++j) {
                if (dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next_node[i][j] = next_node[i][k];
                }
            }
        }
    }
    
    std::vector<int> odds;
    for (int i = 1; i <= appData.n; ++i) {
        if (degree[i] % 2 != 0) odds.push_back(i);
    }
    
    appData.min_circuit_weight = appData.total_weight;
    
    if (!odds.empty()) {
        int K = odds.size();
        std::vector<int> dp(1 << K, INF);
        std::vector<int> parent(1 << K, -1);
        std::vector<std::pair<int, int>> matchEdge(1 << K);
        
        dp[0] = 0;
        for (int mask = 0; mask < (1 << K); ++mask) {
            if (dp[mask] == INF) continue;
            
            int first_unset = -1;
            for (int i = 0; i < K; ++i) {
                if (!(mask & (1 << i))) {
                    first_unset = i;
                    break;
                }
            }
            if (first_unset == -1) continue;
            
            for (int j = first_unset + 1; j < K; ++j) {
                if (!(mask & (1 << j))) {
                    int w = dist[odds[first_unset]][odds[j]];
                    if (w == INF) continue;
                    
                    int next_mask = mask | (1 << first_unset) | (1 << j);
                    if (dp[mask] + w < dp[next_mask]) {
                        dp[next_mask] = dp[mask] + w;
                        parent[next_mask] = mask;
                        matchEdge[next_mask] = {first_unset, j};
                    }
                }
            }
        }
        
        int end_mask = (1 << K) - 1;
        
        if (dp[end_mask] == INF) {
            // Граф имеет более одной компоненты связности с нечетными узлами
            appData.has_eulerian_circuit = false;
        } else {
            appData.min_circuit_weight += dp[end_mask];
            
            // Восстановление паросочетания
            int cur_mask = end_mask;
            while (cur_mask > 0) {
                auto m_edge = matchEdge[cur_mask];
                int u = odds[m_edge.first];
                int v = odds[m_edge.second];
                
                // Добавление кратчайшего пути к дублированным ребрам
                int curr = u;
                while (curr != v) {
                    int nxt = next_node[curr][v];
                    appData.duplicated_edges.push_back({curr, nxt});
                    curr = nxt;
                }
                
                cur_mask = parent[cur_mask];
            }
        }
    }
    
    std::multiset<std::pair<int, int>> edges;
    for (const auto& e : appData.original_edges) {
        edges.insert({e.u, e.v});
        edges.insert({e.v, e.u});
    }
    for (const auto& e : appData.duplicated_edges) {
        edges.insert({e.first, e.second});
        edges.insert({e.second, e.first});
    }
    
    // Алгоритм Иерахолцера для поиска Эйлерова цикла
    if (appData.has_eulerian_circuit) {
        std::stack<int> st;
        st.push(1); // Предполагаем связный граф от вершины 1
        
        while (!st.empty()) {
            int v = st.top();
            
            auto it = edges.lower_bound({v, 0});
            if (it != edges.end() && it->first == v) {
                int u = it->second;
                st.push(u);
                
                edges.erase(it);
                auto it2 = edges.find({u, v});
                if (it2 != edges.end()) edges.erase(it2);
            } else {
                appData.circuit.push_back(v);
                st.pop();
            }
        }
    }
    
    std::reverse(appData.circuit.begin(), appData.circuit.end());
    
    // Вывод текста
    QString res = QString("Граф: Вершин %1, Исходных ребер %2\n").arg(appData.n).arg(appData.original_edges.size());
    res += QString("Нечетных узлов: %1\n").arg(odds.size());
    res += QString("Длина исходных ребер: %1\n").arg(appData.total_weight);
    
    if (!appData.has_eulerian_circuit) {
        res += "Граф несвязный, обход невозможен.\n";
    } else {
        res += QString("Длина оптимального маршрута: %1\n").arg(appData.min_circuit_weight);
        res += "Оптимальный маршрут почтальона (Эйлеров цикл):\n";
        for (int i = 0; i < appData.circuit.size(); ++i) {
            res += QString::number(appData.circuit[i]);
            if (i < appData.circuit.size() - 1) res += " -> ";
        }
        res += "\n\nДобавленные (пройденные дважды) ребра:\n";
        if (appData.duplicated_edges.empty()) res += "Нет\n";
        for (auto p : appData.duplicated_edges) {
            res += QString("%1 - %2\n").arg(p.first).arg(p.second);
        }
    }
    textOutput->setText(res);
    
    graphWidget->setData(appData);
}
