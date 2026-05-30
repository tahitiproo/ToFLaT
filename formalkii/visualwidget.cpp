#include "VisualWidget.h"
#include <QPainter>
#include <QColor>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

VisualWidget::VisualWidget(QWidget *parent) : QWidget(parent) {}

void VisualWidget::setData(int T, int n, const std::vector<Interval>& data) {
    globalT = T;
    numSatellites = n;
    intervals = data;
    update();
}

void VisualWidget::setCurrentTime(int t) {
    currentTime = t;
    update();
}

// Алгоритм поиска компонент связности (Система непересекающихся множеств)
struct UnionFind {
    std::vector<int> parent;
    UnionFind(int n) {
        parent.resize(n);
        for (int k = 0; k < n; ++k) parent[k] = k;
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int rI = find(i), rJ = find(j);
        if (rI != rJ) parent[rI] = rJ;
    }
};

void VisualWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();
    int h1 = r.height() * 2 / 5;
    int h2 = r.height() - h1;
    
    QRect ganttRect(0, 0, r.width(), h1);
    QRect netRect(0, h1, r.width(), h2);

    painter.fillRect(ganttRect, QColor("#ffffff"));
    painter.fillRect(netRect, QColor("#f8fafc"));
    painter.setPen(QColor("#cbd5e1"));
    painter.drawLine(0, h1, r.width(), h1);

    drawGantt(painter, ganttRect);
    drawNetwork(painter, netRect);
}

void VisualWidget::drawGantt(QPainter& painter, const QRect& rect) {
    if (globalT <= 0 || intervals.empty()) return;
    
    int paddingX = 80;
    int paddingY = 30;
    int workW = rect.width() - paddingX * 2;
    int workH = rect.height() - paddingY * 2;
    if(workW <= 0 || workH <= 0) return;
    
    std::vector<std::pair<int,int>> pairs;
    for (const auto& in : intervals) {
        std::pair<int,int> p = {std::min(in.i, in.j), std::max(in.i, in.j)};
        if (std::find(pairs.begin(), pairs.end(), p) == pairs.end()) {
            pairs.push_back(p);
        }
    }
    
    int rowH = workH / std::max(1, (int)pairs.size());
    
    painter.setPen(Qt::black);
    for (size_t k = 0; k < pairs.size(); ++k) {
        int y = rect.top() + paddingY + k * rowH + rowH/2;
        painter.drawText(5, y + 5, QString("<a%1, b%2>").arg(pairs[k].first).arg(pairs[k].second));
        painter.setPen(QColor("#e2e8f0"));
        painter.drawLine(paddingX, y, paddingX + workW, y);
        painter.setPen(Qt::black);
    }
    
    for (const auto& in : intervals) {
        std::pair<int,int> p = {std::min(in.i, in.j), std::max(in.i, in.j)};
        auto it = std::find(pairs.begin(), pairs.end(), p);
        int idx = std::distance(pairs.begin(), it);
        
        bool active = (currentTime >= in.start && currentTime <= in.end);
        
        int x = paddingX + (in.start * workW) / globalT;
        int w = ((in.end - in.start) * workW) / globalT;
        int y = rect.top() + paddingY + idx * rowH + 6;
        int h = rowH - 12;
        
        painter.fillRect(x, y, w, h, active ? QColor("#2563eb") : QColor("#334155"));
    }
    
    // Красная линия трекера времени
    int curX = paddingX + (currentTime * workW) / globalT;
    painter.setPen(QPen(QColor("#e11d48"), 2));
    painter.drawLine(curX, rect.top() + 10, curX, rect.bottom() - 10);
}

void VisualWidget::drawNetwork(QPainter& painter, const QRect& rect) {
    if (numSatellites <= 0) return;

    UnionFind uf(numSatellites);
    std::vector<std::pair<int,int>> activeEdges;

    // Считаем активные связи в данный момент
    for (const auto& in : intervals) {
        if (currentTime >= in.start && currentTime <= in.end) {
            uf.unite(in.i - 1, in.j - 1);
            activeEdges.push_back({in.i - 1, in.j - 1});
        }
    }

    QColor palette[] = {QColor("#3b82f6"), QColor("#ef4444"), QColor("#10b981"), 
                        QColor("#f59e0b"), QColor("#8b5cf6"), QColor("#ec4899"), QColor("#06b6d4")};

    int cx = rect.left() + rect.width() / 2;
    int cy = rect.top() + rect.height() / 2;
    int R = std::min(rect.width(), rect.height()) / 2 - 50;

    std::vector<QPoint> pts(numSatellites);
    for (int i = 0; i < numSatellites; ++i) {
        double a = (i * 2.0 * M_PI / numSatellites) - M_PI / 2.0; // По кругу
        pts[i] = QPoint(cx + R * cos(a), cy + R * sin(a));
    }

    // Линии связей (Edges)
    painter.setPen(QPen(QColor("#94a3b8"), 3, Qt::SolidLine, Qt::RoundCap));
    for (auto edge : activeEdges) {
        painter.drawLine(pts[edge.first], pts[edge.second]);
    }

    // Узлы (Сателлиты)
    for (int i = 0; i < numSatellites; ++i) {
        int root = uf.find(i);
        bool hasGroup = false;
        
        for(auto e : activeEdges) {
            if(e.first == i || e.second == i) hasGroup = true;
        }

        QColor color = hasGroup ? palette[root % 7] : QColor("#e2e8f0");
        painter.setBrush(color);
        painter.setPen(QPen(Qt::white, 3));

        painter.drawEllipse(pts[i], 22, 22);
        
        painter.setPen(hasGroup ? Qt::white : QColor("#475569"));
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(11);
        painter.setFont(font);
        painter.drawText(QRect(pts[i].x() - 22, pts[i].y() - 22, 44, 44), Qt::AlignCenter, QString::number(i + 1));
    }
}