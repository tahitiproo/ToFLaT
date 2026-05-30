#ifndef VISUALWIDGET_H
#define VISUALWIDGET_H

#include <QWidget>
#include <vector>
#include "types.h"

class VisualWidget : public QWidget {
    Q_OBJECT
public:
    explicit VisualWidget(QWidget *parent = nullptr);
    void setData(int T, int n, const std::vector<Interval>& intervals);

public slots:
    void setCurrentTime(int t);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int globalT = 100;
    int numSatellites = 0;
    std::vector<Interval> intervals;
    int currentTime = 0;
    
    void drawGantt(QPainter& painter, const QRect& rect);
    void drawNetwork(QPainter& painter, const QRect& rect);
};

#endif // VISUALWIDGET_H