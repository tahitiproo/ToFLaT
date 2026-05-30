#include "GraphWidget.h"
#include <QPainter>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void GraphWidget::setData(const AppData& data) {
    appData = data;
    update();
}

void GraphWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (appData.n <= 0) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#f8fafc"));

    QColor palette[] = {
        QColor("#e2e8f0"), // 0 - unassigned (gray)
        QColor("#ef4444"), // 1 - red
        QColor("#3b82f6"), // 2 - blue
        QColor("#10b981"), // 3 - green
        QColor("#f59e0b"), // 4 - yellow/amber
        QColor("#8b5cf6"), // 5 - purple
        QColor("#ec4899"), // 6 - pink
        QColor("#06b6d4")  // 7 - cyan
    };

    int cx = rect().left() + rect().width() / 2;
    int cy = rect().top() + rect().height() / 2;
    int R = std::min(rect().width(), rect().height()) / 2 - 50;
    if (R < 10) R = 10;

    std::vector<QPoint> pts(appData.n + 1);
    for (int i = 1; i <= appData.n; ++i) {
        double a = ((i - 1) * 2.0 * M_PI / appData.n) - M_PI / 2.0;
        pts[i] = QPoint(cx + R * std::cos(a), cy + R * std::sin(a));
    }

    // Линии связей (дороги)
    painter.setPen(QPen(QColor("#94a3b8"), 2, Qt::SolidLine, Qt::RoundCap));
    for (const auto& edge : appData.edges) {
        painter.drawLine(pts[edge.u], pts[edge.v]);
        
        QPoint mid = (pts[edge.u] + pts[edge.v]) / 2;
        painter.setPen(QColor("#475569"));
        painter.drawText(mid.x() - 15, mid.y() - 10, 30, 20, Qt::AlignCenter, QString::number(edge.len));
        painter.setPen(QPen(QColor("#94a3b8"), 2, Qt::SolidLine, Qt::RoundCap));
    }

    // Узлы (города) - подсвечены цветами государств
    for (int i = 1; i <= appData.n; ++i) {
        int state = 0;
        if (i < appData.city_state.size()) {
            state = appData.city_state[i];
        }
        
        QColor color = (state >= 0 && state < 8) ? palette[state] : palette[1 + (state % 7)];
        painter.setBrush(color);
        painter.setPen(QPen(Qt::white, 3));

        painter.drawEllipse(pts[i], 22, 22);
        
        painter.setPen(state > 0 ? Qt::white : QColor("#475569"));
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(11);
        painter.setFont(font);
        painter.drawText(QRect(pts[i].x() - 22, pts[i].y() - 22, 44, 44), Qt::AlignCenter, QString::number(i));
    }
}
