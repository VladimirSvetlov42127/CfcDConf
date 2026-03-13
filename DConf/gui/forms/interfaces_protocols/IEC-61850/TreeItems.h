#pragma once

#include "IED.h"

class BaseTreeItem
{
public:	

	enum Type {
		UnknownType,
		LDType,
		LNType,
		DOType,
		SDOType,
		DAType,
		BDAType,
		DSType,
		FCDAType,
		RType,
		GSEType
	};

	enum Mode {
		DataModelMode,
		SetupMode
	};

	BaseTreeItem(BaseTreeItem *parent = nullptr);
	virtual ~BaseTreeItem();

	void appendChild(BaseTreeItem *item);
	BaseTreeItem* child(int row) const;
	BaseTreeItem* parent() const;
	int childCount() const;
	int row() const;
	
	virtual void removeChild(int position);
	virtual int columnCount() const;
	virtual QString name() const = 0;
	virtual Type itemType() const { return UnknownType; }

private:
	BaseTreeItem *m_parent;
	QList<BaseTreeItem*> m_childItems;
};
//==========================================================================================
class IEDTreeItem : public BaseTreeItem
{
public:
	IEDTreeItem(const IEDPtr &ied, const QString &name, BaseTreeItem::Mode mode, BaseTreeItem *parent = nullptr);

	virtual QString name() const { return m_name; }

private:
	IEDPtr m_ied;
	QString m_name;
};

//==========================================================================================
class LDeviceTreeItem : public BaseTreeItem
{
public:
	LDeviceTreeItem(const LDevicePtr &lDevice, BaseTreeItem::Mode mode, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return LDType; }
	LDevicePtr ld() const { return m_lDevice; }

private:
	LDevicePtr m_lDevice;
};

//==========================================================================================
class LNTreeItem : public BaseTreeItem
{
public:
	LNTreeItem(const LNPtr &ln, BaseTreeItem::Mode mode, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return LNType; }
	virtual void removeChild(int position) override;

	LNPtr ln() const { return m_ln; }
	void appendDataSet(const QString &name);
	void appendReport(const QString &name);

private:
	LNPtr m_ln;
};

//==========================================================================================
class DOTreeItem : public BaseTreeItem
{
public:
	DOTreeItem(const DOPtr &doItem, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return DOType; }
	DOPtr doPtr() const { return m_do; }

private:
	DOPtr m_do;
};

//==========================================================================================
class SDOTreeItem : public BaseTreeItem
{
public:
	SDOTreeItem(const SDOPtr &sdoItem, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return SDOType; }
	SDOPtr sdo() const { return m_sdo; }

private:
	SDOPtr m_sdo;
};

//==========================================================================================
class DATreeItem : public BaseTreeItem
{
public:
	DATreeItem(const DAPtr &daItem, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return DAType; }
	DAPtr da() const { return m_da; }

private:
	DAPtr m_da;
};

//==========================================================================================
class BDATreeItem : public BaseTreeItem
{
public:
	BDATreeItem(const BDAPtr &bdaItem, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return BDAType; }

private:
	BDAPtr m_bda;
};

//==========================================================================================
class DataSetTreeItem : public BaseTreeItem
{
public:
	DataSetTreeItem(const DataSetPtr &ds, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return DSType; }
	virtual void removeChild(int position) override;

	void setName(const QString &name);
	void appendFCDA(const QString &ldInst, const QString &prefix, const QString &lnClass, const QString &lnInst,
		const QString &doName, const QString &daName, const QString &fc);

private:
	DataSetPtr m_ds;
};

//==========================================================================================
class FCDATreeItem : public BaseTreeItem
{
public:
	FCDATreeItem(const FCDAPtr &fcda, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return FCDAType; }

private:
	FCDAPtr m_fcda;
};

//==========================================================================================
class ReportControlTreeItem : public BaseTreeItem
{
public:
	ReportControlTreeItem(const ReportControlPtr &p, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return RType; }

	void setName(const QString &name);
	ReportControlPtr report() const { return m_report; }

private:
	ReportControlPtr m_report;
};

//==========================================================================================
class GSEControlTreeItem : public BaseTreeItem
{
public:
	GSEControlTreeItem(const GSEControlPtr &p, BaseTreeItem *parent);

	virtual QString name() const override;
	virtual Type itemType() const override { return GSEType; }

	GSEControlPtr gseControl() const { return m_gseControl; }

private:
	GSEControlPtr m_gseControl;
};