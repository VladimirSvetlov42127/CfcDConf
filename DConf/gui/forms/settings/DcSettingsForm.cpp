#include "DcSettingsForm.h"

#include <qtabwidget.h>
#include <qabstractitemmodel.h>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

#include <gui/editors/EditorsManager.h>

using namespace Dpc::Gui;

namespace {
    enum ConnectionScheme {
        SchemeInvalid = -1,
        SchemeStar = 0,
        SchemeTriangle
    };

    enum WindingType {
        Winding_2,
        Winding_3
    };

    const char* SCHEME_STAR_SIGN = "Y";
    const char* SCHEME_TRIANGLE_SIGN = "Δ";

	const ListEditorContainer g_VoltageList = { "Ua Uc 3Uo", "Uab Ubc 3Uo", "Ua Ub Uс", "Ua Ub Uc 3Uo", "Uca" };
	const ListEditorContainer g_AmperageList = { "Ia, Ib, Ic", "Ia, Ic, 3Io", "Ia, Ib, Ic, 3Io" };

	const ListEditorContainer g_VoltageList_PSCI = { {"Ua Ub Uс", 2} };
	const ListEditorContainer g_AmperageList_PSCI = { {"Ia, Ib, Ic", 0}, {"Ia, Ib, Ic, 3Io", 2} };

	const ListEditorContainer g_PhaseList = { "Прямое", "Обратное" };

	const ListEditorContainer g_ObjectTypeList = { "Трансформатор", "Реактор", "Шины" };
    const ListEditorContainer g_AFE7_ObjectGroupConnectionList = { { "0", "1", "5", "6", "7", "11" }, true };
    const ListEditorContainer g_AFE11_ObjectGroupConnectionList = { { "0", "1", "11" }, true };
	const ListEditorContainer g_ObjectSchemeConnectionList = { "Звезда", "Треугольник" };

	const ListEditorContainer g_LEDPeriodList = { {"Не мигает", 0 }, { "Мигает", 250 } };

	const ListEditorContainer g_RTU_M_alternateModeList = { "T2", "depRTU" };
	
	const ListEditorContainer g_CoreAmperageList = { "200А", "5А" };

	const ListEditorContainer g_KeyFuncList = {
		{ "Не используется", 0 },
		{ "Кнопка ВКЛ", 1 },
		{ "Кнопка ОТКЛ", 2 },
		{ "Экран токов", 3 },
		{ "Экран линейных напряжений", 4 },
		{ "Настройка защит по току", 5 },
		{ "Настройка защит по напряжению", 6 },
		{ "Режим «Местный / Дистанция»", 8 },
	};

	const ListEditorContainer g_SetpointsGroupsKeyFuncList = {
		{ "Выбор группы уставок 1", 9 },
		{ "Выбор группы уставок 2", 10 },
		{ "Выбор группы уставок 3", 11 },
		{ "Выбор группы уставок 4", 12 },
	};

	const ListEditorContainer g_DisplayTypeList = {
		{ "Не установлено", 0 },
		{ "DepRTU-EM", 1 },
		{ "DRZA", 2 },
		{ "DepRTU-LT", 3 },
		{ "ExPNL5", 10 },
		{ "ExSearch", 11 },
	};

	const ListEditorContainer g_ProfileList = { "1", "2", "3", "4" };

	const ListEditorContainer g_SignalTypesList = { 
		{ "Дискреты", DEF_SIG_TYPE_DISCRETE },
		{ "Аналоги", DEF_SIG_TYPE_ANALOG},
		{ "Счётчики", DEF_SIG_TYPE_COUNTER}
	};

	class SetpointsGroupsModel : public QAbstractTableModel
	{
	public:
		enum Columns {
			GroupColumn,
			SetColumn,
			GetColumn,

			ColumnsCount
		};

		struct Item {
			QString name;
			QVariant setSignal;
			QVariant getSignal;
		};

