#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <vector>
#include "types.h"

class GraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);
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
};

#endif // GRAPHWIDGET_H
