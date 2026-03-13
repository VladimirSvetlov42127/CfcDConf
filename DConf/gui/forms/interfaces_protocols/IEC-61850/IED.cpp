#include "IED.h"

#include <qdebug.h>

#include "61850_defines.h"

namespace {
	QString fromBool(bool val) { return val ? "true" : "false"; }
	bool toBool(const QString &val) { return val == "true"; }
}

FCDA::FCDA(const XMLNode & node) :
	m_node(node)
{
}

QString FCDA::name() const
{
	QString ldInst = m_node.attribute(IEC61850::XML::ATTR::LdInst);
	QString prefix = m_node.attribute(IEC61850::XML::ATTR::Prefix);
	QString lnClass = m_node.attribute(IEC61850::XML::ATTR::LnClass);
	QString lnInst = m_node.attribute(IEC61850::XML::ATTR::LnInst);
	QString doName =  m_node.attribute(IEC61850::XML::ATTR::DoName);
	QString daName = m_node.attribute(IEC61850::XML::ATTR::DaName);
	QString fc = m_node.attribute(IEC61850::XML::ATTR::FC);

	QString result = QString("%1/%2%3%4.%5").arg(ldInst, prefix, lnClass, lnInst, doName);
	if (!daName.isEmpty())
		result.append(".").append(daName);
	return QString("%1 (%2)").arg(result, fc);
}

QString FCDA::ldInst() const
{
	return m_node.attribute(IEC61850::XML::ATTR::LdInst);
}

QString FCDA::prefix() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Prefix);
}

QString FCDA::lnClass() const
{
	return m_node.attribute(IEC61850::XML::ATTR::LnClass);
}

QString FCDA::lnInst() const
{
	return m_node.attribute(IEC61850::XML::ATTR::LnInst);
}

QString FCDA::doName() const
{
	return m_node.attribute(IEC61850::XML::ATTR::DoName);
}

QString FCDA::daName() const
{
	return m_node.attribute(IEC61850::XML::ATTR::DaName);
}

QString FCDA::fc() const
{
	return m_node.attribute(IEC61850::XML::ATTR::FC);
}

//==========================================================================================
DataSet::DataSet(const XMLNode & node) :
	m_node(node)
{
	for (auto &node : m_node.childs(IEC61850::XML::TAG::FCDA)) {
		m_fcdaList.append(std::make_shared<FCDA>(node));
	}
}

QString DataSet::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

void DataSet::setName(const QString & name)
{
	QString oldName = m_node.attribute(IEC61850::XML::ATTR::Name);
	if (oldName == name)
		return;

	m_node.setAttribute(IEC61850::XML::ATTR::Name, name);
	emit nameChanged(name);
}

void DataSet::removeFCDA(int position)
{
	if (position < 0 || position >= m_fcdaList.size())
		return;

	m_node.removeChild(m_fcdaList.takeAt(position)->node());
}

FCDAPtr DataSet::addFCDA(const QString & ldInst, const QString &prefix, const QString & lnClass, const QString & lnInst, const QString &doName, const QString & daName, const QString & fc)
{
	XMLNode fcdaNode = m_node.createChild(IEC61850::XML::TAG::FCDA);
	fcdaNode.setAttribute(IEC61850::XML::ATTR::LdInst, ldInst);
	fcdaNode.setAttribute(IEC61850::XML::ATTR::LnClass, lnClass);
	fcdaNode.setAttribute(IEC61850::XML::ATTR::LnInst, lnInst);
	fcdaNode.setAttribute(IEC61850::XML::ATTR::DoName, doName);
	fcdaNode.setAttribute(IEC61850::XML::ATTR::FC, fc);
	if (!prefix.isEmpty())
		fcdaNode.setAttribute(IEC61850::XML::ATTR::Prefix, prefix);
	if (!daName.isEmpty())
		fcdaNode.setAttribute(IEC61850::XML::ATTR::DaName, daName);

	FCDAPtr fcdaPtr = std::make_shared<FCDA>(fcdaNode);
	m_fcdaList.append(fcdaPtr);
	return fcdaPtr;
}

