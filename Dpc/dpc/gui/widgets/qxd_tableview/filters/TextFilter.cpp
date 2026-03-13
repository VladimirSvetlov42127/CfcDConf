#include "TextFilter.h"

#include <QModelIndex>

TextFilter::TextFilter() :
	m_exactMatch(false)
{
}

TextFilter::~TextFilter()
{
}

int TextFilter::type() const
{
    return Qxd::Text;
}

bool TextFilter::accept(const QModelIndex &index, int filterRole) const
{
    QString str = index.data().toString();
    if (!m_regExp.isValid())
		return str.contains(m_regExp);

	if (!exactMatch())
		return str.contains(m_pattern);

    return str == m_pattern;
}

void TextFilter::reset()
{
	m_exactMatch = false;
    m_regExp = QRegularExpression();
    m_pattern = QString();
}

void TextFilter::onSetEqualTo(const QList<QVariant> &list)
{
    m_pattern = list.first().toString();
    m_exactMatch = true;
}
