#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include "types.h"

class GraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);
    void setData(const AppData& data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    AppData appData;
};

#endif // GRAPHWIDGET_H
