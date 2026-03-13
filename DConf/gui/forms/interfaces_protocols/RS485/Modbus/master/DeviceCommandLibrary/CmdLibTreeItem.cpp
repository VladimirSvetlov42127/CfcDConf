#include "CmdLibTreeItem.h"


CmdLibTreeItem::CmdLibTreeItem()
	: m_parentItem(nullptr), m_Command(nullptr), m_InitCommand(nullptr)
{
}

CmdLibTreeItem::CmdLibTreeItem(const QString &name, sp_Command command, CmdLibTreeItem * parentItem)
	: m_name(name), m_Command(command), m_parentItem(parentItem), m_InitCommand(nullptr)
{
}

CmdLibTreeItem::CmdLibTreeItem(bool isInit, const QString & name, sp_InitCommand command, CmdLibTreeItem * parentItem)
	: m_name(name), m_Command(nullptr), m_parentItem(parentItem), m_isInitCommand(true), m_InitCommand(command)
{
}

CmdLibTreeItem::~CmdLibTreeItem()
{
	qDeleteAll(m_childItems);	// Из документации Qt
}

void CmdLibTreeItem::appendChild(CmdLibTreeItem * child)
{
	if(child)
		m_childItems.append(child);
}

CmdLibTreeItem * CmdLibTreeItem::child(int row)
{
	if (row < 0 || row >= m_childItems.size())
		return nullptr;
	return m_childItems.at(row);
}

int CmdLibTreeItem::childCount() const
{
	return m_childItems.count();
}

int CmdLibTreeItem::columnCount() const
{
	return 1;
}

const QString & CmdLibTreeItem::data(int column) const
{
	return m_name;
}

int CmdLibTreeItem::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<CmdLibTreeItem*>(this));

	return 0;
}

CmdLibTreeItem * CmdLibTreeItem::parentItem()
{
	return m_parentItem;
}
