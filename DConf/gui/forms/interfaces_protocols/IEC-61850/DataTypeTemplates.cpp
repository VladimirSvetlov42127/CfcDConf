#include "DataTypeTemplates.h"

#include <qset.h>
#include <functional>

#include "61850_defines.h"

namespace {
	void findFc(const DOType *doType, QSet<QString> &res);

	void findFc(const SDO *sdo, QSet<QString> &res)
	{
		for (auto &it : sdo->childs())
			findFc(it.get(), res);

		findFc(sdo->type().get(), res);
	}

	void findFc(const DOType *doType, QSet<QString> &res)
	{
		if (!doType)
			return;

		for (auto &it : doType->daList())
			res.insert(it->fc());
		for (auto &it : doType->sdoList())
			findFc(it.get(), res);
	}

} // namespace

BDA::BDA(const QString &name, const XMLNode &node) :
	m_name(name),
	m_node(node)
{
}

QString BDA::type() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Type);
}

QString BDA::bType() const
{
	return m_node.attribute(IEC61850::XML::ATTR::BType);
}

//==========================================================================================
DAType::DAType(const XMLNode & node) :
	m_node(node)
{
	for (auto &bda_node : m_node.childs(IEC61850::XML::TAG::BDA)) {
		auto bda = std::make_shared<BDA>(bda_node.attribute(IEC61850::XML::ATTR::Name), bda_node);
		m_bdaList.append(bda);
		int count = bda_node.attribute(IEC61850::XML::ATTR::Count).toInt();
		for (size_t i = 0; i < count; i++)
			bda->addChild(std::make_shared<BDA>(QString(), bda_node));
	}
}

QString DAType::id() const
{
	return m_node.attribute(IEC61850::XML::ATTR::ID);
}

//==========================================================================================
DA::DA(const QString &name, const XMLNode & node, const DATypePtr &type) :
	m_name(name),
	m_node(node),
	m_type(type)
{
}

QString DA::typeString() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Type);
}

QString DA::bType() const
{
	return m_node.attribute(IEC61850::XML::ATTR::BType);
}

QString DA::fc() const
{
	return m_node.attribute(IEC61850::XML::ATTR::FC);
}

//==========================================================================================
SDO::SDO(const QString & name, const XMLNode & node) :
	m_name(name),
	m_node(node)
{
}

QString SDO::typeString() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Type);
}

QStringList SDO::fcList() const
{
	QSet<QString> res;
	findFc(this, res);
	return QList<QString>(res.begin(), res.end());
}

//==========================================================================================
DOType::DOType(const XMLNode & node, const QHash<QString, DATypePtr> &daTypes) :
	m_node(node)
{
	for (auto sdoNode : m_node.childs(IEC61850::XML::TAG::SDO)) {
		QString name = sdoNode.attribute(IEC61850::XML::ATTR::Name);
		auto sdo= std::make_shared<SDO>(name, sdoNode);
		m_sdoList.append(sdo);
		int count = sdoNode.attribute(IEC61850::XML::ATTR::Count).toInt();
		for (size_t i = 0; i < count; i++)
			sdo->addChild(std::make_shared<SDO>(QString(), sdoNode));
	}

	for (auto &da_node : m_node.childs(IEC61850::XML::TAG::DA)) {
		QString name = da_node.attribute(IEC61850::XML::ATTR::Name);
		DATypePtr type = daTypes[da_node.attribute(IEC61850::XML::ATTR::Type)];
		int count = da_node.attribute(IEC61850::XML::ATTR::Count).toInt();
		auto da = std::make_shared<DA>(name, da_node, count == 0 ? type : DATypePtr());
		m_daList.append(da);		
		for (size_t i = 0; i < count; i++)
			da->addChild(std::make_shared<DA>(QString(), da_node, type));
	}
}

QString DOType::id() const
{
	return m_node.attribute(IEC61850::XML::ATTR::ID);
}
QString DOType::cdc() const
{
	return m_node.attribute(IEC61850::XML::ATTR::CDC);
}
//==========================================================================================
DO::DO(const XMLNode &node, const DOTypePtr &type) :
	m_node(node),
	m_type(type)
{
}

QString DO::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

QString DO::typeString() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Type);
}

QStringList DO::fcList() const
{
	QSet<QString> res;
	findFc(type().get(), res);
	return QList<QString>(res.begin(), res.end());
}

//==========================================================================================
LNodeType::LNodeType(const XMLNode &node, const QHash<QString, DOTypePtr> &doTypes) :
	m_node(node)
{
	for (auto &doNode : m_node.childs(IEC61850::XML::TAG::DO))
		m_doList.append(std::make_shared<DO>(doNode, doTypes[doNode.attribute(IEC61850::XML::ATTR::Type)]));
}

QString LNodeType::id() const
{
	return m_node.attribute(IEC61850::XML::ATTR::ID);
}

DOPtr LNodeType::getDO(const QString & name) const
{
	for (auto &it : m_doList)
		if (it->name() == name)
			return it;

	return DOPtr();
}

//==========================================================================================
DataTypeTemplates::DataTypeTemplates(const XMLNode & node) :
	m_node(node)
{
	for (auto &daTypeNode : m_node.childs(IEC61850::XML::TAG::DAType)) {
		auto daTypePtr = std::make_shared<DAType>(daTypeNode);
		m_daTypes[daTypePtr->id()] = daTypePtr;
	}

	std::function<void(const QList<BDAPtr> &)> replaceBDAType = [&](const QList<BDAPtr> &bdaList) {
		for (auto &bda : bdaList) {
			replaceBDAType(bda->childs());
			if (bda->bType() == IEC61850::XML::VALUE::Struct) {
				for (auto &it : m_daTypes[bda->type()]->bdaList())
					bda->addChild(it);
			}
		}
	};

	for (auto &daType : m_daTypes)
		replaceBDAType(daType->bdaList());

	for (auto &doTypeNode : m_node.childs(IEC61850::XML::TAG::DOType)) {
		auto doTypePtr = std::make_shared<DOType>(doTypeNode, m_daTypes);
		m_doTypes[doTypePtr->id()] = doTypePtr;
	}

	std::function<void(const QList<SDOPtr>&)> replaceSDOType = [&](const QList<SDOPtr> &list) {
		for (auto &sdo : list) {
			replaceSDOType(sdo->childs());
			if (sdo->childs().isEmpty())
				sdo->setType(m_doTypes[sdo->typeString()]);
		}
	};

	for (auto &doType : m_doTypes)
		replaceSDOType(doType->sdoList());

	for (auto &lnTypeNode : m_node.childs(IEC61850::XML::TAG::LNodeType)) {
		auto lnTypePtr = std::make_shared<LNodeType>(lnTypeNode, m_doTypes);
		m_lnTypes[lnTypePtr->id()] = lnTypePtr;
	}
}