#include "TreeItems.h"

#include <qdebug.h>

BaseTreeItem::BaseTreeItem(BaseTreeItem * parent) :
	m_parent(parent)
{
}

BaseTreeItem::~BaseTreeItem()
{
	qDeleteAll(m_childItems);
}

void BaseTreeItem::appendChild(BaseTreeItem * item)
{
	m_childItems.append(item);
}

BaseTreeItem * BaseTreeItem::child(int row) const
{
	return m_childItems.value(row, nullptr);
}

BaseTreeItem * BaseTreeItem::parent() const
{
	return m_parent;
}

int BaseTreeItem::childCount() const
{
	return m_childItems.count();
}

int BaseTreeItem::row() const
{
	if (m_parent)
		return m_parent->m_childItems.lastIndexOf(const_cast<BaseTreeItem*>(this));

	return 0;
}

void BaseTreeItem::removeChild(int position)
{
	if (position < 0 || position >= m_childItems.size())
		return;

	delete m_childItems.takeAt(position);
}

int BaseTreeItem::columnCount() const
{
	return 1;
}

//==========================================================================================
IEDTreeItem::IEDTreeItem(const IEDPtr & ied, const QString &name, BaseTreeItem::Mode mode, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_ied(ied),
	m_name(name)
{
	if (!ied)
		return;

	for (auto &ldevice : m_ied->lDevices())
		appendChild(new LDeviceTreeItem(ldevice, mode, this));
}

//==========================================================================================
LDeviceTreeItem::LDeviceTreeItem(const LDevicePtr & lDevice, BaseTreeItem::Mode mode, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_lDevice(lDevice)
{
	for (auto &ln : m_lDevice->lnList())
		appendChild(new LNTreeItem(ln, mode, this));
}

QString LDeviceTreeItem::name() const
{
	return m_lDevice->inst();
}

//==========================================================================================
LNTreeItem::LNTreeItem(const LNPtr & ln, BaseTreeItem::Mode mode, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_ln(ln)
{
	if (mode == BaseTreeItem::DataModelMode)
		for (auto &it : m_ln->type()->doList())
			appendChild(new DOTreeItem(it, this));
	else if (mode == BaseTreeItem::SetupMode) {
		for (auto &it : m_ln->dataSets())
			appendChild(new DataSetTreeItem(it, this));
		for (auto &it : m_ln->reports())
			appendChild(new ReportControlTreeItem(it, this));
		for (auto &it : m_ln->gseControls())
			appendChild(new GSEControlTreeItem(it, this));
	}
}

QString LNTreeItem::name() const
{
	return m_ln->name();
}

void LNTreeItem::removeChild(int position)
{
	if (position < 0 || position >= childCount())
		return;

	BaseTreeItem *item = child(position);
	int index = -1;
	for (int i = 0; i < childCount(); i++) {
		if (child(i)->itemType() == item->itemType())
			index++;
		if (item == child(i))
			break;
	}

	if (item->itemType() == DSType)
		m_ln->removeDataSet(index);
	if (item->itemType() == RType)
		m_ln->removeReport(index);

	BaseTreeItem::removeChild(position);
}

void LNTreeItem::appendDataSet(const QString & name)
{
	appendChild(new DataSetTreeItem(m_ln->addDataSet(name), this));
}

void LNTreeItem::appendReport(const QString & name)
{
	appendChild(new ReportControlTreeItem(m_ln->addReport(name), this));
}

//==========================================================================================
DOTreeItem::DOTreeItem(const DOPtr & doItem, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_do(doItem)
{
	for (auto &it : m_do->type()->sdoList())
		appendChild(new SDOTreeItem(it, this));

	for (auto &it : m_do->type()->daList())
		appendChild(new DATreeItem(it, this));
}

QString DOTreeItem::name() const
{
	return m_do->name();
}

//==========================================================================================
SDOTreeItem::SDOTreeItem(const SDOPtr & sdoItem, BaseTreeItem * parent) :
	BaseTreeItem(parent), 
	m_sdo(sdoItem)
{
	for (auto &it : m_sdo->childs())
		appendChild(new SDOTreeItem(it, this));

	if (m_sdo->type())
		for (auto &it : m_sdo->type()->daList())
			appendChild(new DATreeItem(it, this));
}

QString SDOTreeItem::name() const
{
	return m_sdo->name();
}

//==========================================================================================
DATreeItem::DATreeItem(const DAPtr & daItem, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_da(daItem)
{
	for (auto &it : m_da->childs())
		appendChild(new DATreeItem(it, this));

	if (m_da->type())
		for (auto &it : m_da->type()->bdaList())
			appendChild(new BDATreeItem(it, this));
}

QString DATreeItem::name() const
{
	return m_da->name();
}

//==========================================================================================
BDATreeItem::BDATreeItem(const BDAPtr & bdaItem, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_bda(bdaItem)
{
	for (auto &it : m_bda->childs())
		appendChild(new BDATreeItem(it, this));
}

QString BDATreeItem::name() const
{
	return m_bda->name();
}

//==========================================================================================
DataSetTreeItem::DataSetTreeItem(const DataSetPtr & ds, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_ds(ds)
{
	for (auto &it : m_ds->fcdaList())
		appendChild(new FCDATreeItem(it, this));
}

QString DataSetTreeItem::name() const
{
	return m_ds->name();
}

void DataSetTreeItem::removeChild(int position)
{
	m_ds->removeFCDA(position);
	BaseTreeItem::removeChild(position);
}

void DataSetTreeItem::setName(const QString & name)
{
	m_ds->setName(name);
}

void DataSetTreeItem::appendFCDA(const QString & ldInst, const QString &prefix, const QString & lnClass, const QString & lnInst, const QString &doName, const QString & daName, const QString & fc)
{
	appendChild(new FCDATreeItem(m_ds->addFCDA(ldInst, prefix, lnClass, lnInst, doName, daName, fc), this));
}

//==========================================================================================
FCDATreeItem::FCDATreeItem(const FCDAPtr & fcda, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_fcda(fcda)
{
}

QString FCDATreeItem::name() const
{
	return m_fcda->name();
}

//==========================================================================================
ReportControlTreeItem::ReportControlTreeItem(const ReportControlPtr & p, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_report(p)
{
}

QString ReportControlTreeItem::name() const
{
	return m_report->name();
}

void ReportControlTreeItem::setName(const QString & name)
{
	m_report->setName(name);
}

//==========================================================================================
GSEControlTreeItem::GSEControlTreeItem(const GSEControlPtr & p, BaseTreeItem * parent) :
	BaseTreeItem(parent),
	m_gseControl(p)
{
}

QString GSEControlTreeItem::name() const
{
	return m_gseControl->name();
}
