#include "dc_dir_node.h"

#include <QDebug>

DcDirNode::DcDirNode(int32_t id, const QString &name, DcNode *parent)
    : DcNode{id, name, parent}
{
}

DcDirNode::~DcDirNode()
{
    //    qDebug() << "~Deleting DcDirNode" << name();
}

QIcon DcDirNode::icon() const
{
    return QIcon(":/icons/folder.svg");
}
