#include "visualnode.h"
#include "edge.h"


VisualNode::VisualNode(CommandNode* data, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_data(data)
{
    // 필요하다면 여기서 툴팁 등을 설정할 수 있음
    setToolTip(m_data->getText());
}

QRectF VisualNode::boundingRect() const
{
    return QRectF(0, 0, m_width, m_height);
}

void VisualNode::addEdge(Edge* edge)
{
    m_edges.append(edge);
}

// ★★★ [노드가 움직일 때 선도 같이 움직이게 하는 핵심 로직] ★★★
QVariant VisualNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // 내 위치가 변하면, 나랑 연결된 모든 선(Edge)에게 "따라와!" 하고 명령
        for (Edge *edge : m_edges) {
            edge->adjust();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}


void VisualNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 1. 배경색 / 타입박스 색
    QColor bgColor;
    QColor typeColor;
    QString typeText;

    switch (m_data->getType()) {
    case NodeType::Type_S:
        bgColor = QColor("#DDEBFF");
        typeColor = QColor("#379CDB");
        typeText = "S";
        break;

    case NodeType::Type_C:
        bgColor = QColor("#E9FFE5");
        typeColor = QColor("#4CAF50");
        typeText = "C";
        break;

    case NodeType::Type_Delay:
        bgColor = QColor("#FFF3D9");
        typeColor = QColor("#FF9800");
        typeText = "D";
        break;
    }

    // 2. 메인 노드 박스
    painter->setBrush(bgColor);
    painter->setPen(QPen(QColor("#9A9A9A"), 3));
    painter->drawRect(0, 0, m_width, m_height);

    // 3. 타입 박스 (조그만 사각형)
    const int badgeW = 20;
    const int badgeH = 20;
    const int badgeX = 6;
    const int badgeY = 6;

    painter->setBrush(typeColor);
    painter->setPen(Qt::NoPen);
    painter->drawRect(badgeX, badgeY, badgeW, badgeH);

    // 4. 타입 텍스트 (S/C/D)
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10, QFont::Bold));

    QRect badgeRect(badgeX, badgeY, badgeW, badgeH);
    painter->drawText(badgeRect, Qt::AlignCenter, typeText);

    // 5. 노드 메인 텍스트
    painter->setPen(QColor("#1E1E1E"));
    painter->setFont(QFont("Arial", 11));

    const int padTopBottom = 8;
    const int padRight     = 20;

    // 타입 박스 위치/크기랑 맞춰서 텍스트 시작 위치를 오른쪽으로 밀기
    const int txbadgeX = 6;
    const int txbadgeW = 20;
    int textLeft = txbadgeX + txbadgeW + 8; // ← 여기 늘리면 더 멀어짐

    QRect textRect(
        textLeft,
        padTopBottom,
        m_width - textLeft - padRight,
        m_height - padTopBottom * 2
        );

    painter->drawText(textRect,
                      Qt::AlignVCenter | Qt::AlignLeft,
                      m_data->getText());
}