		SetpointsGroupsModel(DcController* contr, DefSignalType sigType, QObject* parent = nullptr) :
			QAbstractTableModel(parent),
			m_contr(contr) 
		{
			if (sigType == DEF_SIG_TYPE_DISCRETE) {
                auto param = contr->paramsRegistry().parameter(SP_APPL_MANAGE_WORDDIN_PARAM);
				if (!param)
					return;

                for (size_t i = 0; i < param->elementsCount(); i++) {
					Item it;
					it.name = QString("Группа уставок %1").arg(i + 1);

                    auto p = contr->paramsRegistry().element(SP_APPL_MANAGE_WORDDIN_PARAM, i);
					if (p)
						it.setSignal = p->value().toUInt();

                    p = contr->paramsRegistry().element(SP_APPL_MANAGE_WORDOUT_PARAM, i);
					if (p)
						it.getSignal = p->value().toUInt();

					m_items << it;
				}
			}

			if (sigType == DEF_SIG_TYPE_ANALOG) {
                auto param = contr->paramsRegistry().element(SP_APPL_MANAGE_WORDAOUT_PARAM, 0);
				if (!param)
					return;

				Item it{ "Общий" };
				it.getSignal = param->value().toUInt();

				m_items << it;
			}

			if (sigType == DEF_SIG_TYPE_COUNTER) {
                auto param = contr->paramsRegistry().element(SP_APPL_MANAGE_WORDCIN_PARAM, 0);
				if (!param)
					return;

				Item it{ "Общий" };
				it.getSignal = param->value().toUInt();

				m_items << it;
			}
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const 
		{
			if (parent.isValid())
				return 0;

			return m_items.size();
		}

		int columnCount(const QModelIndex& parent = QModelIndex()) const 
		{
			if (parent.isValid())
				return 0;

			return ColumnsCount;
		}

		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const 
		{
			if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
				return columnName((Columns)section);

			return QVariant();
		}

		Qt::ItemFlags flags(const QModelIndex& index) const 
		{
			Qt::ItemFlags fl = QAbstractTableModel::flags(index);
			if (index.column() < 1 || !index.data().isValid())
				return fl;

			return fl |= Qt::ItemIsEditable;
		}

		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const 
		{
			if (!index.isValid())
				return QVariant();

			auto d = m_items.at(index.row());
			int col = index.column();
			if (Qt::DisplayRole == role || Qt::EditRole == role) {
				if (GroupColumn == col)
					return d.name;
				if (SetColumn == col)
					return d.setSignal;
				if (GetColumn == col)
					return d.getSignal;
			}

			return QVariant();
		}

		bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) 
		{
			auto& d = m_items[index.row()];
			int col = index.column();
			if (SetColumn != col && GetColumn != col)
				return false;

			if (SetColumn == col) 
				d.setSignal = value;
			if (GetColumn == col)
				d.getSignal = value;

			emit dataChanged(index, index);
			return true;
		}

		QString columnName(Columns col) const 
		{
			switch (col)
			{
			case GroupColumn: return "Группа";
			case SetColumn: return "Установка";
			case GetColumn: return "Отображение";
			default: return QString();
			}
		}

		QList<Item> items() const { return m_items; }

	private:
		DcController* m_contr;
		QList<Item> m_items;
	};

	ComboBoxDelegate* g_DiscretsDelegate(DcController* contr, QObject* parent = nullptr)
	{
		ComboBoxDelegate* result = new ComboBoxDelegate(parent);
		result->append({ "Не используется" , std::numeric_limits<uint16_t>::max() });
		for (auto signal : contr->getSignalList(DEF_SIG_TYPE_DISCRETE)) {
			result->append({ signal->name(), static_cast<uint>(signal->internalId()) });
		}

		return result;
	}

	ComboBoxDelegate* g_virtualSignalsDelegate(DcController* contr, DefSignalType sigType, QObject* parent = nullptr)
	{
		ComboBoxDelegate* result = new ComboBoxDelegate(parent);
		result->append({ "Не используется" , std::numeric_limits<uint16_t>::max() });
		for (auto signal : contr->getSignalList(sigType, DEF_SIG_SUBTYPE_VIRTUAL)) {
			result->append({ signal->name(), static_cast<uint>(signal->internalId()) });
		}

		return result;
	}

