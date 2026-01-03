#ifndef COMMANDNODE_H
#define COMMANDNODE_H
#include <QString>
#include <QGraphicsItem>
#include <QPainter>

enum class NodeType {
    Type_S,
    Type_C,
    Type_Delay
};



class CommandNode
{
public:
    CommandNode(NodeType type, QString text);

    NodeType getType() const;
    QString getText() const;


    // 트리 구조를 위한 포인터
    // S/Delay 타입용 (순차 연결)
    CommandNode* nextNode = nullptr;
    // C 타입용 (분기 연결)
    CommandNode* branchNode = nullptr;
private:
    NodeType m_type;
    QString m_text;

};

#endif // COMMANDNODE_H