//==========================================================================================
LN::LN(const XMLNode & node, const LNodeTypePtr & type, bool ln0) :
	m_node(node),
	m_isLN0(ln0),
	m_type(type)
{
	QHash<QString, DataSetPtr> dataSetList;
	for (auto &dsNode : m_node.childs(IEC61850::XML::TAG::DataSet)) {
		auto dataSet = std::make_shared<DataSet>(dsNode);
		m_dataSets.append(dataSet);
		dataSetList[dataSet->name()] = dataSet;
	}

	for (auto &node : m_node.childs(IEC61850::XML::TAG::ReportControl)) {
		auto r = std::make_shared<ReportControl>(node);
		r->setDatSet(dataSetList[node.attribute(IEC61850::XML::ATTR::DatSet)]);
		m_reports.append(r);
	}

	for (auto &node : m_node.childs(IEC61850::XML::TAG::GSEControl)) {
		auto gse = std::make_shared<GSEControl>(node);
		gse->setDatSet(dataSetList[node.attribute(IEC61850::XML::ATTR::DatSet)]);
		m_gseControls.append(gse);
	}

	for (auto &node : m_node.childs(IEC61850::XML::TAG::SampledValueControl)) {
		auto sv = std::make_shared<SVControl>(node);
		m_svControls.append(sv);
	}
}

QString LN::inst() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Inst);
}

QString LN::prefix() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Prefix);
}

QString LN::lnClass() const
{
	return m_node.attribute(IEC61850::XML::ATTR::LnClass);
}

QString LN::name() const
{
	return QString("%1%2%3").arg(prefix(), lnClass(), inst());
}

DataSetPtr LN::dataSet(const QString & name) const
{
	for (auto &it : m_dataSets)
		if (it->name() == name)
			return it;

	return DataSetPtr();
}

DataSetPtr LN::addDataSet(const QString & name)
{
	XMLNode ds = m_node.createChild(IEC61850::XML::TAG::DataSet);
	ds.setAttribute(IEC61850::XML::ATTR::Name, name);

	DataSetPtr dsPtr = std::make_shared<DataSet>(ds);
	m_dataSets.append(dsPtr);
	return dsPtr;
}

void LN::removeDataSet(int position)
{
	if (position < 0 || position >= m_dataSets.size())
		return;

	auto ds = m_dataSets.takeAt(position);
	for (auto &it : reports())
		if (it->datSet() == ds)
			it->setDatSet(DataSetPtr());
	for (auto &it : gseControls())
		if (it->datSet() == ds)
			it->setDatSet(DataSetPtr());

	m_node.removeChild(ds->node());
}

ReportControlPtr LN::addReport(const QString & name)
{
	XMLNode n = m_node.createChild(IEC61850::XML::TAG::ReportControl);
	n.setAttribute(IEC61850::XML::ATTR::Name, name);
	n.setAttribute(IEC61850::XML::ATTR::DatSet, "");
	n.setAttribute(IEC61850::XML::ATTR::RptID, "LLN0$");
	n.setAttribute(IEC61850::XML::ATTR::ConfRev, "1");
	n.setAttribute(IEC61850::XML::ATTR::Buffered, "true");
	n.setAttribute(IEC61850::XML::ATTR::BufTime, "50");
	n.setAttribute(IEC61850::XML::ATTR::IntgPd, "3000");

	XMLNode to = n.createChild(IEC61850::XML::TAG::TrgOps);
	to.setAttribute(IEC61850::XML::ATTR::Dchg, "true");
	to.setAttribute(IEC61850::XML::ATTR::Qchg, "true");
	to.setAttribute(IEC61850::XML::ATTR::Dupd, "true");
	to.setAttribute(IEC61850::XML::ATTR::Period, "true");

	XMLNode of = n.createChild(IEC61850::XML::TAG::OptFields);
	of.setAttribute(IEC61850::XML::ATTR::SeqNum, "true");
	of.setAttribute(IEC61850::XML::ATTR::TimeStamp, "true");
	of.setAttribute(IEC61850::XML::ATTR::DataSet, "true");
	of.setAttribute(IEC61850::XML::ATTR::ReasonCode, "true");
	of.setAttribute(IEC61850::XML::ATTR::DataRef, "false");
	of.setAttribute(IEC61850::XML::ATTR::EntryID, "true");
	of.setAttribute(IEC61850::XML::ATTR::ConfigRef, "true");

	XMLNode rpt = n.createChild(IEC61850::XML::TAG::RptEnabled);
	rpt.setAttribute(IEC61850::XML::ATTR::Max, "1");

	ReportControlPtr p = std::make_shared<ReportControl>(n);
	m_reports.append(p);
	return p;
}

