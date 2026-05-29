#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QSplitter>
#include <QStringList>
#include <QTextStream>
#include <QString>
#include <vector>
#include <algorithm>

struct Connection {
    int u, v;
    int len;
    int t1, t2;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Спутники и группировки (КА)");
    window.resize(900, 600);

    QWidget *central = new QWidget(&window);
    window.setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    // --- ЛЕВАЯ ПАНЕЛЬ (ВВОД ДАННЫХ) ---
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(new QLabel("Ввод данных:\n1 строка: n m (кол-во КА и связей)\nДалее связи: i j len [ti, tj]"));
    
    QTextEdit *inputText = new QTextEdit();
    // Тестовый пример
    inputText->setPlainText("4 4\n1 2 15 [0, 15]\n2 3 20 [5, 25]\n3 4 10 [10, 20]\n1 4 5 [20, 25]");
    leftLayout->addWidget(inputText);

    QPushButton *calcBtn = new QPushButton("Рассчитать группировки");
    leftLayout->addWidget(calcBtn);
    splitter->addWidget(leftWidget);

    // --- ПРАВАЯ ПАНЕЛЬ (ВЫВОД РЕЗУЛЬТАТОВ) ---
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical);
    
    // Блок для вывода текста (интервалы и списки)
    QWidget *consoleWidget = new QWidget();
    QVBoxLayout *consoleLayout = new QVBoxLayout(consoleWidget);
    consoleLayout->addWidget(new QLabel("Списки КА по тактам времени:"));
    QTextEdit *outputText = new QTextEdit();
    outputText->setReadOnly(true);
    consoleLayout->addWidget(outputText);
    rightSplitter->addWidget(consoleWidget);

    // Блок для матрицы смежности
    QWidget *matrixWidget = new QWidget();
    QVBoxLayout *matrixLayout = new QVBoxLayout(matrixWidget);
    matrixLayout->addWidget(new QLabel("Матрица смежности (временные интервалы):"));
    QTableWidget *tableWidget = new QTableWidget();
    matrixLayout->addWidget(tableWidget);
    rightSplitter->addWidget(matrixWidget);

    splitter->addWidget(rightSplitter);

    // --- ЛОГИКА ПРОГРАММЫ (По нажатию кнопки) ---
    QObject::connect(calcBtn, &QPushButton::clicked, [&]() {
        QString text = inputText->toPlainText();
        QTextStream stream(&text);
        
        int n = 0, m = 0;
        stream >> n >> m;
        
        if (n <= 0) {
            outputText->setPlainText("Ошибка: неверное количество КА.");
            return;
        }

        std::vector<Connection> conns;
        int max_t = 0;
        // Двумерный массив (матрица) для хранения строк-интервалов
        std::vector<std::vector<QString>> matrix(n, std::vector<QString>(n, ""));

        // Парсинг ввода (учитывая квадратные скобки и запятые)
        for (int k = 0; k < m; ++k) {
            int u, v, len, t1, t2;
            stream >> u >> v >> len;
            
            // Считываем как строки, чтобы вырезать лишние символы '[', ']', ','
            QString t1_str, t2_str;
            stream >> t1_str >> t2_str;
            t1 = t1_str.remove('[').remove(',').remove(']').toInt();
            t2 = t2_str.remove('[').remove(',').remove(']').toInt();

            conns.push_back({u, v, len, t1, t2});
            max_t = std::max(max_t, t2);
            
            // Записываем данные для матрицы
            if (u >= 1 && u <= n && v >= 1 && v <= n) {
                QString interval = QString("[%1, %2]").arg(t1).arg(t2);
                if (!matrix[u-1][v-1].isEmpty()) matrix[u-1][v-1] += "\n";
                matrix[u-1][v-1] += interval;
                if (!matrix[v-1][u-1].isEmpty()) matrix[v-1][u-1] += "\n";
                matrix[v-1][u-1] += interval;
            }
        }

        // Заполнение QTableWidget (Визуальная матрица)
        tableWidget->setRowCount(n);
        tableWidget->setColumnCount(n);
        QStringList headers;
        for (int i = 1; i <= n; ++i) headers << QString("КА %1").arg(i);
        tableWidget->setHorizontalHeaderLabels(headers);
        tableWidget->setVerticalHeaderLabels(headers);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                tableWidget->setItem(i, j, new QTableWidgetItem(matrix[i][j]));
            }
        }
        tableWidget->resizeColumnsToContents();

        // Расчет группировок (шаг 10 минут)
        QString outStr;
        QTextStream out(&outStr);
        out << "Глобальный интервал T = [0, " << max_t << "]\n";
        out << "Алгоритм: шаг 10 мин.\n\n";

        for (int t = 0; t < max_t; t += 10) {
            int current_start = t;
            int current_end = t + 10;
            out << "Такт времени [" << current_start << ", " << current_end << "]:\n";

            // Строим граф связей конкретно для этого такта времени
            std::vector<std::vector<int>> adj(n);
            for (const auto& c : conns) {
                // Если интервал связи пересекается с текущим тактом времени
                if (c.t1 < current_end && c.t2 > current_start) {
                    if (c.u >= 1 && c.u <= n && c.v >= 1 && c.v <= n) {
                        adj[c.u - 1].push_back(c.v - 1);
                        adj[c.v - 1].push_back(c.u - 1);
                    }
                }
            }

            // Поиск компонент связности (алгоритм обхода в глубину - DFS)
            std::vector<bool> visited(n, false);
            std::vector<std::vector<int>> components;

            for (int i = 0; i < n; ++i) {
                if (!visited[i]) {
                    std::vector<int> comp;
                    std::vector<int> stack = {i};
                    visited[i] = true;

                    while (!stack.empty()) {
                        int curr = stack.back();
                        stack.pop_back();
                        // Сохраняем номера КА (сдвигаем +1, чтобы было от 1 до N)
                        comp.push_back(curr + 1); 

                        for (int neighbor : adj[curr]) {
                            if (!visited[neighbor]) {
                                visited[neighbor] = true;
                                stack.push_back(neighbor);
                            }
                        }
                    }
                    std::sort(comp.begin(), comp.end());
                    components.push_back(comp);
                }
            }

            // Выводим сгруппированные списки
            out << "  Группировки:\n";
            for (size_t c = 0; c < components.size(); ++c) {
                out << "    Группа " << c + 1 << ": {";
                for (size_t i = 0; i < components[c].size(); ++i) {
                    out << components[c][i] << (i + 1 == components[c].size() ? "" : ", ");
                }
                out << "}\n";
            }
            out << "--------------------------------\n";
        }
        outputText->setPlainText(outStr);
    });

    window.show();
    return app.exec();
}