#include "dc_controller.h"

#include <unordered_map>

#include "db/dc_db_manager.h"

using namespace Dpc::Sybus;

namespace {

    const char* g_SettingKey_C1 = "C1";

	struct SpecialParam
	{
		bool base;
		bool updatable;
	};

	const std::unordered_map<uint16_t, SpecialParam> g_SpecialParams = {
        { SP_DEVICE_NAME,		{ true,  false } },	// Название устройства
        { SP_HDWPARTNUMBER,		{ true,  false } },	// Код устройства
        { SP_SOFTWARE_ID,		{ true,  false } },	// Код ПО
        { SP_SERNUM,			{ true,  false } },	// Серийный номер
        { SP_SOFTWARE_VERTION,	{ true,  true  } },	// Сборка ПО
        { SP_CFGVER,			{ true,  true  } },	// Версия конфигурации ПО устройства
        { SP_DOUT_FUNCLIST,		{ false, true  } },	// Список поддерживаемых функций виртуальных выходов
        { SP_61850_SUPLIST,     { false, true  } },	// Состав протокола 61850
        { SP_OSC_LIMITS,        { false, true  } },	// Особенности и ограничения осциллографа
	};

	const QHash<int, QString> g_VirtualFuncions = {
		{DcController::NOTUSE, "Не используется"},
		{DcController::TEST_CNTRL, "Пуск теста цепей управления"},
		{DcController::OSCILL_START, "Пуск осциллографирования"},
		{DcController::VDIN_CONTROL, "Управление виртуальным входом"},
		{DcController::XCBR_CNTRL, "Управление выключателем"},
		{DcController::XCBR_RZA_CNTRL, "Управление выключателем c РЗА"},
		{DcController::QUIT_CMD, "Квитация событий"},
		{DcController::FIX_VDIN, "Фиксация входа"},
		{DcController::VDOUT_CONFIRM, "Квитация события"},
		{DcController::VDIN_EVENT, "Событие в виртуальном входе"},
		{DcController::EXEC_EMBEDED_ALG, "Запуск встроенного алгоритма"},
		{DcController::NETWUSE,	"Штатное управление"},
		{DcController::CHANGE_SIM, "Смена СИМ карты"},
		{DcController::BLOCK_TU, "Запретить ТУ"}, 
		{DcController::CONTROL_SV, "Управление SV потоком"},
		{DcController::ACTIVE_GROUP, "Установка активной группы уставок"}
	};
}

DcController::DcController(const QString &filePath)
    : DcController(filePath, QString())
{
}

DcController::DcController(const QString& filepath, const QString &name)
    : m_uid(0)
    , m_path(filepath)
    , m_name(name)
{
    m_algs_internal = new DcPoolSingleKey<DcAlgInternal*>();
    m_algs_cfc = new DcPoolSingleKey<DcAlgCfc*>();
    m_matrix_alg = new DcMatrixPool<DcMatrixElementAlg*>();
    m_matrix_cfc = new DcMatrixPool<DcMatrixElementAlgCfc*>();
    m_matrix_signals = new DcMatrixPool<DcMatrixElementSignal*>();
    _service_manager = new ServiceManager(this);

    m_params.setDevice(this);
}

DcController::~DcController()
{
    gDbManager.close(uid());
//    qDebug() << "~DcController" << uid() << name() << path();

	delete m_matrix_signals;
	delete m_matrix_cfc;
    delete m_matrix_alg;
	delete m_algs_cfc;
	delete m_algs_internal;
	delete _service_manager;
}

int32_t DcController::uid() const
{
    return m_uid;
}

void DcController::setUid(int32_t uid)
{
    m_uid = uid;
}

QString DcController::path() const
{
    return m_path;
}

void DcController::setPath(const QString &path)
{
    m_path = path;
}

QString DcController::name() const
{
    return m_name;
}

QIcon DcController::icon() const
{
    switch (type())
    {
    case DcController::Deprotec:   return QIcon(":/icons/dep_deprotec.svg");
    case DcController::LT:         return QIcon(":/icons/dep_lt.svg");
    case DcController::GSM:        return QIcon(":/icons/dep_p-gsm.svg");
    case DcController::P_SC:       return QIcon(":/icons/dep_p-sc.svg");
    case DcController::P_SCI:      return QIcon(":/icons/dep_p-sci.svg");
    case DcController::DepRtu:     return QIcon(":/icons/dep_rtu.svg");
    case DcController::ExRza:      return QIcon(":/icons/dep_exrza.svg");
    case DcController::ExSW:       return QIcon(":/icons/dep_ex-sw.svg");
    case DcController::RTU3_M_P:   return QIcon(":/icons/dep_rtu3-m.svg");
    case DcController::T2:         return QIcon(":/icons/dep_t-module.svg");
    case DcController::T2_DOUT_8R: return QIcon(":/icons/dep_t-module.svg");
    case DcController::T2_EM_3M:   return QIcon(":/icons/dep_t-module.svg");
    case DcController::RPR_485_T3: return QIcon(":/icons/dep_t-module.svg");
    default: break;
    }

    return QIcon();
}

