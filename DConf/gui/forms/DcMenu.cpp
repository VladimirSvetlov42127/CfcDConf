#include "DcMenu.h"

DcMenu::DcMenu(const QString &iconPath, const QString &title, DcMenu *parent, int pos) :
	m_iconPath(iconPath),
	m_title(title)	
{
	if (parent)	parent->m_childs[pos] = this;
}

bool DcMenu::isAvailableFor(DcController * controller) const
{
	if (m_checkFunc) return m_checkFunc(controller);
	for (auto &it : m_childs) if (it.second->isAvailableFor(controller)) return true;
	return false;
}

void DcMenu::fillReport(DcIConfigReport * report) const
{
	if (m_fillReportFunc) m_fillReportFunc(report);
}
