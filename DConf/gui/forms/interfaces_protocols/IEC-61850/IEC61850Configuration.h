#pragma once

#include <qobject.h>
#include <QDomDocument>

#include "Communication.h"
#include "IED.h"
#include "DataTypeTemplates.h"

class IEC61850Configuration;
typedef std::shared_ptr<IEC61850Configuration> IEC61850ConfigurationPtr;

class IEC61850Configuration : public QObject
{
	Q_OBJECT
public:
	IEC61850Configuration(QObject *parent = nullptr);
	~IEC61850Configuration();

	QString errorString() const { return m_error; }
	bool isLoaded() const { return m_isLoaded; }

	CommunicationPtr communication() const { return m_communication; }
	QList<IEDPtr> iedList() const { return m_iedList; }
	IEDPtr ied(const QString &iedName) const;

public slots:
	bool load(const QString &fileName);
	bool save(const QString &fileName) const;

private:
	void setError(const QString &err = QString()) const;

private:
	QDomDocument m_doc;
	mutable QString m_error;
	bool m_isLoaded;
	CommunicationPtr m_communication;
	QList<IEDPtr> m_iedList;
};