    QString sign(ConnectionScheme s) {
        switch(s) {
        case SchemeStar:     return SCHEME_STAR_SIGN;
        case SchemeTriangle: return SCHEME_TRIANGLE_SIGN;
        default:
            break;
        }

        return QString();
    }

    ConnectionScheme scheme(const QString &sign) {
        if (sign == SCHEME_STAR_SIGN)
            return SchemeStar;
        if (sign == SCHEME_TRIANGLE_SIGN)
            return SchemeTriangle;

        return SchemeInvalid;
    }

    QString makeObjectConnectionSchemeGroup(WindingType winding, ConnectionScheme vn, ConnectionScheme nn,
                                             uint8_t groupNN, ConnectionScheme sn = SchemeInvalid, uint8_t groupSN = -1) {
        switch (winding) {
        case WindingType::Winding_2: return QString("%1/%2 -%3").arg(sign(vn)).arg(sign(nn)).arg(groupNN);
        case WindingType::Winding_3: return QString("%1/%2/%3 -%4 -%5").arg(sign(vn)).arg(sign(sn)).arg(sign(nn)).arg(groupSN).arg(groupNN);
        default: break;
        }

        return QString();
    }

    ListEditorContainer afeConnectionSchemeGroupList(WindingType winding) {
        QStringList result;
        switch (winding) {
        case WindingType::Winding_2:
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeStar, SchemeStar, 0);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeStar, SchemeStar, 6);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeStar, SchemeTriangle, 1);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeStar, SchemeTriangle, 5);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeStar, SchemeTriangle, 7);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeStar, SchemeTriangle, 11);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeStar, 1);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeStar, 5);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeStar, 7);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeStar, 11);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeTriangle, 0);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeTriangle, 2);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeTriangle, 4);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeTriangle, 6);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeTriangle, 8);
            result << makeObjectConnectionSchemeGroup(Winding_2, SchemeTriangle, SchemeTriangle, 10);
            break;
        case WindingType::Winding_3:
            result << makeObjectConnectionSchemeGroup(Winding_3, SchemeStar,     SchemeStar,     0,  SchemeStar,     0);
            result << makeObjectConnectionSchemeGroup(Winding_3, SchemeStar,     SchemeTriangle, 1,  SchemeStar,     0);
            result << makeObjectConnectionSchemeGroup(Winding_3, SchemeStar,     SchemeTriangle, 11, SchemeStar,     0);
            result << makeObjectConnectionSchemeGroup(Winding_3, SchemeStar,     SchemeTriangle, 1,  SchemeTriangle, 1);
            result << makeObjectConnectionSchemeGroup(Winding_3, SchemeStar,     SchemeTriangle, 11, SchemeTriangle, 11);
            result << makeObjectConnectionSchemeGroup(Winding_3, SchemeTriangle, SchemeTriangle, 0,  SchemeTriangle, 0);
            break;
        default:
            break;
        }

        return result;
    }
}

namespace Text
{
	namespace Common {
		const QString Title = "Общие";

		const QString PKE_Unom = "U ном. фазное, В";
		const QString OZZ_FLOAT_PARAM = "Таймер автосброса аварийных событий\nпри отсутсвии условий срабатывания\nи наличии фазных токов выше порога фиксации (мин)";
	}

	namespace Factors {
		const QString Title = "Коэффициенты";
	}

	namespace Connection {
		const QString Title = "Подключение";

		const QString AIN_CONNECT_TYPE0 = "Схема включения цепей напряжения";
		const QString AIN_CONNECT_TYPE1 = "Схема включения цепей тока";
		const QString AIN_CONNECT_TYPE2 = "Программная инверсия включения";
		const QString AIN_CONNECT_TYPE3 = "Чередование фаз";
	}

	namespace Display {
		const QString Title = "Дисплей";
		const QString DeviceNum = "Устройство отображения %1";

		const QString SYS_LEDS_BEHAVIOUR = "Мигание светодиода ГОТОВ";
		const QString DISPLAY_TYPE0 = "Модель";
		const QString DISPLAY_TYPE1 = "Опции подключения";
		const QString DISPLAY_FILENAME = "Полное имя файла";
		const QString DISPLAY_MAINSCR = "ID главного экрана";
	}

