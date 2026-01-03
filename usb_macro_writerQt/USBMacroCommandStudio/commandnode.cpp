#include "commandnode.h"
#include <QString>

CommandNode::CommandNode(NodeType type, QString text)
    : m_type(type), m_text(text)
{
}

NodeType CommandNode::getType() const
{
    return m_type;
}

QString CommandNode::getText() const
{
    return m_text;
}
