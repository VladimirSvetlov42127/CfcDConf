#include "DcIConfigReport.h"

#include <qdebug.h>


DcIConfigReport::DcIConfigReport(DcController *device) :
	m_device(device)
{
}

DcController * DcIConfigReport::device() const
{
	return m_device;
}

bool DcIConfigReport::addSectionPrefix(const QString & prefix)
{
	if (prefix.isEmpty())
		return false;

	m_prefixs << prefix;
	return true;
}

QString DcIConfigReport::takeSecionPrefix()
{
	if (m_prefixs.isEmpty())
		return QString();

	return m_prefixs.takeLast();
}

QStringList DcIConfigReport::prefixs() const
{
	return m_prefixs;
}