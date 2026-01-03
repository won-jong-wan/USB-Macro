#ifndef GRIDSCENE_H
#define GRIDSCENE_H



#include <QGraphicsScene>
#include <QPainter>
#include <cmath>





class GridScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GridScene(QObject *parent = nullptr);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
};

#endif // GRIDSCENE_H