	namespace Keyboard {
		const QString Title = "Клавиатура"; 
		
		const QString DISPLAY_KEY_FUNC = "Функция кнопки F%1";
	}

	namespace Object {
		const QString Title = "Защищаемый объект";

        const QString DIFF_SCURE_BYTE_PARAM0 = "Тип защищаемого объекта";
		const QString DIFF_SCURE_BYTE_PARAM1 = "Схема соединений обмоток стороны ВН";
		const QString DIFF_SCURE_BYTE_PARAM2 = "Схема соединений обмоток стороны НН";
        const QString DIFF_SCURE_BYTE_PARAM3 = "Схема соединений обмоток стороны СН";
        const QString DIFF_SCURE_BYTE_PARAM4 = "Группа соединения обмоток НН";
        const QString DIFF_SCURE_BYTE_PARAM5 = "Группа соединения обмоток СН";
        const QString DIFF_SCURE_FLOAT_PARAM0 = "Номинальная мощность трансформатора, ВА";
		const QString DIFF_SCURE_FLOAT_PARAM1 = "Напряжение стороны ВН, В";
		const QString DIFF_SCURE_FLOAT_PARAM2 = "Напряжение стороны НН, В";
        const QString DIFF_SCURE_FLOAT_PARAM3 = "Напряжение стороны CН, В";
		const QString DIFF_SCURE_FLOAT_PARAM4 = "Номинальный ток шин, А";
	}

	namespace SetpointsGroups {
		const QString Title = "Группы уставок";

		const QString ACTIVE_GROUP = "Активная группа";

	}
}

DcSettingsForm::DcSettingsForm(DcController *controller) :
	DcForm(controller,  "Настройки", false)
{
	QVBoxLayout *formLayout = new QVBoxLayout(centralWidget());

	QTabWidget *tabWidget = new QTabWidget;
	formLayout->addWidget(tabWidget);

	tabWidget->addTab(createCommonTab(), Text::Common::Title);
	tabWidget->addTab(createFactorsTab(), Text::Factors::Title);
	tabWidget->addTab(createConnectionTab(), Text::Connection::Title);
	tabWidget->addTab(createDisplayTab(), Text::Display::Title);
	tabWidget->addTab(createKeyboardTab(), Text::Keyboard::Title);
	tabWidget->addTab(createObjectTab(), Text::Object::Title);
	tabWidget->addTab(createSetpointsGroupsTab(), Text::SetpointsGroups::Title);
}

bool DcSettingsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_PKE_Unom},
		{SP_AIN_PHYK1},
		{SP_AIN_CONNECT_TYPE},
		{SP_DISPLAY_TYPE},
		{SP_DISPLAY_KEY_FUNC},
		{SP_DIFF_SCURE_BYTE_PARAM},
		{SP_ALTERNATE_MODE}
	};

	return hasAny(controller, params);
}

