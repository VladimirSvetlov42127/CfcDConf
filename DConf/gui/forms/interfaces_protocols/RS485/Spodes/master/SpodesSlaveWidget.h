#pragma once

#include <qwidget.h>

#pragma pack(push, 1)
struct SpodesSlaveCommand
{
	SpodesSlaveCommand() : objectCId(0), objectOBIS{0,0,0,0,0,0}, objectAttr(0) {}
	SpodesSlaveCommand(uint16_t id, uint8_t obis[6], uint8_t attr) : objectCId(id), objectOBIS{obis[0], obis[1], obis[2], obis[3], obis[4], obis[5]}, objectAttr(attr) {}
	uint16_t objectCId;
	uint8_t objectOBIS[6];
	uint8_t objectAttr;
};

struct SpodesSlaveReadCommand : SpodesSlaveCommand
{
	SpodesSlaveReadCommand() : count(1) {}
	SpodesSlaveReadCommand(uint16_t id, uint8_t obis[6], uint8_t attr, uint16_t c) : SpodesSlaveCommand(id, obis, attr), count(c) {}
	uint16_t count;
};

struct SpodesSlaveAnalogCommand : SpodesSlaveReadCommand
{
	SpodesSlaveAnalogCommand() : k0(0.0), k1(0.0) {}
	SpodesSlaveAnalogCommand(uint16_t id, uint8_t obis[6], uint8_t attr, uint16_t c, float kf0, float kf1) : 
		SpodesSlaveReadCommand(id, obis, attr, c), k0(kf0), k1(kf1) {}
	float k0;
	float k1;
};

struct SpodesSlaveCounterCommand : SpodesSlaveReadCommand
{
	SpodesSlaveCounterCommand() : multiplicator(0) {}
	SpodesSlaveCounterCommand(uint16_t id, uint8_t obis[6], uint8_t attr, uint16_t c, uint32_t mul) :
		SpodesSlaveReadCommand(id, obis, attr, c), multiplicator(mul) {}
	uint32_t multiplicator;
};

struct SpodesSlaveMethodCommand : SpodesSlaveCommand
{
	SpodesSlaveMethodCommand() {}
	SpodesSlaveMethodCommand(uint16_t id, uint8_t obis[6], uint8_t attr, const QByteArray &par) : SpodesSlaveCommand(id, obis, attr), params(par) {}
	QByteArray params;
};

struct SpodesSlaveTimeSyncCommand : SpodesSlaveCommand
{
	SpodesSlaveTimeSyncCommand() : timeShiftMethod(0), minTimeShift(0), maxTimeShift(0), syncTimeout(0) {}
	SpodesSlaveTimeSyncCommand(uint16_t id, uint8_t obis[6], uint8_t attr, uint8_t tsm, uint16_t min, uint16_t max, uint32_t timeout) :
		SpodesSlaveCommand(id, obis, attr), timeShiftMethod(tsm), minTimeShift(min), maxTimeShift(max), syncTimeout(timeout) {}
	uint8_t timeShiftMethod;
	uint16_t minTimeShift;
	uint16_t maxTimeShift;
	uint32_t syncTimeout;
};
#pragma pack(pop)


class QTabWidget;
class QCheckBox;

namespace Dpc::Gui
{
	class TableView;
	class LineEdit;
	class IntSpinBox;
}

class DcController;
class ConnectionDiscretWidget;
class ListParamEditor;

class SpodesSlaveWidget : public QWidget
{
	Q_OBJECT
public:
	SpodesSlaveWidget(DcController *controller, int interfaceIdx, int deviceIdx, QWidget *parent = nullptr);
	~SpodesSlaveWidget();

	bool isActive() const;
	uint8_t timeout() const;
	uint8_t addrSize() const;
	uint16_t logAddress() const;
	uint16_t phisAddress() const;
	QString password() const;

	void setActive(bool active);
	void setTimeout(uint8_t value);
	void setLogAddress(uint16_t value);
	void setPhisAddress(uint16_t value);
	void setAddrSize(uint8_t value);
	void setPassword(const QString &value);

	QList<SpodesSlaveAnalogCommand> analogCommands() const;
	QList<SpodesSlaveReadCommand> discretCommands() const;
	QList<SpodesSlaveCounterCommand> counterCommands() const;
	QList<SpodesSlaveReadCommand> discretOutputCommands() const;
	QList<SpodesSlaveReadCommand> scalerCommands() const;
	QList<SpodesSlaveMethodCommand> methodCommands() const;
	QList<SpodesSlaveTimeSyncCommand> timeSyncCommands() const;

	void setAnalogCommands(const QList<SpodesSlaveAnalogCommand> &commands);
	void setDiscretCommands(const QList<SpodesSlaveReadCommand> &commands);
	void setCounterCommands(const QList<SpodesSlaveCounterCommand> &commands);
	void setDiscretOutputCommands(const QList<SpodesSlaveReadCommand> &commands);
	void setScalerCommands(const QList<SpodesSlaveReadCommand> &commands);
	void setMethodCommands(const QList<SpodesSlaveMethodCommand> &commands);
	void setTimeSyncCommands(const QList<SpodesSlaveTimeSyncCommand> &commands);

protected:
	void showEvent(QShowEvent *event) override;

private slots:
	void onAddButton();
	void onRemoveButton();

private:
	QTabWidget *m_tabWidget;
	Dpc::Gui::TableView *m_analogsTableView;
	Dpc::Gui::TableView *m_discretsTableView;
	Dpc::Gui::TableView *m_countersTableView;
	Dpc::Gui::TableView *m_discretOuputsTableView;
	Dpc::Gui::TableView *m_scalersTableView;
	Dpc::Gui::TableView *m_methodsTableView;
	Dpc::Gui::TableView *m_timeSyncTableView;

	QCheckBox *m_activeCheckBox;
	Dpc::Gui::IntSpinBox *m_timeoutEdit;
	Dpc::Gui::IntSpinBox *m_logAddressEdit;
	Dpc::Gui::IntSpinBox *m_phisAddressEdit;
	ListParamEditor *m_addrSizeComboBox;
	Dpc::Gui::LineEdit *m_passwordEdit;

	ConnectionDiscretWidget *m_connectionDiscretWidget;
};