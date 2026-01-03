#include "nodemanager.h"
#include "edge.h"
#include <QPen>

NodeManager::NodeManager(GridScene *scene, QObject *parent)
    : QObject(parent), m_scene(scene)
{
    // m_currentX 같은 건 필요 없습니다. 삭제.
}
void NodeManager::addCommand(NodeType type, QString text)
{
    // 1. 데이터 및 시각화 아이템 생성
    CommandNode* newData = new CommandNode(type, text);
    m_nodes.append(newData);
    VisualNode* newVisual = new VisualNode(newData);
    m_scene->addItem(newVisual);
    m_visualMap.insert(newData, newVisual);

    // 2. 첫 노드 처리
    if (m_nodes.size() == 1) {
        newVisual->setPos(START_X, START_Y);
        m_lastMainNode = newData;
    } else {
        // 부모 찾기
        VisualNode* parentVisual = m_visualMap[m_lastMainNode];
        QPointF parentPos = parentVisual->pos();
        Edge* newEdge = nullptr;

        if (type == NodeType::Type_C) {
            // [C 노드: 곁가지]
            newVisual->setPos(parentPos.x() + NODE_WIDTH + GAP_X, parentPos.y() + NODE_HEIGHT + GAP_Y);
            newEdge = new Edge(parentVisual, newVisual, true);

            // [가상 노드 생성]
            CommandNode* ghostNode = new CommandNode(NodeType::Type_S, "GHOST");
            m_nodes.append(ghostNode);

            VisualNode* ghostVisual = new VisualNode(ghostNode);

            // [수정 1] 위치 정렬: C 노드와 X축을 똑같이 맞춰야 다음 노드 간격이 일정함
            ghostVisual->setPos(newVisual->x(), parentPos.y());

            m_visualMap.insert(ghostNode, ghostVisual);

            // [중요] scene에 추가하지 않더라도 parent를 설정하지 않았으므로
            // 나중에 수동으로 delete 해줘야 함 (아래 clear 함수 참고)

            m_lastMainNode = ghostNode;
        } else {
            // [일반 노드: 메인 흐름]
            newVisual->setPos(parentPos.x() + NODE_WIDTH + GAP_X, parentPos.y());
            newEdge = new Edge(parentVisual, newVisual, false);

            // 기준점 업데이트
            m_lastMainNode = newData;
        }

        if (newEdge) {
            m_scene->addItem(newEdge);
            parentVisual->addEdge(newEdge);
            newVisual->addEdge(newEdge);
        }
    }

    // 씬 영역 자동 확장
    updateSceneRectFor(newVisual);
}

void NodeManager::updateSceneRect()
{
    QRectF bounds;

    for (auto visual : m_visualMap.values()) {
        bounds = bounds.united(visual->sceneBoundingRect());
    }

    // 100px 정도 여유 줌
    bounds.adjust(0, 0, 200, 100);

    m_scene->setSceneRect(bounds);
}

void NodeManager::updateSceneRectFor(VisualNode* v)
{
    QRectF r = v->sceneBoundingRect();
    QRectF old = m_scene->sceneRect();
    QRectF expanded = old.united(r.adjusted(0,0,200,200));
    m_scene->setSceneRect(expanded);
}

void NodeManager::clear()
{
    // 1) 씬에 올라간 모든 QGraphicsItem 제거 (VisualNode, Edge 다 같이 삭제)
    if (m_scene) {
        m_scene->clear();
    }

    // 2) CommandNode 동적 할당 해제
    qDeleteAll(m_nodes);   // <QtAlgorithms> 또는 Qt5에선 QList 포함으로 충분한 경우도 있음
    m_nodes.clear();

    // 3) 매핑 초기화
    m_visualMap.clear();

    // 4) 흐름 포인터 / 좌표 초기화
    m_lastMainNode = nullptr;
    m_currentX = 0.0;
}