void LN::removeReport(int position)
{
	if (position < 0 || position >= m_reports.size())
		return;

	m_node.removeChild(m_reports.takeAt(position)->node());
}

GSEControlPtr LN::gseControl(const QString & name) const
{
	for (auto &it : m_gseControls)
		if (it->name() == name)
			return it;

	return GSEControlPtr();
}

SVControlPtr LN::addSV(const QString & name, const DataSetPtr &datset)
{
	auto n = m_node.createChild(IEC61850::XML::TAG::SampledValueControl);
	n.setAttribute(IEC61850::XML::ATTR::Name, name);
	n.setAttribute(IEC61850::XML::ATTR::DatSet, datset->name());
	n.setAttribute(IEC61850::XML::ATTR::ConfRev, "1");
	n.setAttribute(IEC61850::XML::ATTR::SmvID, QString("depRTU_%1").arg(datset->name()));
	n.setAttribute(IEC61850::XML::ATTR::SmpRate, "80");
	n.setAttribute(IEC61850::XML::ATTR::NofASDU, "1");

	XMLNode of = n.createChild(IEC61850::XML::TAG::SmvOpts);
	of.setAttribute(IEC61850::XML::ATTR::RefreshTime, "false");
	of.setAttribute(IEC61850::XML::ATTR::SampleSynchronized, "true");
	of.setAttribute(IEC61850::XML::ATTR::SampleRate, "false");
	of.setAttribute(IEC61850::XML::ATTR::Security, "false");
	of.setAttribute(IEC61850::XML::ATTR::DataRef, "false");

	auto p = std::make_shared<SVControl>(n);
	m_svControls.append(p);
	return p;
}

void LN::removeSV(int position)
{
	if (position < 0 || position >= m_svControls.size())
		return;

	m_node.removeChild(m_svControls.takeAt(position)->node());
}

//==========================================================================================
LDevice::LDevice(const XMLNode & node, const QHash<QString, LNodeTypePtr>& lnTypes) :
	m_node(node)
{
	for (auto &lnNode : m_node.childs(IEC61850::XML::TAG::LN0)) {
		m_lnList.append(std::make_shared<LN>(lnNode, lnTypes[lnNode.attribute(IEC61850::XML::ATTR::LnType)], true));
	}

	for (auto &lnNode : m_node.childs(IEC61850::XML::TAG::LN)) {
		m_lnList.append(std::make_shared<LN>(lnNode, lnTypes[lnNode.attribute(IEC61850::XML::ATTR::LnType)]));
	}
}

QString LDevice::inst() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Inst);
}

LNPtr LDevice::ln0() const
{
	for (auto &it : m_lnList)
		if (it->isLN0())
			return it;

	return LNPtr();
}

LNPtr LDevice::ln(const QString & prefix, const QString & lnClass, const QString & lnInst) const
{
	for (auto &it : m_lnList)
		if (it->prefix() == prefix && it->lnClass() == lnClass && it->inst() == lnInst)
			return it;

	return LNPtr();
}

LNPtr LDevice::ln(const QString & name) const
{
	for (auto &it : m_lnList)
		if (it->name() == name)
			return it;

	return LNPtr();
}

