#ifndef GANTTWIDGET_H
#define GANTTWIDGET_H

#include <QWidget>
#include <vector>
#include "types.h"

class GanttWidget : public QWidget {
    Q_OBJECT
public:
    explicit GanttWidget(QWidget *parent = nullptr);
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

#endif // GANTTWIDGET_H
