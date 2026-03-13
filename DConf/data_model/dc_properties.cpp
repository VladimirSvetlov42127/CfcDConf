#include "dc_properties.h"

#include <QJsonDocument>
#include <QJsonObject>

DcProperties::DcProperties()
{
}

bool DcProperties::operator!=(const DcProperties & other) const
{
    return m_container != other.m_container;
}

bool DcProperties::operator==(const DcProperties &other) const
{
    return m_container == other.m_container;
}

bool DcProperties::fromJson(const QString & jsondoc)
{
    m_container.clear();

	if (jsondoc.isEmpty())
		return true;

	QJsonDocument json = QJsonDocument::fromJson(jsondoc.toUtf8());
	if (json.isNull())
		return false;

	QJsonObject obj = json.object();
    for (auto it = obj.begin(); it != obj.end(); it++) {
		QString value;
		if (it.value().isBool())
			value = QString::number(it.value().toBool());
		else if (it.value().isDouble())
			value = QString::number(it.value().toDouble());
		else
			value = it.value().toString();

        m_container[it.key()] = value;
	}

	return true;
}

QString DcProperties::toJson() const
{
    if (m_container.empty())
        return QString();

    QJsonObject obj;
    for(auto &[key, value]: m_container) {
        obj.insert(key, value);
    }

    QJsonDocument doc(obj);
    QString result = doc.toJson(QJsonDocument::Compact);
    return result;
}

void DcProperties::set(const QString &name, const QString &value)
{
    if (name.isEmpty())
        return;

    m_container[name] = value;
}

QString DcProperties::get(const QString &name, const QString &defaultValue) const
{

    if (auto findIt = m_container.find(name); findIt != m_container.end())
        return findIt->second;

    return defaultValue;
}
