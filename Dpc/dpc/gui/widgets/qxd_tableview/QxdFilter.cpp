#include "QxdFilter.h"

QxdIFilter::QxdIFilter() :
	m_enabled(false)
{
}

QxdIFilter::~QxdIFilter()
{
}

void QxdIFilter::setEqualTo(const QList<QVariant> &list)
{
    if (list.isEmpty())
        return;

    reset();
    onSetEqualTo(list);
    setEnabled(true);
}

bool QxdIFilter::isEnabled() const
{
	return m_enabled;
}

void QxdIFilter::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

	m_enabled = enabled;
	if (!m_enabled)
		reset();
}
