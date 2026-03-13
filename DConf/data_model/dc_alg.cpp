#include "dc_alg.h"

DcAlg::DcAlg(int32_t id, int32_t position, QString name) :
	m_id(id), m_position(position), m_name(name)
{
}

int32_t DcAlg::index() const
{
	return m_id;
}

int32_t DcAlg::position() const
{
	return m_position;
}

QString DcAlg::name() const
{
	return m_name;
}

bool DcAlg::updateName(QString newname)
{
	if (newname.isNull())
		return false;
	if (newname.isEmpty())
		return false;
	m_name = newname;
	updatedb();
	return true;
}