void DcSettingsForm::fillReport(DcIConfigReport * report)
{
	auto device = report->device();
    auto element = device->paramsRegistry().element(SP_PKE_Unom, 0);
    if (element) {
		report->insertSection(Text::Common::Title);

		DcReportTable table(device);
        table.addRow(DcReportTable::ParamRecord( element, Text::Common::PKE_Unom ));
		table.addRow(DcReportTable::ParamRecord( SP_OZZ_FLOAT_PARAM, 6, Text::Common::OZZ_FLOAT_PARAM ));
		report->insertTable(table);
	}

    auto param = device->paramsRegistry().parameter(SP_AIN_PHYK1);
	if (param) {
		report->insertSection(Text::Factors::Title);

		DcReportTable table(device);
        for (size_t i = 0; i < param->elementsCount(); i++) {
			table.addRow(DcReportTable::ParamRecord( SP_AIN_PHYK1, i ));
		}
		table.addRow(DcReportTable::ParamRecord(SP_FIO_TypeFIO, 3, g_CoreAmperageList.toHash(), "Керн токов РЗА"));
		report->insertTable(table);
	}

    param = device->paramsRegistry().parameter(SP_AIN_CONNECT_TYPE);
	if (param) {
		report->insertSection(Text::Connection::Title);

		auto voltageList = device->type() == DcController::P_SCI ? g_VoltageList_PSCI : g_VoltageList;
		auto amperageList = device->type() == DcController::P_SCI ? g_AmperageList_PSCI : g_AmperageList;

		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord( SP_AIN_CONNECT_TYPE, 0, voltageList.toHash(), Text::Connection::AIN_CONNECT_TYPE0 ));
		table.addRow(DcReportTable::ParamRecord( SP_AIN_CONNECT_TYPE, 1, amperageList.toHash(), Text::Connection::AIN_CONNECT_TYPE1 ));
		table.addRow(DcReportTable::ParamRecord( SP_AIN_CONNECT_TYPE, 2, Text::Connection::AIN_CONNECT_TYPE2, true ));
		table.addRow(DcReportTable::ParamRecord( SP_AIN_CONNECT_TYPE, 3, g_PhaseList.toHash(), Text::Connection::AIN_CONNECT_TYPE3 ));
		report->insertTable(table);
	}

    param = device->paramsRegistry().parameter(SP_DISPLAY_TYPE);
	if (param) {
		report->insertSection(Text::Display::Title);

		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(SP_SYS_LEDS_BEHAVIOUR, 0, g_LEDPeriodList.toHash(), Text::Display::SYS_LEDS_BEHAVIOUR));
		report->insertTable(table);

		QStringList headers = {
			Text::ReportTable::Name,
			Text::Display::DISPLAY_TYPE0,
			Text::Display::DISPLAY_TYPE1,
			Text::Display::DISPLAY_FILENAME,
			Text::Display::DISPLAY_MAINSCR
		};

		DcReportTable displayTable(device, headers, { 30 });
        for (size_t i = 0; i < param->profilesCount(); i++) {
			auto idxBase = PROFILE_SIZE * i;
			QList<DcReportTable::ParamRecord> recs;
			recs.append(DcReportTable::ParamRecord(SP_DISPLAY_TYPE, idxBase + 0, g_DisplayTypeList.toHash()));
			recs.append(DcReportTable::ParamRecord(SP_DISPLAY_TYPE, idxBase + 1));
			recs.append(DcReportTable::ParamRecord(SP_DISPLAY_FILENAME, i));
			recs.append(DcReportTable::ParamRecord(SP_DISPLAY_MAINSCR, i));
			displayTable.addRow(QString(Text::Display::DeviceNum).arg(i + 1), recs);
		}
		report->insertTable(displayTable);
	}

    param = device->paramsRegistry().parameter(SP_DISPLAY_KEY_FUNC);
	if (param) {
		report->insertSection(Text::Keyboard::Title);

		auto funcList = g_KeyFuncList;
        if (auto activeGroupParam = device->paramsRegistry().element(SP_ACTIVPROFILE, 0); activeGroupParam)
			funcList.unite(g_SetpointsGroupsKeyFuncList);

		DcReportTable table(device);
        for (size_t i = 0; i < param->elementsCount(); i++) {
			table.addRow(DcReportTable::ParamRecord(SP_DISPLAY_KEY_FUNC, i, funcList.toHash(), Text::Keyboard::DISPLAY_KEY_FUNC.arg(i + 1)));
		}
		report->insertTable(table);
	}

    param = device->paramsRegistry().parameter(SP_DIFF_SCURE_BYTE_PARAM);
	if (param) {
		report->insertSection(Text::Object::Title);

		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_BYTE_PARAM, 0, g_ObjectTypeList.toHash(), Text::Object::DIFF_SCURE_BYTE_PARAM0));
		table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_FLOAT_PARAM, 0, Text::Object::DIFF_SCURE_FLOAT_PARAM0));
        table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_BYTE_PARAM, 4, g_AFE7_ObjectGroupConnectionList.toHash(), Text::Object::DIFF_SCURE_BYTE_PARAM4));
		table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_BYTE_PARAM, 1, g_ObjectSchemeConnectionList.toHash(), Text::Object::DIFF_SCURE_BYTE_PARAM1));
		table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_FLOAT_PARAM, 1, Text::Object::DIFF_SCURE_FLOAT_PARAM1));
		table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_BYTE_PARAM, 2, g_ObjectSchemeConnectionList.toHash(), Text::Object::DIFF_SCURE_BYTE_PARAM2));
		table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_FLOAT_PARAM, 2, Text::Object::DIFF_SCURE_FLOAT_PARAM2));
		table.addRow(DcReportTable::ParamRecord(SP_DIFF_SCURE_FLOAT_PARAM, 4, Text::Object::DIFF_SCURE_FLOAT_PARAM4));
		report->insertTable(table);
	}
}

