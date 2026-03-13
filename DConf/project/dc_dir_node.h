#ifndef DCDIRNODE_H
#define DCDIRNODE_H

#include "project/dc_node.h"

// DcDirNode - Узел дерева в иерархической структуре дерева проектов соответствующий "Папка" в проекте

class DcDirNode : public DcNode
{
public:
    DcDirNode(int32_t id, const QString &name, DcNode* parent = nullptr);
    virtual ~DcDirNode();

    DcNode::Type type() const override { return DcNode::DirType; }
    QIcon icon() const override;

private:
};

#endif // DCDIRNODE_H