void DcController::beginTransaction() const
{
    if (!uid())
        return;

    gDbManager.beginTransaction(uid());
}

void DcController::endTransaction() const
{
    if (!uid())
        return;

    gDbManager.endTransaction(uid());
}

ParameterRegistry &DcController::paramsRegistry()
{
    return m_params;
}

const ParameterRegistry &DcController::paramsRegistry() const
{
    return m_params;
}

const DcBoard::UPtrVector &DcController::boards() const
{
    return m_boards;
}

DcBoard *DcController::board(int32_t id) const
{
    if (id >= 0)
        for(auto &board: boards())
            if (board->id() == id)
                return board.get();

    return nullptr;
}

void DcController::append(DcBoard::UPtr board)
{
    if (!board)
        return;

    board->setDevice(this);
    board->insert();
    m_boards.emplace_back(std::move(board));
}

void DcController::remove(DcBoard *board)
{
    if (!board)
        return;

    board->remove();
    auto newEnd = std::remove_if(m_boards.begin(), m_boards.end(), [=](const DcBoard::UPtr &b) { return b.get() == board; });
    m_boards.erase(newEnd, m_boards.end());
}

const DcSetting::UPtrMap &DcController::settings() const
{
    return m_settings;
}

DcSetting *DcController::setting(const QString &settingName) const
{
    if (auto findIt = settings().find(settingName); findIt != settings().end())
        return findIt->second.get();

    return nullptr;
}

QString DcController::settingValue(const QString &settingName, const QString &defaultValue) const
{
    if (auto setting = this->setting(settingName); setting)
        return setting->value();

    return defaultValue;
}

void DcController::setSetting(const QString &settingName, const QString &value)
{
    auto setting = this->setting(settingName);
    if (setting) {
        setting->updateValue(value);
        return;
    }

    auto newSetting = std::make_unique<DcSetting>(settingName, value, this);
    newSetting->insert();
    m_settings.emplace(settingName, std::move(newSetting));
}

void DcController::removeSetting(const QString &settingName)
{
    auto findIt = settings().find(settingName);
    if (findIt == settings().end())
        return;

    findIt->second->remove();
    m_settings.erase(findIt);
}

DcPoolSingleKey<DcAlgCfc*>* DcController::algs_cfc() const
{
	return m_algs_cfc;
}

DcPoolSingleKey<DcAlgInternal*>* DcController::algs_internal() const
{
	return m_algs_internal;
}

DcMatrixPool<DcMatrixElementAlg*>* DcController::matrix_alg() const
{
	return m_matrix_alg;
}

DcMatrixPool<DcMatrixElementAlgCfc*>* DcController::matrix_cfc() const
{
	return m_matrix_cfc;
}

DcMatrixPool<DcMatrixElementSignal*>* DcController::matrix_signals() const
{
	return m_matrix_signals;
}

bool DcController::updateName(const QString &newname)
{
	if (newname == m_name) return true;

	m_name = newname;
	emit nameChanged(newname);
	return true;
}

DcController::Type DcController::type() const
{
    QString value = this->settingValue("model").trimmed().toUpper();

	if (value.contains("DEPROTEC"))	return Deprotec;
	if (value == "DEPRTU-LT-GSM") return GSM;
	if (value.contains("DEPRTU-LT-")) return LT;
	if (value == "DEPRTU-P-SC")	return P_SC;
	if (value == "DEPRTU-P-SCI") return P_SCI;	
	if (value == "DEPRTU") return DepRtu;
	if (value == "EXRZA") return ExRza;
	if (value == "EXSW") return ExSW;
	if (value == "RTU3-M") return RTU3_M_P;
    if (value.startsWith("T2-")) return T2;
	if (value == "T2-DOUT-8R")	return T2_DOUT_8R;
	if (value == "T2-EM3M")		return T2_EM_3M;
	if (value == "RPR-485-T3") return RPR_485_T3;

	return Unknown;
}