QWidget * DcSettingsForm::createCommonTab()
{
	static QList<Param> params = {
		{SP_PKE_Unom},
		{SP_ALTERNATE_MODE}
	};

	if (!hasAny(controller(), params))
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addLineEditor(SP_PKE_Unom, 0, Text::Common::PKE_Unom, 5, 2000);
	QString description = "При значении '0', значение таймера равно 10 секунд";
	manager.addLineEditor(SP_OZZ_FLOAT_PARAM, 6, Text::Common::OZZ_FLOAT_PARAM, 0, 1440, 2, description);
	
	
	//manager.addListEditor(SP_ALTERNATE_MODE, 0, "Режим работы", g_RTU_M_alternateModeList);
	manager.addListEditor(SP_ALTERNATE_MODE, 0, "Режим работы", g_RTU_M_alternateModeList, false);

	manager.addStretch();
	return tab;
}

QWidget * DcSettingsForm::createFactorsTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_AIN_PHYK1);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

    for (size_t i = 0; i < param->elementsCount(); i++) {
        auto p = controller()->paramsRegistry().element(SP_AIN_PHYK1, i);
		manager.addLineEditor(p, p->name(), 0, 2000);
	}

	//
	manager.addListEditor(SP_FIO_TypeFIO, 3, "Керн токов РЗА", g_CoreAmperageList);


	manager.addStretch();
	return tab;
}

QWidget * DcSettingsForm::createConnectionTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_AIN_CONNECT_TYPE);
	if (!param)
        return nullptr;

    for(auto &board: controller()->boards()) {
        auto boardType = board->type();
        if ("DRZA_AFE7" == boardType || "DRZA_AFE11" == boardType)
            return nullptr;
    }

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	auto voltageList = controller()->type() == DcController::P_SCI ? g_VoltageList_PSCI : g_VoltageList;
	auto amperageList = controller()->type() == DcController::P_SCI ? g_AmperageList_PSCI : g_AmperageList;

	manager.addListEditor(SP_AIN_CONNECT_TYPE, 0, Text::Connection::AIN_CONNECT_TYPE0, voltageList);
	manager.addListEditor(SP_AIN_CONNECT_TYPE, 1, Text::Connection::AIN_CONNECT_TYPE1, amperageList);
	manager.addCheckEditor(SP_AIN_CONNECT_TYPE, 2, Text::Connection::AIN_CONNECT_TYPE2);
	manager.addListEditor(SP_AIN_CONNECT_TYPE, 3, Text::Connection::AIN_CONNECT_TYPE3, g_PhaseList);

	manager.addStretch();
	return tab;
}

QWidget * DcSettingsForm::createDisplayTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_DISPLAY_TYPE);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);
	manager.addListEditor(SP_SYS_LEDS_BEHAVIOUR, 0, Text::Display::SYS_LEDS_BEHAVIOUR, g_LEDPeriodList);

    for (size_t i = 0; i < param->profilesCount(); i++) {
		int idxBase = PROFILE_SIZE * i;

		auto groupManager = manager.addGroupBox(QString(Text::Display::DeviceNum).arg(i + 1), 0, 1, 3);		
		groupManager->addListEditor(SP_DISPLAY_TYPE, idxBase + 0, Text::Display::DISPLAY_TYPE0, g_DisplayTypeList);
		groupManager->addLineEditor(SP_DISPLAY_TYPE, idxBase + 1, Text::Display::DISPLAY_TYPE1);
		groupManager->addLineEditor(SP_DISPLAY_FILENAME, i, Text::Display::DISPLAY_FILENAME);
		groupManager->addLineEditor(SP_DISPLAY_MAINSCR, i, Text::Display::DISPLAY_MAINSCR, 1);
	}

	manager.addStretch();
	return tab;
}

