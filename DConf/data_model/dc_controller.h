#pragma once

#include <stdint.h>
#include <qvariant.h>
#include <qset.h>
#include <memory>

#include <QIcon>

#include "data_model/parameters/parameter_registry.h"
#include "data_model/dc_board.h"
#include "data_model/dc_setting.h"

#include <data_model/dc_pool.h>
#include <data_model/dc_alg_internal.h>
#include <data_model/dc_alg_cfc.h>
#include <data_model/dc_signal.h>
#include <data_model/dc_matrix_pool.h>
#include <data_model/dc_matrix_element_alg.h>
#include <data_model/dc_matrix_element_alg_cfc.h>
#include <data_model/dc_matrix_element_signal.h>

#include "service_manager/service_manager.h"

//	Добавление менеджера сервисов
class ServiceManager;

//	Класс DcController
class DcController : public QObject
{
	Q_OBJECT

public:
	enum Type {
		Unknown,
		Deprotec, 
		LT,
		GSM,
		P_SC, 
		P_SCI,
		DepRtu,
		ExRza, 
		ExSW,
		RTU3_M_P,
        T2,
		T2_DOUT_8R,
		T2_EM_3M,
		RPR_485_T3,

	};

	enum Rs485Protocol {
		NotSet = 0x0,
		SybusSlaveProtocol,
		Iec101SlaveProtocol,
		ModbusSlaveProtocol,
		SybusMasterProtocol,
		Iec101MasterProtocol,
		ModbusMasterPrototcol,
		HayesModemMasterProtocol,
		Iec103SlaveProtocol,
		SpodesSlaveProtocol,
		SpodesMasterProtocol,
		ModemNanotronProtocol,
		ComPortProtocol
	};

	enum SpecialParamType {
		AllParam,
		BaseParam, 
		UpdatableParam
	};

	enum VirtualFunctionType {
		NOTUSE = 0,				// "Не используется";
		TEST_CNTRL,				// "Пуск теста цепей управления";				
		OSCILL_START,			// "Пуск осциллографирования";				
		VDIN_CONTROL,			// "Управление виртуальным входом";	
		XCBR_CNTRL,				// "Управление выключателем";				
		XCBR_RZA_CNTRL,			// "Управление выключателем c РЗА";			
		QUIT_CMD,				// "Квитация событий";							
		FIX_VDIN,				// "Фиксация входа";	
		VDOUT_CONFIRM,			// "Квитация события";
		VDIN_EVENT,				// "Событие в виртуальном входе";
		EXEC_EMBEDED_ALG,		// "Запуск встроенного алгоритма";	
		NETWUSE,				// "Штатное управление";
		CHANGE_SIM,				// "Смена СИМ карты";					
		BLOCK_TU,				// "Запретить ТУ";
		CONTROL_SV = 15,		// "Управление SV потоком"
		ACTIVE_GROUP,			// "Активная группа уставок"

		UNKNOWN
	};

    enum TimeSyncProtocols {
        Protocol_IEEE1588v2_UDP = 0,
        Protocol_IEEE1588v2_802_3,
        Protocol_SNTPv4,
        Protocol_1PPS,
        Protocol_SYBUS_RTU,
        Protocol_IEC101_104vGSM,
        Protocol_IEC104,
        Protocol_SYBUS_TCP,
        Protocol_MODBUS_RTU,
        Protocol_MODBUS_TCP,
        Protocol_SNTP_GPRS,
        Protocol_IEC103,
        Protocol_GSM,

        Protocol_NoUse = 0xFF
    };

    using UPtr = std::unique_ptr<DcController>;

    DcController(const QString& filePath = QString());
    DcController(const QString& filepath, const QString &name);
	virtual ~DcController();

    // Уникальный идентификатор для работы с файлом базы данных. Выставляется gDbManager автоматически!!!
    // Если равен 0, при изменение любых данных, параметров, плат, сигналов, привязок ничего не пишется в базу.
    int32_t uid() const;
    void setUid(int32_t uid);

	QString path() const;
    void setPath(const QString &path);

	QString name() const;
    QIcon icon() const;

    ParameterRegistry& paramsRegistry();
    const ParameterRegistry& paramsRegistry() const;

    const DcBoard::UPtrVector& boards() const;
    DcBoard* board(int32_t id) const;
    void append(DcBoard::UPtr board);
    void remove(DcBoard* board);