bool DcController::hasEthernet() const
{
    auto ipParam = paramsRegistry().element(SP_IP4_ADDR, 0);
    if (!ipParam)
        return false;

    if (DcController::Deprotec == type()) {
        for(auto &board: boards()) {
            auto boardType = board->type().trimmed();
            if ( "DRZA_2FX" == boardType || "DRZA_2TX" == boardType )
                return true;
        }

        return false;
    }

    return true;
}

bool DcController::isC1Panel() const
{
    return setting(g_SettingKey_C1);
}

void DcController::setC1Panel(bool c1)
{
    if (!c1) {
        removeSetting(g_SettingKey_C1);
        return;
    }

    setSetting(g_SettingKey_C1, QString::number(1));

    auto param = paramsRegistry().element(SP_DISPLAY_KEY_FUNC, 2);
    if (param)
        param->updateValue(QString::number(1));

    param = paramsRegistry().element(SP_DISPLAY_KEY_FUNC, 3);
    if (param)
        param->updateValue(QString::number(2));
}

uint DcController::rs485PortsCount() const
{
    auto param = paramsRegistry().parameter(SP_USARTPRTPAR_BYTE);
    if (!param)
        return 0;

    return param->profilesCount();
}

uint DcController::rs485SlavesMax() const
{
    auto param = paramsRegistry().parameter(SP_USARTEXTNET_DEFINITION);
    if (!param)
        return 0;

    return param->profilesCount();
}

QVariant DcController::getValue(int32_t addr, uint16_t index) const
{
    auto param = paramsRegistry().element(addr, index);
	if (!param)
		return QVariant();

	return param->value();
}

bool DcController::setValue(int32_t addr, uint16_t index, const QVariant & value)
{
    auto param = paramsRegistry().element(addr, index);
	if (!param)
		return false;

	param->updateValue(value.toString());
	return true;
}

QVariant DcController::getBitValue(int32_t addr, uint16_t bit, int profile, int32_t addrIndex) const
{	
	auto p = getParamForBit(addr, bit, profile, addrIndex);
	auto param = p.first;
	bit = p.second;

	if (!param || 0 > bit)
		return QVariant();

	return param->value().toUInt() & (1 << bit) ? Qt::Checked : Qt::Unchecked;
}

bool DcController::setBitValue(int32_t addr, uint16_t bit, const QVariant & value, int profile, int32_t addrIndex)
{
	auto p = getParamForBit(addr, bit, profile, addrIndex);
	auto param = p.first;
	bit = p.second;

	if (!param || 0 > bit)
		return false;	

	uint32_t val = param->value().toUInt();
	if (value.toBool())
		val |= 1 << bit;
	else
		val &= ~(1 << bit);

	param->updateValue(QString::number(val));
	return true;
}

QSet<uint16_t> DcController::specialParams(DcController::SpecialParamType type)
{
	QSet<uint16_t> result;
	for (auto &&it : g_SpecialParams) {
		switch (type) {
		case DcController::BaseParam: if (it.second.base) result.insert(it.first); break;
		case DcController::UpdatableParam: if (it.second.updatable) result.insert(it.first); break;
		default: result.insert(it.first); break;
		}
	}

	return result;
}

std::vector<std::pair<uint, QString>> DcController::virtualFunctionList() const
{
	std::vector<std::pair<uint, QString>> result;

    auto vfListParam = paramsRegistry().parameter(SP_DOUT_FUNCLIST);
	if (!vfListParam)
		return result;

    auto softIdParam = paramsRegistry().element(SP_SOFTWARE_ID, 0);
	if (!softIdParam)
		return result;

	auto softId = softIdParam->value().toUInt();
	auto isInvalidLtOscill = softId == DEPRTU_LT_T || softId == DEPRTU_LTA_T || softId == DEPRTU_LT_R || softId == DEPRTU_LTA_R || 
		softId == DEPRTU_LT_F || softId == DEPRTU_LTA_F;

    for (size_t i = 0; i < vfListParam->elementsCount(); i++) {
        auto p = paramsRegistry().element(SP_DOUT_FUNCLIST, i);
		auto funcId = p->value().toUInt();
		if (funcId == OSCILL_START && isInvalidLtOscill)
			continue;

		QString funcName = g_VirtualFuncions.value(funcId);
		if (funcName.isEmpty())
			continue;

		result.emplace_back(std::make_pair(i, funcName));
	}

	return result;
}