QWidget * DcSettingsForm::createKeyboardTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_DISPLAY_KEY_FUNC);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);	

	auto funcList = g_KeyFuncList;
    if (auto activeGroupParam = controller()->paramsRegistry().element(SP_ACTIVPROFILE, 0); activeGroupParam)
		funcList.unite(g_SetpointsGroupsKeyFuncList);

	manager.addListEditor(SP_DISPLAY_KEY_FUNC, 0, Text::Keyboard::DISPLAY_KEY_FUNC.arg(1), funcList);
	manager.addListEditor(SP_DISPLAY_KEY_FUNC, 1, Text::Keyboard::DISPLAY_KEY_FUNC.arg(2), funcList);

    bool isC1 = controller()->isC1Panel();
	auto editor = manager.addListEditor(SP_DISPLAY_KEY_FUNC, 2, Text::Keyboard::DISPLAY_KEY_FUNC.arg(3), funcList);
	if (editor)
		editor->setReadOnly(isC1);

	editor = manager.addListEditor(SP_DISPLAY_KEY_FUNC, 3, Text::Keyboard::DISPLAY_KEY_FUNC.arg(4), funcList);
	if (editor)
		editor->setReadOnly(isC1);

	manager.addStretch();
	return tab;
}

QWidget * DcSettingsForm::createObjectTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_DIFF_SCURE_BYTE_PARAM);
	if (!param)
		return nullptr;

    WindingType winding = Winding_2;
    for(auto &board: controller()->boards()) {
        auto boardType = board->type();
        if ("DRZA_AFE" == boardType)
            return nullptr;
        if ("DRZA_AFE11" == boardType)
            winding = Winding_3;
    }

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addListEditor(SP_DIFF_SCURE_BYTE_PARAM, 0, Text::Object::DIFF_SCURE_BYTE_PARAM0, g_ObjectTypeList);
    manager.addLineEditor(SP_DIFF_SCURE_FLOAT_PARAM, 0, Text::Object::DIFF_SCURE_FLOAT_PARAM0, 0);
    manager.addLineEditor(SP_DIFF_SCURE_FLOAT_PARAM, 1, Text::Object::DIFF_SCURE_FLOAT_PARAM1, 0);
    if (Winding_3 == winding) {
        manager.addLineEditor(SP_DIFF_SCURE_FLOAT_PARAM, 3, Text::Object::DIFF_SCURE_FLOAT_PARAM3, 0);
    }
    manager.addLineEditor(SP_DIFF_SCURE_FLOAT_PARAM, 2, Text::Object::DIFF_SCURE_FLOAT_PARAM2, 0);

    auto vnSchemeParam = controller()->paramsRegistry().element(SP_DIFF_SCURE_BYTE_PARAM, 1);
    auto nnSchemeParam = controller()->paramsRegistry().element(SP_DIFF_SCURE_BYTE_PARAM, 2);
    auto snSchemeParam = controller()->paramsRegistry().element(SP_DIFF_SCURE_BYTE_PARAM, 3);
    auto nnGroupParam = controller()->paramsRegistry().element(SP_DIFF_SCURE_BYTE_PARAM, 4);
    auto snGroupParam = controller()->paramsRegistry().element(SP_DIFF_SCURE_BYTE_PARAM, 5);
    auto currentSchemeGroupText = makeObjectConnectionSchemeGroup(winding,
                                                                  static_cast<ConnectionScheme>(vnSchemeParam->value().toInt()),
                                                                  static_cast<ConnectionScheme>(nnSchemeParam->value().toInt()),
                                                                  static_cast<ConnectionScheme>(nnGroupParam->value().toInt()),
                                                                  static_cast<ConnectionScheme>(snSchemeParam->value().toInt()),
                                                                  static_cast<ConnectionScheme>(snGroupParam->value().toInt()));

    auto schemeGroupEditor = manager.addListEditor("Схема и группа соединений", afeConnectionSchemeGroupList(winding));
    schemeGroupEditor->setCurrentByText(currentSchemeGroupText);
    connect(schemeGroupEditor, &ListParamEditor::valueChanged, this, [=]() {
        auto splitedTextList = schemeGroupEditor->currentText().split(' ');

        auto schemeList = splitedTextList.value(0).split('/');
        auto scheme1 = scheme(schemeList.value(0));
        auto scheme2 = scheme(schemeList.value(1));
        auto scheme3 = scheme(schemeList.value(2));
        auto group1 = splitedTextList.value(1).remove(0, 1);
        auto group2 = splitedTextList.value(2).remove(0, 1);

        vnSchemeParam->updateValue( QString::number(scheme1) );
        switch (winding) {
        case Winding_2:
            nnSchemeParam->updateValue( QString::number(scheme2) );
            nnGroupParam->updateValue( group1 );
            break;
        case Winding_3:
            snSchemeParam->updateValue( QString::number(scheme2) );
            snGroupParam->updateValue( group1 );
            nnSchemeParam->updateValue( QString::number(scheme3) );
            nnGroupParam->updateValue( group2 );
            break;
        default:
            break;
        };
    });

    manager.addLineEditor(SP_DIFF_SCURE_FLOAT_PARAM, 4, Text::Object::DIFF_SCURE_FLOAT_PARAM4, 0);
	manager.addStretch();
	return tab;
}

