#pragma once

#include <functional>
#include <map>
#include <qicon.h>

#include <report/DcIConfigReport.h>

#define ADD_MENU(MENU_NAME, ICON_PATH, TITLE, PARENT) \
static DcMenu& MENU_NAME() { static DcMenu m(ICON_PATH, TITLE, &PARENT(), __LINE__); return m; }

class DcController;

class DcMenu
{
	DcMenu(const QString &iconPath, const QString &title = QString(), DcMenu *parent = nullptr, int pos = 0);

public:
	using CheckFuncType = std::function<bool(DcController*)>;
	using FillReportFuncType = std::function<void(DcIConfigReport*)>;

public:
	DcMenu(const DcMenu &) = delete;
	DcMenu(DcMenu &&) = delete;
	DcMenu& operator=(const DcMenu&) = delete;
	DcMenu& operator=(DcMenu &&) = delete;	

	bool isAvailableFor(DcController *controller) const;
	void fillReport(DcIConfigReport *report) const;
	std::map<int, DcMenu*> childs() const { return m_childs; }
	QString title() const { return m_title; }
	QIcon icon() const { return QIcon(m_iconPath); }
	QString formIdx() const { return m_formIdx; }

	void setCheckFunc(CheckFuncType func) { m_checkFunc = func; }
	void setFormIdx(const QString &formIdx) { m_formIdx = formIdx; }
	void setFillReportFunc(FillReportFuncType func) { m_fillReportFunc = func; }

    static DcMenu& root() { static DcMenu m(":/icons/list.svg"); return m; }

	ADD_MENU(general_settings, ":/icons/setting_3.svg", "Общие настройки", root);

	ADD_MENU(input_output, ":/icons/exchange.svg", "Входы/выходы", root);
        ADD_MENU(discret_inputs, ":/icons/signal_d_in.svg", "Дискретные входы", input_output);
			ADD_MENU(discret_input_channels, ":/icons/chanel_settings.svg", "Поканальные настройки", discret_inputs);
				ADD_MENU(dins_physical, ":/icons/signal_in.svg", "Физические входы", discret_input_channels);
				ADD_MENU(dins_logical, ":/icons/signal_in.svg", "Логические входы", discret_input_channels);
				ADD_MENU(dins_virtual, ":/icons/signal_in.svg", "Виртуальные входы", discret_input_channels);
				ADD_MENU(dins_external, ":/icons/signal_in.svg", "Внешние входы", discret_input_channels);
			ADD_MENU(discret_2pos_channels, ":/icons/double_point.svg", "Двухпозиционные сигналы", discret_inputs);
			ADD_MENU(discret_input_adc, ":/icons/acp.svg", "Входы АЦП", discret_inputs);
        ADD_MENU(discret_outputs, ":/icons/signal_d_out.svg", "Дискретные выходы", input_output);
			ADD_MENU(discret_output_channels, ":/icons/chanel_settings.svg", "Поканальные настройки", discret_outputs);			
			ADD_MENU(block_control, ":/icons/block.svg", "Блокировка управления", discret_outputs);
			ADD_MENU(output_clones, ":/icons/double_point.svg", "Дублирование физических выходов", discret_outputs);
        ADD_MENU(analog_inputs, ":/icons/signal_a_in.svg", "Аналоговые входы", input_output);
            ADD_MENU(analogs_aperture, ":/icons/limits.svg", "Апертуры и пороги", analog_inputs);
        ADD_MENU(counter_inputs, ":/icons/signal_c_in.svg", "Счетчики", input_output);

    ADD_MENU(thresholds_settings, ":/icons/limits2.svg", "Уставки/пороги", root);
        ADD_MENU(thresholds, ":/icons/limits.svg", "Пороги", thresholds_settings);
		ADD_MENU(pqi_settings, ":/icons/pke.svg", "Уставки ПКЭ", thresholds_settings);
		ADD_MENU(pqi_event_settings, ":/icons/pke_event.svg", "Уставки событий ПКЭ", thresholds_settings);

	ADD_MENU(interface_protocols, ":/icons/stack.svg", "Интерфейсы/протоколы", root);
		ADD_MENU(sim, ":/icons/sim.svg", "SIM", interface_protocols);
        ADD_MENU(time_sync, ":/icons/clock1.svg", "Синхронизация времени", interface_protocols);
		ADD_MENU(rs485, ":/icons/rs485.svg", "RS-485", interface_protocols);
		ADD_MENU(tcp, ":/icons/ethernet_fr.svg", "TCP", interface_protocols);
		ADD_MENU(iec101_104, ":/icons/iec_104_fr.svg", "МЭК 60870-5-101/104", interface_protocols);
            ADD_MENU(iec101_104_settings, ":/icons/setting.svg", "Параметры общие", iec101_104);
			ADD_MENU(iec101_104_channels, ":/icons/chanel_settings.svg", "Поканальные", iec101_104);
		ADD_MENU(iec103, ":/icons/iec_103_fr.svg", "МЭК 60870-5-103", interface_protocols);
            ADD_MENU(iec103_settings, ":/icons/setting.svg", "Параметры общие", iec103);
			ADD_MENU(iec103_channels, ":/icons/chanel_settings.svg", "Поканальные", iec103);
		ADD_MENU(iec61850, ":/icons/iec61850_fr.svg", "МЭК 61850", interface_protocols);
		ADD_MENU(spodes, ":/icons/spodes.svg", "CПОДЭС", interface_protocols);
            ADD_MENU(spodes_settings, ":/icons/setting.svg", "Параметры общие", spodes);
			ADD_MENU(spodes_channels, ":/icons/chanel_settings.svg", "Поканальные", spodes);		

	ADD_MENU(functions, ":/icons/function.svg", "Функции", root);
		ADD_MENU(analogs_functions, ":/icons/transformation.svg", "Аналоги", functions);
		ADD_MENU(counters_functions, ":/icons/transformation.svg", "Счётчики", functions);
		ADD_MENU(virtual_functions, ":/icons/virtual_function.svg", "Функции виртуальных выходов", functions);
		ADD_MENU(switches, ":/icons/switches.svg", "Выключатели", functions);
		ADD_MENU(oscillography, ":/icons/oscillogram.svg", "Осциллографирование", functions);
		ADD_MENU(auv_urov, ":/icons/document.svg", "АУВ/УРОВ", functions);

	ADD_MENU(algorithms, ":/icons/algorithm.svg", "Алгоритмы", root);
	ADD_MENU(algorithms_embedded, ":/icons/algorithm_built_in.svg", "Встроенные", algorithms);
	ADD_MENU(algorithms_custom, ":/icons/algorithm_custom.svg", "Пользовательские", algorithms);

	ADD_MENU(converter, ":/icons/convert.svg", "Конвертер", root);

	ADD_MENU(matrix, ":/icons/bind_table.svg", "Таблица привязки", root);

private:
	std::map<int, DcMenu*> m_childs;

	QString m_iconPath;
	QString m_title;	
	
	CheckFuncType m_checkFunc;
	QString m_formIdx;
	FillReportFuncType m_fillReportFunc;
};
