#include "SpodesSlaveSettingsDialog.h"

#include <qdebug.h>
#include <qgridlayout.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qcombobox.h>
#include <qlabel.h>

#include <dpc/sybus/utils.h>
#include <data_model/dc_controller.h>
#include <gui/forms/interfaces_protocols/RS485/DcRs485Defines.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "SpodesSlaveWidget.h"
#include "SpodesMasterCommon.h"

using namespace Dpc::Gui;

namespace {
	const uint16_t FILE_ID = 0x5432;
	const uint16_t MAX_FILE_DATA_SIZE = 1024;
	const QString SlaveConfigFileName = "Slave";

	const uint8_t ANALOG_TYPE = 0;
	const uint8_t DISCRET_TYPE = 1;
	const uint8_t COUNTER_TYPE = 2;
	const uint8_t DISCRET_OUTPUT_TYPE = 4;
	const uint8_t SCALER_TYPE = 12;
	const uint8_t METHOD_TYPE = 13;
	const uint8_t TIMESYNC_TYPE = 14;

#pragma pack(push, 1)

	struct BaseDefinition
	{
		BaseDefinition() {}
		BaseDefinition(uint8_t l, uint8_t t, uint16_t c, uint16_t cId, uint8_t obic[6], uint8_t attr) :
			len(l), varType(t), varCount(c), reserve(0), objectCId(cId), objectOBIC{ obic[0], obic[1], obic[2], obic[3], obic[4], obic[5] }, objectAttr(attr) {}
		BaseDefinition(const QByteArray &buff) { memcpy(this, buff.data(), sizeof(BaseDefinition)); }
		uint8_t len;
		uint8_t varType;
		uint16_t varCount;
		uint16_t reserve;
		uint16_t objectCId;
		uint8_t objectOBIC[6];
		uint8_t objectAttr;

		QByteArray data() const { return QByteArray((const char*)this, sizeof(BaseDefinition)); }
		SpodesSlaveReadCommand toCommand() const { return SpodesSlaveReadCommand(objectCId, const_cast<uint8_t*>(objectOBIC), objectAttr, varCount); }
	};

	struct AnalogDefinition : public BaseDefinition
	{
		AnalogDefinition(SpodesSlaveAnalogCommand c) :
			BaseDefinition(sizeof(AnalogDefinition), ANALOG_TYPE, c.count, c.objectCId, c.objectOBIS, c.objectAttr), k0(c.k0), k1(c.k1) {}
		AnalogDefinition(const QByteArray &buff) { memcpy(this, buff.data(), sizeof(AnalogDefinition)); }
		float k0;
		float k1;

		QByteArray data() const { return QByteArray((const char*)this, sizeof(AnalogDefinition)); }
		SpodesSlaveAnalogCommand toCommand() const { return SpodesSlaveAnalogCommand(objectCId, const_cast<uint8_t*>(objectOBIC), objectAttr, varCount, k0, k1); }
	};

	struct DiscretDefinition : public BaseDefinition
	{
		DiscretDefinition(SpodesSlaveReadCommand c) :
			BaseDefinition(sizeof(DiscretDefinition), DISCRET_TYPE, c.count, c.objectCId, c.objectOBIS, c.objectAttr) {}
	};

	struct CounterDefinition : public BaseDefinition
	{
		CounterDefinition(SpodesSlaveCounterCommand c) :
			BaseDefinition(sizeof(CounterDefinition), COUNTER_TYPE, c.count, c.objectCId, c.objectOBIS, c.objectAttr), multiplicator(c.multiplicator) {}
		CounterDefinition(const QByteArray &buff) { memcpy(this, buff.data(), sizeof(CounterDefinition)); }
		uint32_t multiplicator;

		QByteArray data() const { return QByteArray((const char*)this, sizeof(CounterDefinition)); }
		SpodesSlaveCounterCommand toCommand() const { return SpodesSlaveCounterCommand(objectCId, const_cast<uint8_t*>(objectOBIC), objectAttr, varCount, multiplicator); }
	};

	struct DiscretOutputDefinition : public BaseDefinition
	{
		DiscretOutputDefinition(SpodesSlaveReadCommand c) :
			BaseDefinition(sizeof(DiscretOutputDefinition), DISCRET_OUTPUT_TYPE, c.count, c.objectCId, c.objectOBIS, c.objectAttr) {}
	};

