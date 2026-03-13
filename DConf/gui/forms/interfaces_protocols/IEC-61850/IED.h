#pragma once

#include <qobject.h>

#include "DataTypeTemplates.h"
#include "Communication.h"

class LDevice;
class LN;
class IED;
class DataSet;
class FCDA;
class ReportControl;
class GSEControl;
class SVControl;
class AccessPoint;

typedef std::shared_ptr<LDevice> LDevicePtr;
typedef std::shared_ptr<LN> LNPtr;
typedef std::shared_ptr<IED> IEDPtr;
typedef std::shared_ptr<DataSet> DataSetPtr;
typedef std::shared_ptr<FCDA> FCDAPtr;
typedef std::shared_ptr<ReportControl> ReportControlPtr;
typedef std::shared_ptr<GSEControl> GSEControlPtr;
typedef std::shared_ptr<SVControl> SVControlPtr;
typedef std::shared_ptr<AccessPoint> AccessPointPtr;

class FCDA
{
public:
	FCDA(const XMLNode &node);

	QString name() const;
	XMLNode node() const { return m_node; }

	QString ldInst() const;
	QString prefix() const;
	QString lnClass() const;
	QString lnInst() const;
	QString doName() const;
	QString daName() const;
	QString fc() const;

private:
	XMLNode m_node;
};

//==========================================================================================
class DataSet : public QObject
{
	Q_OBJECT
public:
	DataSet(const XMLNode &node);

	QString name() const;
	void setName(const QString &name);
	QList<FCDAPtr> fcdaList() const { return m_fcdaList; }
	XMLNode node() const { return m_node; }
	void removeFCDA(int position);
	FCDAPtr addFCDA(const QString &ldInst, const QString &prefix, const QString &lnClass, const QString &lnInst,
		const QString &doName, const QString &daName, const QString &fc);

signals:
	void nameChanged(const QString &name);

private:
	XMLNode m_node;
	QList<FCDAPtr> m_fcdaList;
};

//==========================================================================================
class LN : public QObject
{
public:
	LN(const XMLNode &node, const LNodeTypePtr &type, bool ln0 = false);

	QString inst() const;
	QString prefix() const;
	QString lnClass() const;
	QString name() const;
	LNodeTypePtr type() const { return m_type; }
	bool isLN0() const { return m_isLN0; }

	QList<DataSetPtr> dataSets() const { return m_dataSets; }
	DataSetPtr dataSet(const QString &name) const;
	DataSetPtr addDataSet(const QString &name);
	void removeDataSet(int position);

	QList<ReportControlPtr> reports() const { return m_reports; }
	ReportControlPtr addReport(const QString &name);
	void removeReport(int position);

	QList<GSEControlPtr> gseControls() const { return m_gseControls; }
	GSEControlPtr gseControl(const QString &name) const;

	QList<SVControlPtr> svControls() const { return m_svControls; }
	SVControlPtr addSV(const QString &name, const DataSetPtr &datset);
	void removeSV(int position);

private:
	XMLNode m_node;
	bool m_isLN0;
	LNodeTypePtr m_type;
	QList<DataSetPtr> m_dataSets;
	QList<ReportControlPtr> m_reports;
	QList<GSEControlPtr> m_gseControls;
	QList<SVControlPtr> m_svControls;
};

//==========================================================================================
class LDevice : public QObject
{
public:
	LDevice(const XMLNode &node, const QHash<QString, LNodeTypePtr> &lnTypes);

	QString inst() const;
	QList<LNPtr> lnList() const { return m_lnList; }
	LNPtr ln0() const;
	LNPtr ln(const QString &prefix, const QString &lnClass, const QString &lnInst) const;
	LNPtr ln(const QString &name) const;

private:
	XMLNode m_node;
	QList<LNPtr> m_lnList;
};

//==========================================================================================
class AccessPoint
{
public:
	AccessPoint(const XMLNode &node, const DataTypeTemplates &dataTypes);
	QList<LDevicePtr> lDevices() const { return m_lDevices; }
	LDevicePtr device(const QString &name) const;

