#ifndef EDGE_H
#define EDGE_H
#include "visualnode.h"
#include <QGraphicsPathItem>
#include <QPen>
#include "ui_config.h"
// Edge.h (최종 수정 버전)
// 두 VisualNode를 연결하고, 노드가 움직일 때 따라 움직이는 선




class Edge : public QGraphicsPathItem {
public:
    Edge(VisualNode* source, VisualNode* dest, bool isBranch)
        : sourceNode(source), destNode(dest), m_isBranch(isBranch) {
        updateStyle();
        setZValue(-1); // 노드보다 뒤에 그려지게 설정
        adjust();      // 초기 위치 계산
    }

    void adjust() {
        if (!sourceNode || !destNode) return;

        QRectF srcRect = sourceNode->boundingRect();
        QRectF dstRect = destNode->boundingRect();

        if (m_isBranch) {
            // [Branch 타입: 부모 -> C 노드 상단 중앙 수직 연결]

            // 1. 시작점 (s1): 부모 노드의 오른쪽 중앙 (메인 라인 높이)
            QPointF s1 = sourceNode->scenePos() + QPointF(srcRect.width(), srcRect.height() / 2);

            // 2. 메인 라인 끝점 (e1): 유령 노드 너비까지 고려하여 길게 그음
            //    이전 대화에서 끊김 현상을 막기 위해 적용한 로직입니다.
            QPointF e1 = s1 + QPointF(GAP_X + NODE_WIDTH, 0);

            // 3. 자식 노드 진입점 (e2): ★ C 노드의 '상단 중앙'으로 변경 ★
            QPointF e2 = destNode->scenePos() + QPointF(dstRect.width() / 2, 0);

            // 4. 분기점 (m): ★ 메인 라인 상에서 e2와 X 좌표가 같은 지점 ★
            //    이렇게 해야 수직으로 내려오는 선이 됩니다.
            QPointF m(e2.x(), s1.y());

            QPainterPath path(s1);
            path.lineTo(e1); // 메인 라인 그리기 (s1 -> e1)

            path.moveTo(m);  // 펜을 분기점으로 이동
            path.lineTo(e2); // 분기점 -> C 노드 상단 중앙 (수직선)

            setPath(path);
        }
        else {
            // [Next 타입: 일반 연결] (기존과 동일)
            QPointF start = sourceNode->scenePos() + QPointF(srcRect.width(), srcRect.height() / 2);
            QPointF end   = destNode->scenePos()   + QPointF(0, dstRect.height() / 2);

            QPainterPath path(start);
            path.lineTo(end);
            setPath(path);
        }
    }

    void updateStyle() {
        // Branch는 점선, 나머지는 실선으로 설정 (취향껏 변경 가능)
        if (m_isBranch) setPen(QPen(Qt::darkGray, 3, Qt::SolidLine));
        else            setPen(QPen(Qt::darkGray, 3, Qt::SolidLine));
    }

private:
    VisualNode* sourceNode;
    VisualNode* destNode;
    bool m_isBranch;
};
#endif // EDGE_H