	struct ScalerDefinition : public BaseDefinition
	{
		ScalerDefinition(SpodesSlaveReadCommand c) :
			BaseDefinition(sizeof(ScalerDefinition), SCALER_TYPE, c.count, c.objectCId, c.objectOBIS, c.objectAttr) {}
	};

	struct MethodDefinition : public BaseDefinition
	{
		MethodDefinition(SpodesSlaveMethodCommand c) :
			BaseDefinition(sizeof(BaseDefinition) + c.params.count(), METHOD_TYPE, c.params.count(), c.objectCId, c.objectOBIS, c.objectAttr), params(c.params) {}
		MethodDefinition(const QByteArray &buff) { 
			memcpy(this, buff.data(), sizeof(BaseDefinition));
			params = buff.mid(sizeof(BaseDefinition), varCount);
		}

		QByteArray data() const { return QByteArray((const char*)this, sizeof(BaseDefinition)) + params; }
		SpodesSlaveMethodCommand toCommand() const { return SpodesSlaveMethodCommand(objectCId, const_cast<uint8_t*>(objectOBIC), objectAttr, params); }

	private:
		QByteArray params;
	};

	struct TimesyncDefinition : public BaseDefinition
	{
		TimesyncDefinition(SpodesSlaveTimeSyncCommand c) :
			BaseDefinition(sizeof(TimesyncDefinition), TIMESYNC_TYPE, 0, c.objectCId, c.objectOBIS, c.objectAttr),
			timeShiftMethod(c.timeShiftMethod),
			minTimeShift(c.minTimeShift),
			maxTimeShift(c.maxTimeShift),
			syncTimeout(c.syncTimeout)
		{}
		TimesyncDefinition(const QByteArray &buff) { memcpy(this, buff.data(), sizeof(TimesyncDefinition)); }

		uint8_t timeShiftMethod;
		uint16_t minTimeShift;
		uint16_t maxTimeShift;
		uint32_t syncTimeout;

		QByteArray data() const { return QByteArray((const char*)this, sizeof(TimesyncDefinition)); }
		SpodesSlaveTimeSyncCommand toCommand() const { 
			return SpodesSlaveTimeSyncCommand(objectCId, const_cast<uint8_t*>(objectOBIC), objectAttr, timeShiftMethod, minTimeShift, maxTimeShift, syncTimeout); 
		}
	};

	struct InitDefinition
	{
		InitDefinition(const QString &password) { m_password.append(password.toUtf8()); m_password.append(char(0)); len = m_password.size(); }
		InitDefinition(const QByteArray &buff) : len(0) {
			if (!buff.size())
				return;
			len = (uint8_t) buff.at(0);
			m_password = buff.mid(1, len);
		}
		uint8_t len;

		QByteArray data() const { return QByteArray((const char*)&len, sizeof(len)) + m_password; }
		QString password() const { return m_password; }
	private:
		QByteArray m_password;
	};

	struct FileHeader {
		FileHeader(const QByteArray &definition = QByteArray()) : defineId(FILE_ID), len(definition.size()), crc(0)
		{
			if (!definition.isEmpty())
				crc = Dpc::Sybus::crc16((uint8_t *)definition.data(), definition.size(), 0);
		}
		uint16_t	defineId;
		uint16_t	len;
		uint16_t	crc;

		QByteArray data() const { return QByteArray((const char*)this, sizeof(FileHeader)); }
	};


#pragma pack(pop)

} // namespace

