#pragma once

#include <qstring.h>

namespace IEC61850 {
	
	const QString FILE_BASE_NAME = "config";

	namespace XML {
		namespace TAG {
			const QString Communication = "Communication";
			const QString SubNetwork = "SubNetwork";
			const QString Address = "Address";
			const QString ConnectedAP = "ConnectedAP";
			const QString P = "P";
			const QString MinTime = "MinTime";
			const QString MaxTime = "MaxTime";
			const QString GSE = "GSE";
			const QString SMV = "SMV";
			const QString IED = "IED";
			const QString BDA = "BDA";
			const QString DAType = "DAType";
			const QString DA = "DA";
			const QString SDO = "SDO";
			const QString DOType = "DOType";
			const QString DO = "DO";
			const QString LNodeType = "LNodeType";
			const QString DataTypeTemplates = "DataTypeTemplates";
			const QString LDevice = "LDevice";
			const QString LN0 = "LN0";
			const QString LN = "LN";
			const QString AccessPoint = "AccessPoint";
			const QString Server = "Server";
			const QString DataSet = "DataSet";
			const QString FCDA = "FCDA";
			const QString ReportControl = "ReportControl";
			const QString GSEControl = "GSEControl";
			const QString TrgOps = "TrgOps";
			const QString OptFields = "OptFields";
			const QString RptEnabled = "RptEnabled";
			const QString SampledValueControl = "SampledValueControl";
			const QString SmvOpts = "SmvOpts";
		} // TAG

		namespace ATTR {
			const QString Type = "type";
			const QString BType = "bType";
			const QString IedName = "iedName";
			const QString ApName = "apName";
			const QString CbName = "cbName";
			const QString Name = "name";
			const QString Count = "count";
			const QString ID = "id";
			const QString CDC = "cdc";
			const QString FC = "fc";
			const QString Inst = "inst";			
			const QString LnType = "lnType";
			const QString LnClass = "lnClass";
			const QString Prefix = "prefix";
			const QString LdInst = "ldInst";
			const QString LnInst = "lnInst";
			const QString DoName = "doName";
			const QString DaName = "daName";
			const QString DatSet = "datSet";
			const QString RptID = "rptID";
			const QString ConfRev = "confRev";
			const QString Buffered = "buffered";
			const QString Desc = "desc";
			const QString BufTime = "bufTime";
			const QString IntgPd = "intgPd";
			const QString Dchg = "dchg";
			const QString Qchg = "qchg";
			const QString Dupd = "dupd";
			const QString Period = "period";
			const QString SeqNum = "seqNum";
			const QString TimeStamp = "timeStamp";
			const QString DataSet = "dataSet";
			const QString ReasonCode = "reasonCode";
			const QString DataRef = "dataRef";
			const QString EntryID = "entryID";
			const QString ConfigRef = "configRef";
			const QString Max = "max";
			const QString AppID = "appID";
			const QString GoEna = "GoEna";
			const QString SmvID = "smvID";
			const QString SmpRate = "smpRate";
			const QString NofASDU = "nofASDU";
			const QString RefreshTime = "refreshTime";
			const QString SampleSynchronized = "sampleSynchronized";
			const QString SampleRate = "sampleRate";
			const QString Security = "security";
		} // ATTR

		namespace VALUE {
			const QString Ip = "IP";
			const QString Subnet = "IP-SUBNET";
			const QString Gateway = "IP-GATEWAY";
			const QString MacAddress = "MAC-Address";
			const QString VlanId = "VLAN-ID";
			const QString VlanPriority = "VLAN-PRIORITY";
			const QString GoID = "GoID";
			const QString AppID = "APPID";
			const QString Struct = "Struct";
		} // VALUE

	} // XML

} //61850