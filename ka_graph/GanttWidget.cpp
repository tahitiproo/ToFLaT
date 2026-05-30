#include "GanttWidget.h"
#include <QPainter>
#include <algorithm>

GanttWidget::GanttWidget(QWidget *parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void GanttWidget::setData(int T, int n, const std::vector<Interval>& data) {
    globalT = T;
    numSatellites = n;
    intervals = data;
    
    // Рассчитываем необходимую высоту для отрисовки абсолютно всех пар:
    int pairsCount = n * (n - 1) / 2;
    int minH = 60 + pairsCount * 40; // 60 на отступы, 40 на каждую строку пары
    setMinimumHeight(std::max(minH, 200)); 
    setMinimumWidth(500); 

    update();
}

void GanttWidget::setCurrentTime(int t) {
    currentTime = t;
    update();
}

void GanttWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (globalT <= 0 || numSatellites <= 0) return;
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#ffffff"));
    
    int paddingX = 80;
    int paddingY = 30;
    int workW = width() - paddingX * 2 - 20; // 20 для отступа справа
    if(workW <= 0) return;
    
    // Генерируем все n*(n-1)/2 пар
    std::vector<std::pair<int,int>> pairs;
    for (int i = 1; i <= numSatellites; ++i) {
        for (int j = i + 1; j <= numSatellites; ++j) {
            pairs.push_back({i, j});
        }
    }
    
    int rowH = 40;
    
    // Рисуем сетку 
    painter.setPen(QColor("#64748b"));
    painter.drawText(paddingX - 10, paddingY - 10, "0");
    painter.drawText(paddingX + workW - 10, paddingY - 10, QString::number(globalT));
    
    painter.setPen(Qt::black);
    for (size_t k = 0; k < pairs.size(); ++k) {
        int yOffset = paddingY + k * rowH;
        painter.setPen(Qt::black);
        painter.drawText(5, yOffset + rowH/2 + 5, QString("<a%1, b%2>").arg(pairs[k].first).arg(pairs[k].second));
        
        painter.setPen(QColor("#f1f5f9"));
        painter.drawLine(paddingX, yOffset, paddingX + workW, yOffset);
        painter.drawLine(paddingX, yOffset + rowH, paddingX + workW, yOffset + rowH);
    }
    
    painter.setPen(QColor("#cbd5e1"));
    painter.drawLine(paddingX, paddingY, paddingX, paddingY + pairs.size() * rowH);
    
    // Рисуем интервалы взаимодействия
    for (const auto& in : intervals) {
        int u = std::min(in.i, in.j);
        int v = std::max(in.i, in.j);
        auto it = std::find(pairs.begin(), pairs.end(), std::pair<int,int>(u, v));
        if (it == pairs.end()) continue;
        
        int idx = std::distance(pairs.begin(), it);
        bool active = (currentTime >= in.start && currentTime <= in.end);
        
        int x = paddingX + (in.start * workW) / globalT;
        int w = ((in.end - in.start) * workW) / globalT;
        int y = paddingY + idx * rowH + 8;
        int h = rowH - 16;
        
        painter.fillRect(x, y, w, h, active ? QColor("#2563eb") : QColor("#334155"));
    }
    
    // Рисуем ползунок текущего времени
    int curX = paddingX + (currentTime * workW) / globalT;
    painter.setPen(QPen(QColor("#e11d48"), 2));
    painter.drawLine(curX, paddingY - 5, curX, paddingY + pairs.size() * rowH + 5);
    painter.setPen(QColor("#e11d48"));
    painter.drawText(curX - 10, paddingY - 10, QString("t=%1").arg(currentTime));
}