SpodesSlaveSettingsDialog::SpodesSlaveSettingsDialog(uint16_t portIdx, DcController *controller, QWidget *parent) :
	QDialog(parent),
	m_portIdx(portIdx),
	m_controller(controller),
	m_currentSlaveWidget(new QWidget(this)),
	m_fileManager(controller)
{
	setWindowTitle("СПОДЭС Мастер: настройка ведомых");

    auto slaveCountParam = getParam(SP_USARTPRTPAR_BYTE, m_portIdx * PROFILE_SIZE + Rs485::Indexs::Byte::SlaveCount);
	if (!slaveCountParam)
		return;

	int slaveCount = slaveCountParam->value().toInt();
	if (!slaveCount)
		return;

	QComboBox *slavesComboBox = new QComboBox(this);
    for (int i = 0; i < slaveCount; i++) {
		slavesComboBox->addItem(QString::number(i + 1));
		SpodesSlaveWidget *w = new SpodesSlaveWidget(m_controller, m_portIdx, i, this);
		w->hide();
		m_slaveWidgets << w;
	}

	QPushButton *saveConfigButton = new QPushButton("Сохранить");
	connect(saveConfigButton, &QPushButton::clicked, this, &SpodesSlaveSettingsDialog::onSaveButton);

	QGridLayout *mainLayout = new QGridLayout(this);
	int row = 0;
	
	mainLayout->addWidget(new QLabel("Ведомый"), row, 0);
	mainLayout->addWidget(slavesComboBox, row, 1);
	row++;

	mainLayout->addWidget(m_currentSlaveWidget, row, 0, 2, 4);
	row++;
	row++;

	mainLayout->addWidget(saveConfigButton, row, 3);
	mainLayout->setColumnStretch(2, 1);

	connect(slavesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SpodesSlaveSettingsDialog::onSlaveComboBoxIndexChanged);
	onSlaveComboBoxIndexChanged(0);

	loadSlaves();
	setMinimumSize(800, 550);
}

SpodesSlaveSettingsDialog::~SpodesSlaveSettingsDialog()
{
}

void SpodesSlaveSettingsDialog::onSlaveComboBoxIndexChanged(int idx)
{
	QWidget* next = m_slaveWidgets.value(idx);
	if (!next)
		return;

	layout()->replaceWidget(m_currentSlaveWidget, next);
	m_currentSlaveWidget->hide();
	m_currentSlaveWidget = next;
	m_currentSlaveWidget->show();
}

void SpodesSlaveSettingsDialog::onSaveButton()
{
    auto portsParam = getParam(SP_USARTPRTPAR_BYTE, 0);
	if (!portsParam)
		return;

    auto slavesParam = getParam(SP_USARTEXTNET_DEFINITION, 0);
	if (!slavesParam)
		return;

    uint16_t maxBytesInSlaveParam = slavesParam->parameter()->elementsCount();
    uint8_t slavesPerPort = slavesParam->parameter()->profilesCount() / portsParam->parameter()->profilesCount();
	for (uint8_t i = 0; i < m_slaveWidgets.count(); i++) {
		SpodesSlaveWidget *slaveWidget = m_slaveWidgets.at(i);

		uint16_t ainsCount = 0;
		uint16_t dinsCount = 0;
		uint16_t cinsCount = 0;
		uint16_t doutsCount = 0;
		QByteArray commandsData;
		for (auto &command : slaveWidget->scalerCommands())					// Масштабы нужно описать перывми, так было в тз.
			commandsData.append(ScalerDefinition(command).data());
		for (auto &command : slaveWidget->analogCommands()) {
			commandsData.append(AnalogDefinition(command).data());
			ainsCount += command.count;
		}
		for (auto &command : slaveWidget->discretCommands()) {
			commandsData.append(DiscretDefinition(command).data());
			dinsCount += command.count;
		}
		for (auto &command : slaveWidget->counterCommands()) {
			commandsData.append(CounterDefinition(command).data());
			cinsCount += command.count;
		}
		for (auto &command : slaveWidget->discretOutputCommands()) {
			commandsData.append(DiscretOutputDefinition(command).data());
			doutsCount += command.count;
		}
		for (auto &command : slaveWidget->methodCommands())
			commandsData.append(MethodDefinition(command).data());
		for (auto &command : slaveWidget->timeSyncCommands())
			commandsData.append(TimesyncDefinition(command).data());
		commandsData.append(char(0));

		if (!slaveWidget->password().isEmpty()) {
			commandsData.append(InitDefinition(slaveWidget->password()).data());
			commandsData.append(char(0));
		}

		bool writeToFile = sizeof(SpodesMaster::SlaveDefinition) + commandsData.size() > maxBytesInSlaveParam;
		SpodesMaster::SlaveDefinition def(writeToFile);
		def.setActive(slaveWidget->isActive());
		def.timeout = slaveWidget->timeout();
		def.logAddr = slaveWidget->logAddress();
		def.phisAddr = slaveWidget->phisAddress();
		def.addrSize = slaveWidget->addrSize();
		def.ainsCount = ainsCount;
		def.dinsCount = dinsCount;
		def.cinsCount = cinsCount;
		def.rDoutsCount = doutsCount;
		QByteArray definitionData = def.data();

		QByteArray allData = definitionData + commandsData;
		uint8_t slaveGlobalIdx = m_portIdx * slavesPerPort + i;
		if (writeToFile) {
			if (allData.size() > MAX_FILE_DATA_SIZE) {
				MsgBox::error(QString("Превышен допустимый размер конфигурационого файла(ведомый %1)").arg(i + 1));
				return;
			}

			QString dir = m_fileManager.localPath();
			if (!QDir(dir).mkpath(".")) {
				MsgBox::error(QString("Не удалось создать папку %1").arg(dir));
				return;
			}

			QString fileName = QString("%1_%2.dat").arg(SlaveConfigFileName).arg(slaveGlobalIdx);
			QString localFileName = QString("%1/%2").arg(dir).arg(fileName);
			QFile file(localFileName);
			if (!file.open(QIODevice::WriteOnly)) {
				MsgBox::error(QString("Не удалось открыть файл %1: %2").arg(localFileName).arg(file.errorString()));
				return;
			}

			FileHeader fh(allData);
			file.write(fh.data());
			file.write(allData);
			file.close();

			QString deviceFileName = m_fileManager.devicePath(fileName);
			allData = definitionData;
			allData.append(deviceFileName.toUtf8());
			allData.append(char(0));
			allData.append(char(0));
		}
		
		if (!writeToSlaveParam(allData, slaveGlobalIdx))
			return;
	}

	accept();
}