    const DcSetting::UPtrMap& settings() const;
    DcSetting* setting(const QString &settingName) const;
    QString settingValue(const QString&settingName, const QString &defaultValue = QString()) const;
    void setSetting(const QString &settingName, const QString &value);
    void removeSetting(const QString &settingName);

	DcPoolSingleKey<DcAlgCfc*>* algs_cfc() const;
    DcPoolSingleKey<DcAlgInternal*>* algs_internal() const;
	DcMatrixPool<DcMatrixElementAlg*> *matrix_alg() const;
	DcMatrixPool<DcMatrixElementAlgCfc*> *matrix_cfc() const;
	DcMatrixPool<DcMatrixElementSignal*> *matrix_signals() const;

    bool updateName(const QString &newname);

    Type type() const;
    bool hasEthernet() const;

    bool isC1Panel() const;
    void setC1Panel(bool c1);

	uint rs485PortsCount() const;
	uint rs485SlavesMax() const;

	QVariant getValue(int32_t addr, uint16_t index) const;
	bool setValue(int32_t addr, uint16_t index, const QVariant &value);
	QVariant getBitValue(int32_t addr, uint16_t bit, int profile = 0, int32_t addrIndex = -1) const;
	bool setBitValue(int32_t addr, uint16_t bit, const QVariant &value, int profile = 0, int32_t addrIndex = -1);

	static QSet<uint16_t> specialParams(DcController::SpecialParamType type = AllParam);

	std::vector<std::pair<uint, QString>> virtualFunctionList() const;
	VirtualFunctionType virtualFunctionType(int functionIndex) const;
	bool isConnectionDiscret(DcSignal * signal) const;
	bool isVirtualFunctionParamValue(VirtualFunctionType funcType, int value) const;
	int virtualDiscreteIndex(DcSignal *virtualSignal) const;

	QVariant getTimeOffset() const;

	// Сигналы
	bool addSignal(DcSignal* signal, bool fromDb);
	bool removeSignal(int32_t internalId, DefSignalType type, DefSignalDirection direction = DEF_SIG_DIRECTION_INPUT);	
	DcSignal* getSignal(int32_t index) const;
	DcSignal* getSignal(int32_t internalId, DefSignalType type, DefSignalDirection direction = DEF_SIG_DIRECTION_INPUT) const;
	QList<DcSignal*> getSignalList() const;
	QList<DcSignal*> getSignalList(DefSignalType type, DefSignalSubType subType = DEF_SIG_SUBTYPE_UNDEF, DefSignalDirection direction = DEF_SIG_DIRECTION_INPUT) const;

    DcController* clone() const;

public slots:
    // Для быстрого изменения большого количества параметров, в рамках транзакции. !!!КОСТЫЛЬ ДЛЯ ОПТИМИЗАЦИИ!!!
    void beginTransaction() const;
    void endTransaction() const;

signals:
	void nameChanged(const QString &name);

private:
	using SignalContainer = std::map<int32_t, DcSignal*>;
	SignalContainer* getContainer(DefSignalType type, DefSignalDirection direction = DEF_SIG_DIRECTION_INPUT) const;

	// Возвращает пару, искомый параметр и номер бита в нём
    std::pair<ParameterElement*, int> getParamForBit(int addr, int bit, int profile, int addrIndex) const;

private:
    int32_t m_uid;
	QString m_path;
	QString m_name;

    ParameterRegistry m_params;
    DcBoard::UPtrVector m_boards;
    DcSetting::UPtrMap m_settings;

	DcPoolSingleKey<DcAlgInternal*> *m_algs_internal;
    DcPoolSingleKey<DcAlgCfc*> *m_algs_cfc;
	DcMatrixPool<DcMatrixElementAlg*> *m_matrix_alg;
	DcMatrixPool<DcMatrixElementAlgCfc*> *m_matrix_cfc;
	DcMatrixPool<DcMatrixElementSignal*> *m_matrix_signals;

	SignalContainer m_inDiscrets;
	SignalContainer m_inAnalogs;
	SignalContainer m_inCounters;
	SignalContainer m_outDiscrets;

public:
	//===============================================================================================================================================
	//	Дополнительные методы класса
	//===============================================================================================================================================
	const SignalContainer& inDiscrets() { return m_inDiscrets; }
	const SignalContainer& outDiscrets() { return m_outDiscrets; }
	void loadServiceManager();
    ServiceManager* serviceManager() const { return _service_manager; }

private:
	//===============================================================================================================================================
	//	Свойства класса
	//===============================================================================================================================================
    ServiceManager* _service_manager = nullptr;

};

