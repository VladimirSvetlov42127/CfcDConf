#pragma once

#include <qtabwidget.h>

#include <data_model/dc_controller.h>
#include <report/DcIConfigReport.h>

#include <gui/forms/interfaces_protocols/RS485/DcRs485Defines.h>
#include <gui/editors/EditorsManager.h>

class DcRs485BaseProtocol
{
public:
	DcRs485BaseProtocol(DcController *device, int portIdx, const QList<int> &exchangeSpeedList);
	virtual ~DcRs485BaseProtocol();

	DcController* device() const { return m_device; }
	int portIdx() const { return m_portIdx; }
	int indexBase() const { return m_indexBase; }
	ListEditorContainer speedList() const { return m_speedList; }

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main);

private:
	DcController* m_device;
	int m_portIdx;
	int m_indexBase;
	ListEditorContainer m_speedList;	
};

class DcRs485BaseProtocolWidget : public QTabWidget
{
	Q_OBJECT

public:
	DcRs485BaseProtocolWidget(DcRs485BaseProtocol *protocol, QWidget *parent = nullptr);
	~DcRs485BaseProtocolWidget();

protected:
	//virtual DcRs485BaseProtocol* protocol() const { return m_protocol; }
	DcController* device() const { return m_protocol->device(); }
	int portIdx() const { return m_protocol->portIdx(); }
	int indexBase() const { return m_protocol->indexBase(); }

	QWidget* mainWidget() const { return m_mainWidget; }
	QLabel* addressLabel() const { return m_addressLabel; }
	LineParamEditor* inputBufferEditor() const { return m_inputBufferEditor; }
	LineParamEditor* outputBufferEditor() const { return m_outputBufferEditor; }
	LineParamEditor* addressEditor() const { return m_addressEditor; }

public:
	DcRs485BaseProtocol* m_protocol;

	QWidget* m_mainWidget;
	QLabel *m_addressLabel;
	LineParamEditor *m_inputBufferEditor;
	LineParamEditor *m_outputBufferEditor;
	LineParamEditor *m_addressEditor;
};