//==========================================================================================
AccessPoint::AccessPoint(const XMLNode &node, const DataTypeTemplates &dataTypes) :
	m_node(node)
{
	auto serverList = m_node.childs(IEC61850::XML::TAG::Server);
	if (serverList.isEmpty())
		return;

	for (auto &lDeviceNode : serverList.first().childs(IEC61850::XML::TAG::LDevice))
		m_lDevices.append(std::make_shared<LDevice>(lDeviceNode, dataTypes.lnTypes()));
}

LDevicePtr AccessPoint::device(const QString & name) const
{
	for (auto &device : m_lDevices)
		if (device->inst() == name)
			return device;

	return LDevicePtr();
}

QString AccessPoint::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

//==========================================================================================
IED::IED(const XMLNode &node, const DataTypeTemplates &dataTypes, const CommunicationPtr &communication) :
	m_node(node)
{
	auto accessPointList = m_node.childs(IEC61850::XML::TAG::AccessPoint);
	for (auto &it : accessPointList) {
		auto ap = std::make_shared<AccessPoint>(it, dataTypes);
		m_accessPoints << ap;

		for (auto &sn : communication->subNetworkList()) {
			for (auto &cap : sn->connectedAPList())
				if (cap->iedName() == name() && cap->apName() == ap->name()) {
					ap->setConnectedAP(cap);
					break;
				}
			if (ap->connectedAP())
				break;
		}

		if (!ap->connectedAP())
			continue;

		for (auto &gse : ap->connectedAP()->gseList()) {
			auto device = ap->device(gse->ldInst());
			if (!device)
				continue;

			auto ln = device->ln0();
			if (!ln)
				continue;
			
			for (auto &gseControl : ln->gseControls())
				if (gseControl->name() == gse->cbName())
					gseControl->setGseAP(gse);
		}

		for (auto &smv : ap->connectedAP()->smvList()) {
			auto device = ap->device(smv->ldInst());
			if (!device)
				continue;

			auto ln = device->ln0();
			if (!ln)
				continue;

			for (auto &svControl : ln->svControls())
				if (svControl->name() == smv->cbName())
					svControl->setSmvAP(smv);
		}
	}
}

QString IED::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

void IED::setName(const QString & name)
{
	m_node.setAttribute(IEC61850::XML::ATTR::Name, name);

	for (auto &ap : m_accessPoints)
		if (ap->connectedAP())
			ap->connectedAP()->setIedName(name);
}

QList<LDevicePtr> IED::lDevices() const
{
	QList<LDevicePtr> result;
	for (auto &ap : m_accessPoints)
		result << ap->lDevices();

	return result;
}

LDevicePtr IED::device(const QString & name) const
{
	for (auto &ap : m_accessPoints) {
		auto device = ap->device(name);
		if (device)
			return device;
	}

	return LDevicePtr();
}

QList<SVControlPtr> IED::svControlList(const QString & deviceName) const
{
	QList<SVControlPtr> res;
	auto dev = device(deviceName);
	if (dev)
		for (auto &ln : dev->lnList())
			res << ln->svControls();

	return res;
}

//==========================================================================================
ReportControl::ReportControl(const XMLNode & node) :
	m_node(node)
{
}

QString ReportControl::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

void ReportControl::setName(const QString & name)
{
	m_node.setAttribute(IEC61850::XML::ATTR::Name, name);
}

DataSetPtr ReportControl::datSet() const
{
	return m_datSet;
}

void ReportControl::setDatSet(const DataSetPtr & datSet)
{
	if (m_datSet)
		disconnect(m_datSet.get(), &DataSet::nameChanged, this, &ReportControl::onDsNameChanged);
	m_datSet = datSet;
	if (m_datSet)
		connect(m_datSet.get(), &DataSet::nameChanged, this, &ReportControl::onDsNameChanged);

	m_node.setAttribute(IEC61850::XML::ATTR::DatSet, m_datSet ? m_datSet->name() : "");
}

QString ReportControl::rptID() const
{
	return m_node.attribute(IEC61850::XML::ATTR::RptID);
}

