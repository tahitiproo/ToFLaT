#include "GraphWidget.h"
#include <QPainter>
#include <cmath>
#include <QPainterPath>

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
    
    int cx = rect().left() + rect().width() / 2;
    int cy = rect().top() + rect().height() / 2;
    int R = std::min(rect().width(), rect().height()) / 2 - 50;
    if (R < 10) R = 10;

    std::vector<QPoint> pts(appData.n + 1);
    for (int i = 1; i <= appData.n; ++i) {
        double a = ((i - 1) * 2.0 * M_PI / appData.n) - M_PI / 2.0;
        pts[i] = QPoint(cx + R * std::cos(a), cy + R * std::sin(a));
    }

    // Отрисовка исходных ребер
    painter.setPen(QPen(QColor("#94a3b8"), 2, Qt::SolidLine, Qt::RoundCap));
    for (const auto& edge : appData.original_edges) {
        painter.drawLine(pts[edge.u], pts[edge.v]);
    }
    
    // Отрисовка продублированных ребер
    painter.setPen(QPen(QColor("#e11d48"), 2, Qt::DashLine, Qt::RoundCap));
    for (const auto& edge : appData.duplicated_edges) {
        QPainterPath path;
        path.moveTo(pts[edge.first]);
        QPoint mid = (pts[edge.first] + pts[edge.second]) / 2;
        
        int dx = pts[edge.second].x() - pts[edge.first].x();
        int dy = pts[edge.second].y() - pts[edge.first].y();
        double len = std::hypot(dx, dy);
        int nx = -dy * 20 / len; 
        int ny = dx * 20 / len;
        
        QPoint ctrlPoint(mid.x() + nx, mid.y() + ny);
        path.quadTo(ctrlPoint, pts[edge.second]);
        
        painter.drawPath(path);
    }
    
    // Текст весов
    for (const auto& edge : appData.original_edges) {
        QPoint mid = (pts[edge.u] + pts[edge.v]) / 2;
        painter.setPen(QColor("#1e293b"));
        painter.drawText(mid.x() - 15, mid.y() - 10, 30, 20, Qt::AlignCenter, QString::number(edge.w));
    }

    // Узлы
    for (int i = 1; i <= appData.n; ++i) {
        painter.setBrush(QColor("#3b82f6"));
        painter.setPen(QPen(Qt::white, 3));
        painter.drawEllipse(pts[i], 22, 22);
        
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(11);
        painter.setFont(font);
        painter.drawText(QRect(pts[i].x() - 22, pts[i].y() - 22, 44, 44), Qt::AlignCenter, QString::number(i));
    }
}