bool SpodesSlaveSettingsDialog::writeToSlaveParam(const QByteArray &data, uint8_t slaveGlobalIdx)
{
	for (int i = 0; i < data.size(); i++) {
        auto p = getParam(SP_USARTEXTNET_DEFINITION, slaveGlobalIdx * PROFILE_SIZE + i);
		if (!p)
			return false;

		p->updateValue(QString::number((uint8_t) data.at(i)));
	}	

	return true;
}

void SpodesSlaveSettingsDialog::loadSlaves()
{
    auto portsParam = getParam(SP_USARTPRTPAR_BYTE, 0);
	if (!portsParam)
		return;

    auto slavesParam = getParam(SP_USARTEXTNET_DEFINITION, 0);
	if (!slavesParam)
		return;

    uint8_t slavesPerPort = slavesParam->parameter()->profilesCount() / portsParam->parameter()->profilesCount();
    for (int i = 0; i < m_slaveWidgets.count(); i++) {
		uint32_t slaveStartIdx = (m_portIdx * slavesPerPort + i) * PROFILE_SIZE;

		QByteArray headerByteArray;
        for (size_t j = 0; j < sizeof(SpodesMaster::SlaveDefinition); j++) {
            auto p = getParam(SP_USARTEXTNET_DEFINITION, slaveStartIdx + j);
			if (!p)
				return;

			headerByteArray.append(static_cast<uint8_t>(p->value().toUShort()));
		}
		
		QByteArray commandsData;
		SpodesMaster::SlaveDefinition def(headerByteArray);
		if (def.isInFile()) {
			QString deviceFileName;
            for (int j = sizeof(def); j < slavesParam->parameter()->elementsCount(); j++) {
                auto p = getParam(SP_USARTEXTNET_DEFINITION, slaveStartIdx + j);
				if (!p)
					return;

				uint8_t byte = static_cast<uint8_t>(p->value().toUShort());
				if (!byte)
					break;

				deviceFileName.append(QChar(byte));
			}

			QString localFileName = m_fileManager.localPath(QFileInfo(deviceFileName).fileName());
			QFile file(localFileName);
			if (!file.open(QIODevice::ReadOnly)) {
				MsgBox::error(QString("Ведомый %1: Не удалось открыть файл %2: %3").arg(i + 1).arg(localFileName).arg(file.errorString()));
				continue;
			}

			QByteArray fileData = file.readAll();
			FileHeader fh;
			memcpy(&fh, fileData.data(), sizeof(fh));
			fileData.remove(0, sizeof(fh));

			if (fh.defineId != FILE_ID) {
				MsgBox::error(QString("Ведомый %1: Не верный идентификатор файла %2").arg(i + 1).arg(fh.defineId));
				continue;
			}

			if (fh.len != fileData.size()) {
				MsgBox::error(QString("Ведомый %1: Не совпадает размер файла %2").arg(i + 1).arg(fh.len));
				continue;
			}

			if (fh.crc != Dpc::Sybus::crc16((uint8_t*)fileData.data(), fileData.size(), 0)) {
				MsgBox::error(QString("Ведомый %1: Не совпадает CRC файла %2").arg(i + 1).arg(fh.crc));
				continue;
			}

			SpodesMaster::SlaveDefinition fileDef(fileData);
			if (def != fileDef) {
				MsgBox::error(QString("Ведомый %1: Не совпадают объявления в конфигурации и в файле").arg(i + 1));
				continue;
			}

			commandsData = fileData.remove(0, sizeof(SpodesMaster::SlaveDefinition));
		}
		else {
            for(int j = sizeof(def); j < slavesParam->parameter()->elementsCount(); j++) {
                auto p = getParam(SP_USARTEXTNET_DEFINITION, slaveStartIdx + j);
				if (!p)
					return;
			
				commandsData.append(static_cast<uint8_t>(p->value().toUShort()));
			}
		}

		QString password;
		QList<SpodesSlaveAnalogCommand> analogCommands;
		QList<SpodesSlaveReadCommand> discretCommands;
		QList<SpodesSlaveCounterCommand> counterCommands;
		QList<SpodesSlaveReadCommand> discretOutputCommands;
		QList<SpodesSlaveReadCommand> scalerCommands;
		QList<SpodesSlaveMethodCommand> methodCommands;
		QList<SpodesSlaveTimeSyncCommand> timeSyncCommands;
		while (commandsData.size() > 1) {
			BaseDefinition bd(commandsData);
			if (bd.len == 0) {
				password = InitDefinition(commandsData.mid(1)).password();
				break;
			}

			if (bd.varType == ANALOG_TYPE)
				analogCommands.append(AnalogDefinition(commandsData).toCommand());
			if (bd.varType == DISCRET_TYPE)
				discretCommands.append(bd.toCommand());
			if (bd.varType == COUNTER_TYPE)
				counterCommands.append(CounterDefinition(commandsData).toCommand());
			if (bd.varType == DISCRET_OUTPUT_TYPE)
				discretOutputCommands.append(bd.toCommand());
			if (bd.varType == SCALER_TYPE)
				scalerCommands.append(bd.toCommand());
			if (bd.varType == METHOD_TYPE)
				methodCommands.append(MethodDefinition(commandsData).toCommand());
			if (bd.varType == TIMESYNC_TYPE)
				timeSyncCommands.append(TimesyncDefinition(commandsData).toCommand());

			commandsData.remove(0, bd.len);
		}

		SpodesSlaveWidget *slaveWidget = m_slaveWidgets.at(i);
		slaveWidget->setActive(def.isActive());
		slaveWidget->setTimeout(def.timeout);
		slaveWidget->setLogAddress(def.logAddr);
		slaveWidget->setPhisAddress(def.phisAddr);
		slaveWidget->setAddrSize(def.addrSize);
		slaveWidget->setPassword(password);
		slaveWidget->setAnalogCommands(analogCommands);
		slaveWidget->setDiscretCommands(discretCommands);
		slaveWidget->setCounterCommands(counterCommands);
		slaveWidget->setDiscretOutputCommands(discretOutputCommands);
		slaveWidget->setScalerCommands(scalerCommands);
		slaveWidget->setMethodCommands(methodCommands);
		slaveWidget->setTimeSyncCommands(timeSyncCommands);
	}
}

ParameterElement *SpodesSlaveSettingsDialog::getParam(uint32_t addr, uint32_t idx, bool showError)
{
    auto p = m_controller->paramsRegistry().element(addr, idx);
	if (!p && showError) {
		MsgBox::error(QString("Не удалось прочитать параметр(%1:%2): параметр не найден в шаблоне устройства")
			.arg(QString("0x%1").arg(QString("%1").arg(addr, 4, 16, QChar('0')).toUpper()))
			.arg(idx));
		return nullptr;
	}

	return p;
}
