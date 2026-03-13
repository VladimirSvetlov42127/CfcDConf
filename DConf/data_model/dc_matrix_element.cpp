#include "dc_matrix_element.h"

DcMatrixElement::DcMatrixElement(int32_t src, int32_t dst, QString properties) :
	m_src(src), m_dst(dst)
{
	m_properties.fromJson(properties);
}

int32_t DcMatrixElement::src()
{
	return m_src;
}

int32_t DcMatrixElement::dst()
{
	return m_dst;
}

QString DcMatrixElement::property(QString propName)
{
	return m_properties.get(propName);
}

bool DcMatrixElement::updateProperty(QString name, QString value)
{
	if (name.isEmpty())
		return false;
	if (value.isEmpty())
		return false;

    m_properties.set(name, value);
	updatedb(); // generate update sql and push to sql queue
	return true;

}