void ReportControl::setRptID(const QString & id)
{
	m_node.setAttribute(IEC61850::XML::ATTR::RptID, id);
}

int ReportControl::confRev() const
{
	return m_node.attribute(IEC61850::XML::ATTR::ConfRev).toInt();
}

void ReportControl::setConfRev(int rev)
{
	m_node.setAttribute(IEC61850::XML::ATTR::ConfRev, QString::number(rev));
}

bool ReportControl::buffered() const
{
	return toBool(m_node.attribute(IEC61850::XML::ATTR::Buffered));
}

void ReportControl::setBuffered(bool val)
{
	m_node.setAttribute(IEC61850::XML::ATTR::Buffered, fromBool(val));
}

int ReportControl::intgPd() const
{
	return m_node.attribute(IEC61850::XML::ATTR::IntgPd).toInt();
}

void ReportControl::setIntgPd(int val)
{
	m_node.setAttribute(IEC61850::XML::ATTR::IntgPd, QString::number(val));
}

int ReportControl::bufTime() const
{
	return m_node.attribute(IEC61850::XML::ATTR::BufTime).toInt();
}

void ReportControl::setBufTime(int val)
{
	m_node.setAttribute(IEC61850::XML::ATTR::BufTime, QString::number(val));
}

bool ReportControl::dchg() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::TrgOps).attribute(IEC61850::XML::ATTR::Dchg));
}

void ReportControl::setDchg(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::TrgOps).setAttribute(IEC61850::XML::ATTR::Dchg, fromBool(val));
}

bool ReportControl::qchg() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::TrgOps).attribute(IEC61850::XML::ATTR::Qchg));
}

void ReportControl::setQchg(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::TrgOps).setAttribute(IEC61850::XML::ATTR::Qchg, fromBool(val));
}

bool ReportControl::dupd() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::TrgOps).attribute(IEC61850::XML::ATTR::Dupd));
}

void ReportControl::setDupd(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::TrgOps).setAttribute(IEC61850::XML::ATTR::Dupd, fromBool(val));
}

bool ReportControl::period() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::TrgOps).attribute(IEC61850::XML::ATTR::Period));
}

void ReportControl::setPeriod(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::TrgOps).setAttribute(IEC61850::XML::ATTR::Period, fromBool(val));
}

bool ReportControl::seqNum() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::OptFields).attribute(IEC61850::XML::ATTR::SeqNum));
}

void ReportControl::setSeqNum(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::OptFields).setAttribute(IEC61850::XML::ATTR::SeqNum, fromBool(val));
}

bool ReportControl::timeStamp() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::OptFields).attribute(IEC61850::XML::ATTR::TimeStamp));
}

void ReportControl::setTimeStamp(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::OptFields).setAttribute(IEC61850::XML::ATTR::TimeStamp, fromBool(val));
}

bool ReportControl::dataSet() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::OptFields).attribute(IEC61850::XML::ATTR::DataSet));
}

void ReportControl::setDataSet(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::OptFields).setAttribute(IEC61850::XML::ATTR::DataSet, fromBool(val));
}

bool ReportControl::reasonCode() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::OptFields).attribute(IEC61850::XML::ATTR::ReasonCode));
}

void ReportControl::setReasonCode(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::OptFields).setAttribute(IEC61850::XML::ATTR::ReasonCode, fromBool(val));
}

bool ReportControl::dataRef() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::OptFields).attribute(IEC61850::XML::ATTR::DataRef));
}

void ReportControl::setDataRef(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::OptFields).setAttribute(IEC61850::XML::ATTR::DataRef, fromBool(val));
}

bool ReportControl::entryID() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::OptFields).attribute(IEC61850::XML::ATTR::EntryID));
}

void ReportControl::setEntryID(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::OptFields).setAttribute(IEC61850::XML::ATTR::EntryID, fromBool(val));
}

bool ReportControl::configRef() const
{
	return toBool(m_node.child(IEC61850::XML::TAG::OptFields).attribute(IEC61850::XML::ATTR::ConfigRef));
}

