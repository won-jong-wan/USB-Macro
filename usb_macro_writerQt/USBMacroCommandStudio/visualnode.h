#ifndef VISUALNODE_H
#define VISUALNODE_H

#include <QGraphicsItem>
#include <QPainter>
#include <QVariant>
#include "commandnode.h" // 데이터 클래스를 알아야 함
#include "ui_config.h"
class Edge;
class VisualNode : public QGraphicsItem
{
public:
    // 생성자: 그릴 데이터(data)와 그래픽 부모(parent)를 받음
    VisualNode(CommandNode* data, QGraphicsItem *parent = nullptr);

    void addEdge(Edge* edge);

    // QGraphicsItem 필수 오버라이드
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;





protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
private:
    CommandNode* m_data; // 내가 그려야 할 데이터의 원본
    QList<Edge*> m_edges;
    qreal m_width = NODE_WIDTH;
    qreal m_height = NODE_HEIGHT;
};

#endif // VISUALNODE_H