	QString name() const;
	ConnectedAPPtr connectedAP() const { return m_connectedAP; }
	void setConnectedAP(const ConnectedAPPtr &cap) { m_connectedAP = cap; }

private:
	XMLNode m_node;
	QList<LDevicePtr> m_lDevices;
	ConnectedAPPtr m_connectedAP;
};

//==========================================================================================
class IED : public QObject
{
public:
	IED(const XMLNode &node, const DataTypeTemplates &dataTypes, const CommunicationPtr &communication);

	QString name() const;
	void setName(const QString &name);

	QList<AccessPointPtr> accessPoints() const { return m_accessPoints; }

	QList<LDevicePtr> lDevices() const;
	LDevicePtr device(const QString &name) const;

	QList<SVControlPtr> svControlList(const QString &deviceName) const;

private:
	XMLNode m_node;
	QList<AccessPointPtr> m_accessPoints;
};

//==========================================================================================
class ReportControl : public QObject
{
	Q_OBJECT
public:
	ReportControl(const XMLNode &node);

	XMLNode node() const { return m_node; }
	QString name() const;
	void setName(const QString &name);

	DataSetPtr datSet() const;
	void setDatSet(const DataSetPtr &datSet);

	QString rptID() const;
	void setRptID(const QString &id);

	int confRev() const;
	void setConfRev(int rev);

	bool buffered() const;
	void setBuffered(bool val);

	int intgPd() const;
	void setIntgPd(int val);

	int bufTime() const;
	void setBufTime(int val);

	bool dchg() const;
	void setDchg(bool val);

	bool qchg() const;
	void setQchg(bool val);

	bool dupd() const;
	void setDupd(bool val);

	bool period() const;
	void setPeriod(bool val);

	bool seqNum() const;
	void setSeqNum(bool val);

	bool timeStamp() const;
	void setTimeStamp(bool val);

	bool dataSet() const;
	void setDataSet(bool val);

	bool reasonCode() const;
	void setReasonCode(bool val);

	bool dataRef() const;
	void setDataRef(bool val);

	bool entryID() const;
	void setEntryID(bool val);

	bool configRef() const;
	void setConfigRef(bool val);

	int maxRpt() const;
	void setMaxRpt(int val);

public slots:
	void onDsNameChanged(const QString &name);

private:
	XMLNode m_node;
	DataSetPtr m_datSet;
};

//==========================================================================================
class GSEControl : public QObject
{
	Q_OBJECT
public:
	GSEControl(const XMLNode &node);

	QString name() const;
	void setName(const QString &name);

	DataSetPtr datSet() const;
	void setDatSet(const DataSetPtr &datSet);

	QString appID() const;
	void setAppID(const QString &id);

	int confRev() const;
	void setConfRev(int rev);

	bool goEna() const;
	void setGoEna(bool val);

	GSEPtr gseAP() const { return m_gseAP; }
	void setGseAP(const GSEPtr &gseAP) { m_gseAP = gseAP; }

public slots:
	void onDsNameChanged(const QString &name);

private:
	XMLNode m_node;
	DataSetPtr m_datSet;
	GSEPtr m_gseAP;
};

//==========================================================================================
class SVControl : public QObject
{
	Q_OBJECT
public:
	SVControl(const XMLNode &node);

	XMLNode node() const { return m_node; }

	QString name() const;
	QString datSet() const;

	int confRev() const;
	void setConfRev(int rev);

	QString smvID() const;
	void setSmvID(const QString &id);

	int smpRate() const;
	void setSmpRate(int val);

	int nofASDU() const;
	void setNofASDU(int val);

	SMVPtr smvAP() const { return m_smvAP; }
	void setSmvAP(const SMVPtr &smvAP) { m_smvAP = smvAP; }

	int workMode() const { return m_workMode; }
	void setWorkMode(int mode) { m_workMode = mode; }

private:
	XMLNode m_node;
	SMVPtr m_smvAP;
	int m_workMode;
};