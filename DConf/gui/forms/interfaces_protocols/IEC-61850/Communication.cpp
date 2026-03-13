#include "Communication.h"

#include <qdebug.h>

#include "61850_defines.h"

SMV::SMV(const XMLNode &smv) :
	m_smv(smv)
{
	auto childAddress = m_smv.childs(IEC61850::XML::TAG::Address);
	if (!childAddress.isEmpty()) {
		auto pChilds = childAddress.first().childs(IEC61850::XML::TAG::P);
		for (size_t i = 0; i < pChilds.count(); i++) {
			XMLNode n = pChilds.at(i);
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::MacAddress)
				m_macAddress = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::VlanId)
				m_vlanID = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::VlanPriority)
				m_vlanPriority = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::AppID)
				m_appID = n;
		}
	}
}

QString SMV::cbName() const
{
	return m_smv.attribute(IEC61850::XML::ATTR::CbName);
}

QString SMV::ldInst() const
{
	return m_smv.attribute(IEC61850::XML::ATTR::LdInst);
}

//==========================================================================================
GSE::GSE(const XMLNode &gse) :
	m_gse(gse)
{
	auto childAddress = m_gse.childs(IEC61850::XML::TAG::Address);
	if (!childAddress.isEmpty()) {
		m_address = childAddress.first();
		auto pChilds = m_address.childs(IEC61850::XML::TAG::P);
		for (size_t i = 0; i < pChilds.count(); i++) {
			XMLNode n = pChilds.at(i);
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::MacAddress)
				m_macAddress = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::VlanId)
				m_vlanID = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::VlanPriority)
				m_vlanPriority = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::AppID)
				m_appID = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::GoID)
				m_goID = n;
		}
	}

	auto minTimeChilds = m_gse.childs(IEC61850::XML::TAG::MinTime);
	if (!minTimeChilds.isEmpty())
		m_minTime = minTimeChilds.first();

	auto maxTimeChilds = m_gse.childs(IEC61850::XML::TAG::MaxTime);
	if (!maxTimeChilds.isEmpty())
		m_maxTime = maxTimeChilds.first();
}

QString GSE::cbName() const
{
	return m_gse.attribute(IEC61850::XML::ATTR::CbName);
}

QString GSE::ldInst() const
{
	return m_gse.attribute(IEC61850::XML::ATTR::LdInst);
}

void GSE::setAppID(const QString &id)
{
	if (!m_appID.isValid()) {
		m_appID = m_address.createChild(IEC61850::XML::TAG::P);
		m_appID.setAttribute(IEC61850::XML::ATTR::Type, IEC61850::XML::VALUE::AppID);
	}

	m_appID.setText(id);
}

//==========================================================================================
ConnectedAP::ConnectedAP(const XMLNode &node) :
	m_node(node)
{
	auto childAddress = m_node.childs(IEC61850::XML::TAG::Address);
	if (!childAddress.isEmpty()) {
		auto pChilds = childAddress.first().childs(IEC61850::XML::TAG::P);
		for (size_t i = 0; i < pChilds.count(); i++) {
			XMLNode n = pChilds.at(i);
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::Ip)
				m_ipAddress = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::Subnet)
				m_ipSubnet = n;
			if (n.attribute(IEC61850::XML::ATTR::Type) == IEC61850::XML::VALUE::Gateway)
				m_ipGateway = n;
		}
	}

	auto childsGSE = m_node.childs(IEC61850::XML::TAG::GSE);
	for (size_t i = 0; i < childsGSE.count(); i++)
		m_gseList.append(std::make_shared<GSE>(childsGSE.at(i)));

	auto childsSMV = m_node.childs(IEC61850::XML::TAG::SMV);
	for (size_t i = 0; i < childsSMV.count(); i++)
		m_smvList.append(std::make_shared<SMV>(childsSMV.at(i)));

	
	for (auto &c : m_node.comments()) {
		auto words = c.text().split(':');
		if (words.size() > 1) {
			for (auto smv : words.at(1).split(','))
				if (auto profile = smv.trimmed(); !profile.isEmpty())
					m_supportedSMVList << profile;
		}
	}
}

QString ConnectedAP::iedName() const
{
	return m_node.attribute(IEC61850::XML::ATTR::IedName);
}

void ConnectedAP::setIedName(const QString & name)
{
	m_node.setAttribute(IEC61850::XML::ATTR::IedName, name);
}

QString ConnectedAP::apName() const
{
	return m_node.attribute(IEC61850::XML::ATTR::ApName);
}

SMVPtr ConnectedAP::addSMV(const QString &cbName, const QString &ldInst)
{
	XMLNode smvNode = m_node.createChild(IEC61850::XML::TAG::SMV);
	smvNode.setAttribute(IEC61850::XML::ATTR::CbName, cbName);
	smvNode.setAttribute(IEC61850::XML::ATTR::LdInst, ldInst);

	auto addressNode = smvNode.createChild(IEC61850::XML::TAG::Address);

	auto macNode = addressNode.createChild(IEC61850::XML::TAG::P);
	macNode.setAttribute(IEC61850::XML::ATTR::Type, IEC61850::XML::VALUE::MacAddress);
	macNode.setText("01-0C-CD-04-00-01");

	auto vlanIdNode = addressNode.createChild(IEC61850::XML::TAG::P);
	vlanIdNode.setAttribute(IEC61850::XML::ATTR::Type, IEC61850::XML::VALUE::VlanId);
	vlanIdNode.setText("000");

	auto vlanPriorityNode = addressNode.createChild(IEC61850::XML::TAG::P);
	vlanPriorityNode.setAttribute(IEC61850::XML::ATTR::Type, IEC61850::XML::VALUE::VlanPriority);
	vlanPriorityNode.setText("4");

	auto appIdNode = addressNode.createChild(IEC61850::XML::TAG::P);
	appIdNode.setAttribute(IEC61850::XML::ATTR::Type, IEC61850::XML::VALUE::AppID);
	appIdNode.setText("4000");

	auto res = std::make_shared<SMV>(smvNode);
	m_smvList.append(res);
	return res;
}

void ConnectedAP::removeSMV(int idx)
{
	if (idx < 0 || idx >= m_smvList.size())
		return;

	m_node.removeChild(m_smvList.takeAt(idx)->node());
}

//==========================================================================================
SubNetwork::SubNetwork(const XMLNode & node) :
	m_node(node)
{
	auto connectedApChilds = m_node.childs(IEC61850::XML::TAG::ConnectedAP);
	for (auto &it : connectedApChilds)
		m_connectedAPList << std::make_shared<ConnectedAP>(it);
}

QString SubNetwork::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

QString SubNetwork::type() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Type);
}

//==========================================================================================
Communication::Communication(const XMLNode & node) :
	m_node(node)
{
	auto subnetwordChilds = m_node.childs(IEC61850::XML::TAG::SubNetwork);
	for (size_t i = 0; i < subnetwordChilds.count(); i++)
		m_subNetworkList.append(std::make_shared<SubNetwork>(subnetwordChilds.at(i)));
}

QList<GSEPtr> Communication::allGSEList() const
{
	QList<GSEPtr> result;
	for (auto &sn : m_subNetworkList)
		for (auto &cap : sn->connectedAPList())
			result.append(cap->gseList());

	return result;
}
