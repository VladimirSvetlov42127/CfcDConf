#pragma once

#include <memory>

#include "XMLNode.h"

class ConnectedAP;
class Communication;
class GSE;
class SubNetwork;
class SMV;
typedef std::shared_ptr<Communication> CommunicationPtr;
typedef std::shared_ptr<ConnectedAP> ConnectedAPPtr;
typedef std::shared_ptr<SubNetwork> SubNetworkPtr;
typedef std::shared_ptr<GSE> GSEPtr;
typedef std::shared_ptr<SMV> SMVPtr;

class SMV 
{
public: 
	SMV(const XMLNode &smv);

	XMLNode node() const { return m_smv; }
	QString cbName() const;
	QString ldInst() const;

	QString macAddress() const { return m_macAddress.text(); }
	void setMacAddress(const QString &addr) { m_macAddress.setText(addr); }

	QString vlanId() const { return m_vlanID.text(); }
	void setVlanId(const QString &id) { m_vlanID.setText(id); }

	QString vlanPriority() const { return m_vlanPriority.text(); }
	void setVlanPriority(const QString &priority) { m_vlanPriority.setText(priority); }

	QString appID() const { return m_appID.text(); }
	void setAppID(const QString &id) { m_appID.setText(id); }

private:
	XMLNode m_smv;
	XMLNode m_macAddress;
	XMLNode m_vlanID;
	XMLNode m_vlanPriority;
	XMLNode m_appID;
};

//==========================================================================================
class GSE 
{
public:
	GSE(const XMLNode &gse);

	QString cbName() const;
	QString ldInst() const;

	QString macAddress() const { return m_macAddress.text(); }
	void setMacAddress(const QString &addr) { m_macAddress.setText(addr); }

	QString vlanId() const { return m_vlanID.text(); }
	void setVlanId(const QString &id) { m_vlanID.setText(id); }

	QString vlanPriority() const { return m_vlanPriority.text(); }
	void setVlanPriority(const QString &priority) { m_vlanPriority.setText(priority); }

	QString minTime() const { return m_minTime.text(); }
	void setMinTime(const QString &time) { m_minTime.setText(time); }

	QString maxTime() const { return m_maxTime.text(); }
	void setMaxTime(const QString &time) { m_maxTime.setText(time); }

	QString appID() const { return m_appID.text(); }
	void setAppID(const QString &id);// { m_appID.setText(id); }

	QString goID() const { return m_goID.text(); }
	void setGoID(const QString &id) { m_goID.setText(id); }

private:
	XMLNode m_gse;
	XMLNode m_address;
	XMLNode m_macAddress;
	XMLNode m_vlanID;
	XMLNode m_vlanPriority;
	XMLNode m_appID;
	XMLNode m_goID;
	XMLNode m_minTime;
	XMLNode m_maxTime;	
};

//==========================================================================================
class ConnectedAP
{
public:
	ConnectedAP(const XMLNode &node);

	QString iedName() const;
	void setIedName(const QString &name);

	QString apName() const;

	QString ipAddress() const { return m_ipAddress.text(); }
	void setIpAddress(const QString &ip) { m_ipAddress.setText(ip); }

	QString subnetAddress() const { return m_ipSubnet.text(); }
	void setSubnetAddress(const QString &ip) { m_ipSubnet.setText(ip); }

	QString gatewayAddress() const { return m_ipGateway.text(); }
	void setGatewayAddress(const QString &ip) { m_ipGateway.setText(ip); }

	QList<GSEPtr> gseList() const { return m_gseList; }

	QList<SMVPtr> smvList() const { return m_smvList; }

	SMVPtr addSMV(const QString &cbName, const QString &ldInst);
	void removeSMV(int idx);

	QStringList supportedSMV() const { return m_supportedSMVList; }

private:
	XMLNode m_node;
	XMLNode m_ipAddress;
	XMLNode m_ipSubnet;
	XMLNode m_ipGateway;
	QList<GSEPtr> m_gseList;
	QList<SMVPtr> m_smvList;
	QStringList m_supportedSMVList;
};

//==========================================================================================
class SubNetwork
{
public:
	SubNetwork(const XMLNode &node);
	QList<ConnectedAPPtr> connectedAPList() const { return m_connectedAPList; }

	QString name() const;
	QString type() const;

private:
	XMLNode m_node;
	QList<ConnectedAPPtr> m_connectedAPList;
};

//==========================================================================================
class Communication
{
public:
	Communication(const XMLNode &node);	
	QList<SubNetworkPtr> subNetworkList() const { return m_subNetworkList; }

	QList<GSEPtr> allGSEList() const;

private:
	XMLNode m_node;
	QList<SubNetworkPtr> m_subNetworkList;
};