#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <QObject>
#include <QList> // 리스트 포함
#include <QMap>  // 시각적 매핑용은 남겨둠 (Data -> Visual 찾기용)
#include "gridscene.h"
#include "commandnode.h"
#include "visualnode.h"

class NodeManager : public QObject
{
    Q_OBJECT
public:
    explicit NodeManager(GridScene *scene, QObject *parent = nullptr);

    // ID 인자 삭제. 그냥 추가하면 끝.
    void addCommand(NodeType type, QString text);

    // 저장하거나 실행할 때 이 리스트를 갖다 쓰면 됨
    const QList<CommandNode*>& getAllNodes() const { return m_nodes; }

    void clear();
    void updateSceneRect();
    void updateSceneRectFor(VisualNode* v);

private:
    GridScene *m_scene;

    // 1. 전체 데이터를 순서대로 담는 리스트
    QList<CommandNode*> m_nodes;

    // 2. 화면 좌표 찾기용 매핑 (Data -> Visual)
    QMap<CommandNode*, VisualNode*> m_visualMap;

    // 흐름 제어용 포인터
    CommandNode *m_lastMainNode = nullptr; // 메인 트렁크(S)의 마지막

    // 배치 좌표
    qreal m_currentX = 0;
};

#endif // NODEMANAGER_H
