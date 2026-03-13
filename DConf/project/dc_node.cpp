#include "dc_node.h"

#include <QStringList>

DcNode::DcNode(int32_t id, const QString &name, DcNode *parent)
    : m_id{id}
    , m_parent{nullptr}
    , m_name{name}
{
    setParent(parent);
}

DcNode::~DcNode()
{
}

bool DcNode::isValid() const
{
    return id() && type() != NoType;
}

QIcon DcNode::icon() const
{
    return QIcon();
}

void DcNode::setName(const QString &name)
{
    if (name == m_name)
        return;

    m_name = name;
    emit nameChanged(name);
    emit fullTextChanged();
}

QString DcNode::fullText() const
{
    QStringList list;
    list.prepend(name());
    auto parent_node = parent();
    while (parent_node) {
        list.prepend(parent_node->name());
        parent_node = parent_node->parent();
    }

    return list.join(" | ");
}

DcNode *DcNode::parent()
{
    return m_parent;
}

const DcNode *DcNode::parent() const
{
    return m_parent;
}

size_t DcNode::position() const
{
    if (!parent())
        return 0;

    auto beginIt = parent()->m_childs.cbegin();
    auto endIt = parent()->m_childs.cend();
    auto it = std::find_if(beginIt, endIt, [this](const DcNode::UPtr &item) {
        return item.get() == this;
    });

    if (it != endIt)
        return std::distance(beginIt, it);

    // Не должно никогда происходить!!!
    return -1;
}

size_t DcNode::childsSize() const
{
    return m_childs.size();
}

DcNode *DcNode::child(size_t pos)
{
    return const_cast<DcNode*>( static_cast<const DcNode*>(this)->child(pos) );
}

const DcNode *DcNode::child(size_t pos) const
{
    if (pos >= m_childs.size())
        return nullptr;

    return m_childs[pos].get();
}

bool DcNode::insertChilds(size_t pos, DcNode::UPtrVector childs)
{
    if (!childs.size() || pos > m_childs.size())
        return false;

    m_childs.reserve(m_childs.size() + childs.size());
    for(size_t i = 0; i < childs.size(); ++i) {
        auto& node = childs[i];
        node->setParent(this);
        m_childs.insert(m_childs.cbegin() + pos + i, std::move(node));
    }

    return true;
}

DcNode::UPtrVector DcNode::takeChilds(size_t pos, size_t count)
{
    if (!count || pos + count > m_childs.size())
        return {};

    DcNode::UPtrVector list;
    list.reserve(count);
    for(size_t i = 0; i < count; ++i) {
        auto& child = m_childs[pos + i];
        child->setParent(nullptr);
        list.emplace_back(std::move(child));
    }

    m_childs.erase(m_childs.begin() + pos, m_childs.begin() + pos + count);
    return list;
}

bool DcNode::removeChild(size_t pos)
{
    m_childs.erase(m_childs.begin() + pos);
    return true;
}

int32_t DcNode::maxId() const
{
    int32_t max = id();
    for(auto &child: m_childs)
        max = std::max(max, child->maxId());

    return max;
}

void DcNode::setId(int32_t id)
{
    m_id = id;
}

void DcNode::setParent(DcNode *parent)
{
    if (parent == m_parent)
        return;

    if (m_parent) {
        disconnect(m_parent, &DcNode::fullTextChanged, this, &DcNode::fullTextChanged);
    }

    m_parent = parent;
    if (m_parent) {
        connect(m_parent, &DcNode::fullTextChanged, this, &DcNode::fullTextChanged);
    }
}
