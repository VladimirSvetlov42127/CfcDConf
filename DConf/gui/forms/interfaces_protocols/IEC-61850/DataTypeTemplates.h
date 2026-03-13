#pragma once

#include <memory>
#include <qhash.h>

#include "XMLNode.h"

class BDA;
class DAType;
class DA;
class SDO;
class DOType;
class DO;
class LNodeType;
typedef std::shared_ptr<BDA> BDAPtr;
typedef std::shared_ptr<DAType> DATypePtr;
typedef std::shared_ptr<DA> DAPtr;
typedef std::shared_ptr<SDO> SDOPtr;
typedef std::shared_ptr<DOType> DOTypePtr;
typedef std::shared_ptr<DO> DOPtr;
typedef std::shared_ptr<LNodeType> LNodeTypePtr;

class BDA
{
public:
	BDA(const QString &name, const XMLNode &node);

	QString name() const { return m_name; }
	QString type() const;
	QString bType() const;
	QList<BDAPtr> childs() const { return m_childs; }
	void addChild(const BDAPtr &b) { m_childs.append(b); }

private:
	QString m_name;
	XMLNode m_node;
	QList<BDAPtr> m_childs;
};

//==========================================================================================
class DAType
{
public:
	DAType(const XMLNode &node);

	QString id() const;
	QList<BDAPtr> bdaList() const { return m_bdaList; }

private:
	XMLNode m_node;
	QList<BDAPtr> m_bdaList;
};

//==========================================================================================
class DA
{
public:
	DA(const QString &name, const XMLNode &node, const DATypePtr &type = DATypePtr());

	QString name() const { return m_name; }
	QString typeString() const;
	QString bType() const;
	QString fc() const;
	DATypePtr type() const { return m_type; }
	QList<DAPtr> childs() const { return m_childs; }
	void addChild(const DAPtr &da) { m_childs.append(da); }

private:
	QString m_name;
	XMLNode m_node;
	DATypePtr m_type;
	QList<DAPtr> m_childs;
};

//==========================================================================================
class SDO
{
public:
	SDO(const QString &name, const XMLNode &node);

	QString name() const { return m_name; }
	QString typeString() const;
	DOTypePtr type() const { return m_type; }
	void setType(const DOTypePtr &type) { m_type = type; }
	QList<SDOPtr> childs() const { return m_childs; }
	void addChild(const SDOPtr &sda) { m_childs.append(sda); }
	QStringList fcList() const;

private:
	QString m_name;
	XMLNode m_node;
	DOTypePtr m_type;
	QList<SDOPtr> m_childs;
};

//==========================================================================================
class DOType
{
public:
	DOType(const XMLNode &node, const QHash<QString, DATypePtr> &daTypes);

	QString id() const;
	QString cdc() const;
	QList<DAPtr> daList() const { return m_daList; }
	QList<SDOPtr> sdoList() const { return m_sdoList; }

private:
	XMLNode m_node;
	QList<DAPtr> m_daList;
	QList<SDOPtr> m_sdoList;
};

//==========================================================================================
class DO
{
public:
	DO(const XMLNode &node, const DOTypePtr &type);

	QString name() const;
	QString typeString() const;
	DOTypePtr type() const { return m_type; }
	QStringList fcList() const;

private:
	XMLNode m_node;
	DOTypePtr m_type;
};

//==========================================================================================
class LNodeType
{
public:
	LNodeType(const XMLNode &node, const QHash<QString, DOTypePtr> &doTypes);

	QString id() const;
	QList<DOPtr> doList() const { return m_doList; }
	DOPtr getDO(const QString &name) const;

private:
	XMLNode m_node;
	QList<DOPtr> m_doList;
};

//==========================================================================================
class DataTypeTemplates
{
public:
	DataTypeTemplates(const XMLNode &node);

	QHash<QString, DATypePtr> daTypes() const { return m_daTypes; }
	QHash<QString, DOTypePtr> doTypes() const { return m_doTypes; }
	QHash<QString, LNodeTypePtr> lnTypes() const { return m_lnTypes; }

private:
	XMLNode m_node;
	QHash<QString, DATypePtr> m_daTypes;
	QHash<QString, DOTypePtr> m_doTypes;
	QHash<QString, LNodeTypePtr> m_lnTypes;
};