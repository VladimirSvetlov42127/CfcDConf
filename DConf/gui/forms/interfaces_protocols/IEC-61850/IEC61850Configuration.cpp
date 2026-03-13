#include "IEC61850Configuration.h"

#include <qdebug.h>
#include <qfile.h>
#include <QTime>

#include "XMLNode.h"
#include "61850_defines.h"

namespace {
} // namespace


IEC61850Configuration::IEC61850Configuration(QObject *parent) :
	QObject(parent),
	m_isLoaded(false)
{
}

IEC61850Configuration::~IEC61850Configuration()
{
	//qDebug() << Q_FUNC_INFO;
}

bool IEC61850Configuration::load(const QString &fileName)
{
	setError();
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		setError(file.errorString());
		return false;
	}

	QByteArray data = file.readAll();
	file.close();

	QDomDocument doc;
	QString error;
	int line;
	int column;
	if (!doc.setContent(data, &error, &line, &column)) {
		setError(QString("%1 (line: %2, column: %3)").arg(error).arg(line).arg(column));
		return false;
	}

	m_doc = doc;
	m_iedList.clear();

	DataTypeTemplates dataTypes(XMLNode(m_doc.elementsByTagName(IEC61850::XML::TAG::DataTypeTemplates).at(0).toElement()));
	m_communication = std::make_shared<Communication>(XMLNode(m_doc.elementsByTagName(IEC61850::XML::TAG::Communication).at(0).toElement()));
	auto iedChildList = m_doc.elementsByTagName(IEC61850::XML::TAG::IED);
	for (size_t i = 0; i < iedChildList.count(); i++)
		m_iedList << std::make_shared<IED>(XMLNode(iedChildList.at(i).toElement()), dataTypes, m_communication);

	m_isLoaded = true;
	return true;
}

IEDPtr IEC61850Configuration::ied(const QString & iedName) const
{
	for (auto &ied : m_iedList)
		if (ied->name() == iedName)
			return ied;

	return IEDPtr();
}

bool IEC61850Configuration::save(const QString &fileName) const
{
	setError();
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		setError(file.errorString());
		return false;
	}

	if (0 > file.write(m_doc.toByteArray())) {
		setError(file.errorString());
		return false;
	}

	return true;
}

void IEC61850Configuration::setError(const QString & err) const
{
	m_error = err;
}