DcController::VirtualFunctionType DcController::virtualFunctionType(int functionIndex) const
{
    auto vfListParam = paramsRegistry().element(SP_DOUT_FUNCLIST, functionIndex);
	if (vfListParam) {
		int value = vfListParam->value().toUInt();
		if (g_VirtualFuncions.contains(value))
			return static_cast<VirtualFunctionType>(value);
	}

	return UNKNOWN;
}

bool DcController::isConnectionDiscret(DcSignal * signal) const
{
	if (signal->type() != DEF_SIG_TYPE_DISCRETE)
		return false;

    auto connectionDiscretParam = paramsRegistry().parameter(SP_INDCON_OUTWORD_PARAM);
	if (connectionDiscretParam)
        for (size_t i = 0; i < connectionDiscretParam->elementsCount(); i++) {
            auto p = paramsRegistry().element(SP_INDCON_OUTWORD_PARAM, i);
			if (p->value().toUInt() == signal->internalId())
				return true;
		}

	return false;
}

bool DcController::isVirtualFunctionParamValue(VirtualFunctionType funcType, int value) const
{
    auto virtualFunctionsParam = paramsRegistry().parameter(SP_DOUT_TYPE);
	if (virtualFunctionsParam)
        for (size_t i = 0; i < virtualFunctionsParam->profilesCount(); i++) {
            auto p = paramsRegistry().element(SP_DOUT_TYPE, i * PROFILE_SIZE);
			if (funcType != virtualFunctionType(p->value().toUInt()))
				continue;

            p = paramsRegistry().element(SP_DOUT_TYPE, i * PROFILE_SIZE + 1);
			if (p->value().toUInt() == value)
				return true;
		}

	return false;
}

int DcController::virtualDiscreteIndex(DcSignal * virtualSignal) const
{
	int index = 0;
	for (auto s: getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_VIRTUAL)) {
		if (s == virtualSignal)
			return index;

		index++;		
	}

	return -1;
}

QVariant DcController::getTimeOffset() const
{
    auto p = paramsRegistry().element(SP_TIMEZONE_HOUR, 0);
	if (p)
		return p->value().toInt();

    p = paramsRegistry().element(SP_PROTOCOL, 0);
	if (!p)
		return QVariant();

	auto syncProtocol = p->value().toUShort();
    if (syncProtocol < Protocol_SNTPv4) {
        if (p = paramsRegistry().element(SP_TIMESYNCROPARS, 13); p)
			return p->value().toInt();
	}
    else if (syncProtocol == Protocol_SNTPv4) {
        if (p = paramsRegistry().element(SP_TIMESYNCROPARS, 256 + 6); p)
			return p->value().toInt();
	}

	return QVariant();
}

bool DcController::addSignal(DcSignal* signal, bool fromDb)
{
	auto container = getContainer(signal->type(), signal->direction());
	if (!container)
		return false;

	auto insertIt = container->insert({ signal->internalId(), signal });
	if (insertIt.second && !fromDb)
		signal->insert();
	return insertIt.second;
}

bool DcController::removeSignal(int32_t internalId, DefSignalType type, DefSignalDirection direction)
{
	auto container = getContainer(type, direction);
	if (!container)
		return false;

	auto findIt = container->find(internalId);
	if (findIt == container->end())
		return false;

	findIt->second->remove();
	container->erase(findIt);
	return true;
}

DcSignal* DcController::getSignal(int32_t index) const
{
	auto searchContainer = [](int32_t index, const SignalContainer &container) -> DcSignal* {
		for (auto &it: container)
			if (it.second->index() == index)
				return it.second;
		return nullptr;
	};

	if (auto res = searchContainer(index, m_inDiscrets); res)
		return res;
	if (auto res = searchContainer(index, m_outDiscrets); res)
		return res;
	if (auto res = searchContainer(index, m_inAnalogs); res)
		return res;
	if (auto res = searchContainer(index, m_inCounters); res)
		return res;

	return nullptr;
}

DcSignal* DcController::getSignal(int32_t internalId, DefSignalType type, DefSignalDirection direction) const
{
	auto container = getContainer(type, direction);
	if (!container)
		return nullptr;

	auto findIt = container->find(internalId);
	if (findIt == container->end())
		return nullptr;

	return findIt->second;
}

QList<DcSignal*> DcController::getSignalList() const
{
	QList<DcSignal*> result;
	auto collectSignals = [&result](const SignalContainer& container) {
		for (auto& s : container)
			result << s.second;
	};

	collectSignals(m_inDiscrets);
	collectSignals(m_inAnalogs);
	collectSignals(m_inCounters);
	collectSignals(m_outDiscrets);
	return result;
}