QWidget* DcSettingsForm::createSetpointsGroupsTab()
{
    auto param = controller()->paramsRegistry().element(SP_ACTIVPROFILE, 0);
	if (!param)
		return nullptr;	

	QWidget* tab = new QWidget;
	QGridLayout* tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	auto discretsDelegate = g_DiscretsDelegate(controller(), this);
	auto virtualDiscretsDelegate = g_virtualSignalsDelegate(controller(), DEF_SIG_TYPE_DISCRETE, this);
	auto virtualAnalogsDelegate = g_virtualSignalsDelegate(controller(), DEF_SIG_TYPE_ANALOG, this);
	auto virtualCountersDelegate = g_virtualSignalsDelegate(controller(), DEF_SIG_TYPE_COUNTER, this);

	auto createTableView = [=](const QVariant& data) {
		auto signalType = static_cast<DefSignalType>(data.toUInt());
		auto model = new SetpointsGroupsModel(controller(), signalType, tab);

		auto view = new TableView(tab);
		view->setModel(model);
		view->horizontalHeader()->resizeSection(SetpointsGroupsModel::GroupColumn, 150);
		view->horizontalHeader()->resizeSection(SetpointsGroupsModel::SetColumn, 300);
		view->horizontalHeader()->resizeSection(SetpointsGroupsModel::GetColumn, 300);

		switch (signalType)
		{
		case DEF_SIG_TYPE_DISCRETE:
			view->setItemDelegateForColumn(SetpointsGroupsModel::SetColumn, discretsDelegate);
			view->setItemDelegateForColumn(SetpointsGroupsModel::GetColumn, virtualDiscretsDelegate);
			break;
		case DEF_SIG_TYPE_ANALOG:
			view->setItemDelegateForColumn(SetpointsGroupsModel::GetColumn, virtualAnalogsDelegate);
			break;
		case DEF_SIG_TYPE_COUNTER:
			view->setItemDelegateForColumn(SetpointsGroupsModel::GetColumn, virtualCountersDelegate);
			break;
		default:
			break;
		}
		
		return view;
	};

	manager.addListEditor(param, Text::SetpointsGroups::ACTIVE_GROUP, g_ProfileList);

	auto groupBox = new QGroupBox("Установка/отображение через сигналы", tab);
	manager.addWidget(groupBox, 0, 1, 3);
	manager.layout()->setColumnStretch(2, 1);

	auto groupBoxLayout = new QGridLayout(groupBox);
	EditorsManager groupBoxManager(controller(), groupBoxLayout);
	auto editor = groupBoxManager.addListEditor("Тип сигнала", g_SignalTypesList);

	auto viewLayout = new QGridLayout;
	groupBoxManager.addWidgetCreatorLayout(editor, createTableView, viewLayout, 0, 1, 3);
	groupBoxLayout->setColumnStretch(2, 1);

	//manager.addVerticalStretch();
	return tab;
}