void ReportControl::setConfigRef(bool val)
{
	m_node.forceChild(IEC61850::XML::TAG::OptFields).setAttribute(IEC61850::XML::ATTR::ConfigRef, fromBool(val));
}

int ReportControl::maxRpt() const
{
	return m_node.child(IEC61850::XML::TAG::RptEnabled).attribute(IEC61850::XML::ATTR::Max).toInt();
}

void ReportControl::setMaxRpt(int val)
{
	m_node.forceChild(IEC61850::XML::TAG::RptEnabled).setAttribute(IEC61850::XML::ATTR::Max, QString::number(val));
}

void ReportControl::onDsNameChanged(const QString &name)
{
	m_node.setAttribute(IEC61850::XML::ATTR::DatSet, name);
}

//==========================================================================================
GSEControl::GSEControl(const XMLNode & node) :
	m_node(node)
{
}

QString GSEControl::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

void GSEControl::setName(const QString & name)
{
	m_node.setAttribute(IEC61850::XML::ATTR::Name, name);
}

DataSetPtr GSEControl::datSet() const
{
	return m_datSet;
}

void GSEControl::setDatSet(const DataSetPtr & datSet)
{
	if (m_datSet)
		disconnect(m_datSet.get(), &DataSet::nameChanged, this, &GSEControl::onDsNameChanged);
	m_datSet = datSet;
	if (m_datSet)
		connect(m_datSet.get(), &DataSet::nameChanged, this, &GSEControl::onDsNameChanged);

	m_node.setAttribute(IEC61850::XML::ATTR::DatSet, m_datSet ? m_datSet->name() : "");
}

QString GSEControl::appID() const
{
	return m_node.attribute(IEC61850::XML::ATTR::AppID);
}

void GSEControl::setAppID(const QString & id)
{
	m_node.setAttribute(IEC61850::XML::ATTR::AppID, id);
}

int  GSEControl::confRev() const
{
	return m_node.attribute(IEC61850::XML::ATTR::ConfRev).toInt();
}

void GSEControl::setConfRev(int rev)
{
	m_node.setAttribute(IEC61850::XML::ATTR::ConfRev, QString::number(rev));
}

bool GSEControl::goEna() const
{
	return toBool(m_node.attribute(IEC61850::XML::ATTR::GoEna));
}

void GSEControl::setGoEna(bool val)
{
	m_node.setAttribute(IEC61850::XML::ATTR::GoEna, fromBool(val));
}

void GSEControl::onDsNameChanged(const QString &name)
{
	m_node.setAttribute(IEC61850::XML::ATTR::DatSet, name);
}

//==========================================================================================
SVControl::SVControl(const XMLNode& node) :
	m_node(node),
	m_workMode(0)
{
}

QString SVControl::name() const
{
	return m_node.attribute(IEC61850::XML::ATTR::Name);
}

QString SVControl::datSet() const
{
	return m_node.attribute(IEC61850::XML::ATTR::DatSet);
}

int SVControl::confRev() const
{
	return m_node.attribute(IEC61850::XML::ATTR::ConfRev).toInt();
}

void SVControl::setConfRev(int rev)
{
	m_node.setAttribute(IEC61850::XML::ATTR::ConfRev, QString::number(rev));
}

QString SVControl::smvID() const
{
	return m_node.attribute(IEC61850::XML::ATTR::SmvID);
}

void SVControl::setSmvID(const QString & id)
{
	m_node.setAttribute(IEC61850::XML::ATTR::SmvID, id);
}

int SVControl::smpRate() const
{
	return m_node.attribute(IEC61850::XML::ATTR::SmpRate).toInt();
}

void SVControl::setSmpRate(int val)
{
	m_node.setAttribute(IEC61850::XML::ATTR::SmpRate, QString::number(val));
}

int SVControl::nofASDU() const
{
	return m_node.attribute(IEC61850::XML::ATTR::NofASDU).toInt();
}

void SVControl::setNofASDU(int val)
{
	m_node.setAttribute(IEC61850::XML::ATTR::NofASDU, QString::number(val));
}
