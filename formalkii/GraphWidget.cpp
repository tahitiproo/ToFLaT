#include "GraphWidget.h"
#include <QPainter>
#include <QColor>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void GraphWidget::setData(int T, int n, const std::vector<Interval>& data) {
    globalT = T;
    numSatellites = n;
    intervals = data;
    update();
}

void GraphWidget::setCurrentTime(int t) {
    currentTime = t;
    update();
}

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

void GraphWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (numSatellites <= 0) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#f8fafc"));

    UnionFind uf(numSatellites);
    std::vector<std::pair<int,int>> activeEdges;

    for (const auto& in : intervals) {
        if (currentTime >= in.start && currentTime <= in.end) {
            uf.unite(in.i - 1, in.j - 1);
            activeEdges.push_back({in.i - 1, in.j - 1});
        }
    }

    QColor palette[] = {QColor("#3b82f6"), QColor("#ef4444"), QColor("#10b981"), 
                        QColor("#f59e0b"), QColor("#8b5cf6"), QColor("#ec4899"), QColor("#06b6d4")};

    int cx = rect().left() + rect().width() / 2;
    int cy = rect().top() + rect().height() / 2;
    int R = std::min(rect().width(), rect().height()) / 2 - 40;

    std::vector<QPoint> pts(numSatellites);
    for (int i = 0; i < numSatellites; ++i) {
        double a = (i * 2.0 * M_PI / numSatellites) - M_PI / 2.0;
        pts[i] = QPoint(cx + R * cos(a), cy + R * sin(a));
    }

    // Соединения
    painter.setPen(QPen(QColor("#94a3b8"), 3, Qt::SolidLine, Qt::RoundCap));
    for (auto edge : activeEdges) {
        painter.drawLine(pts[edge.first], pts[edge.second]);
    }

    // Узлы
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