QList<DcSignal*> DcController::getSignalList(DefSignalType type, DefSignalSubType subType, DefSignalDirection direction) const
{
	QList<DcSignal*> result;
	auto container = getContainer(type, direction);
	if (container)
		for (auto& it : *container) {
			auto s = it.second;
			if (DEF_SIG_SUBTYPE_UNDEF == subType || s->subtype() == subType)
				result << it.second;
		}

    return result;
}

DcController *DcController::clone() const
{
    auto tempDevice = new DcController(path(), name());

    // настройки
    for (auto &[settingName, setting]: settings()) {
        tempDevice->setSetting(setting->name(), setting->value());
    }

    // сигналы
    for (auto sg: getSignalList()) {
        auto newSg = new DcSignal(sg->index(), sg->internalId(), sg->direction(), sg->type(), sg->subtype(), sg->name(), sg->properties().toJson(), tempDevice);
        tempDevice->addSignal(newSg, true);
    }

    // встроенные алгоритмы
    for (size_t i = 0; i < algs_internal()->size(); i++) {
        auto alg = algs_internal()->get(i);
        auto newAlg = new DcAlgInternal(alg->index(), alg->position(), alg->name(), alg->properties().toJson(), tempDevice);

        for (size_t j = 0; j < alg->ios()->size(); j++) {
            auto ios = alg->ios()->get(j);
            auto newIos = new DcAlgIOInternal(ios->index(), ios->alg(), ios->pin(), ios->direction(), ios->name(), tempDevice);
            newAlg->ios()->add(newIos, true);
        }

        tempDevice->algs_internal()->add(newAlg, true);
    }

    // платы
    for (auto &board: boards()) {
        auto newBoard = std::make_unique<DcBoard>(board->id(), board->slot(), board->type(), board->inst(), board->properties(), tempDevice);
        for(auto& [addr, param]: board->paramsRegistry()) {
            newBoard->paramsRegistry().add(param->clone());
        }

        tempDevice->append(std::move(newBoard));
    }

    // параметры
    for (auto& [addr, param]: paramsRegistry()) {
        tempDevice->paramsRegistry().add(param->clone());
    }

    return tempDevice;
}

DcController::SignalContainer* DcController::getContainer(DefSignalType type, DefSignalDirection direction) const
{
	switch (type)
	{
	case DEF_SIG_TYPE_DISCRETE:
		if (direction == DEF_SIG_DIRECTION_INPUT)
			return const_cast<SignalContainer*>(&m_inDiscrets);
		else if (direction == DEF_SIG_DIRECTION_OUTPUT)
			return const_cast<SignalContainer*>(&m_outDiscrets);
		break;
	case DEF_SIG_TYPE_ANALOG:
		return const_cast<SignalContainer*>(&m_inAnalogs);
	case DEF_SIG_TYPE_COUNTER:
		return const_cast<SignalContainer*>(&m_inCounters);
	default:
		break;
	}

	return nullptr;
}

std::pair<ParameterElement *, int> DcController::getParamForBit(int addr, int bit, int profile, int addrIndex) const
{
	int indexBase = profile * PROFILE_SIZE;
	if (addrIndex > -1) {
        auto param = paramsRegistry().element(addr, addrIndex + indexBase);
		return { param, bit };
	}

    auto param = paramsRegistry().element(addr, 0);
	if (!param)
		return { nullptr, -1 };

	uint16_t bitsPerParam = 0; // количество битов в параметре
	switch (param->type()) {
    case T_BYTE:
    case T_CHAR:
    case T_BYTEBOOL:
    case T_BOOL:
    case T_8BIT:
    case T_SBYTE:
		bitsPerParam = 8;
		break;
    case T_SHORT:
    case T_16BIT:
    case T_WORD:
		bitsPerParam = 16;
		break;
    case T_INTEGER:
    case T_DWORD:
    case T_LONG:
    case T_32BIT:
		bitsPerParam = 32;
		break;
	default: return { nullptr, -1 };
	}

	addrIndex = (bit / bitsPerParam) + indexBase; // индекс параметра в котрорм содержится нужный бит.
	bit = bit % bitsPerParam; // номер бита в параметре.
    param = paramsRegistry().element(addr, addrIndex);
	return { param, bit };
}

void DcController::loadServiceManager()
{
    _service_manager->init();
}
