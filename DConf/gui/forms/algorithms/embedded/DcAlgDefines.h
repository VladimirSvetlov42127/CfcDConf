#pragma once

#include <gui/editors/EditorsManager.h>

#define ALG_ADDRS_FUNC(...) \
Algs::Addrs alg_addrs(int n = 0) { static QList<Algs::Addrs> p = {__VA_ARGS__}; return p[n]; }

#define ALG_ADDRS_FUNC_HASH(...) \
Algs::Addrs alg_addrs(int algIdx) { static QHash<int, Algs::Addrs> p = {__VA_ARGS__}; return p[algIdx]; } 

namespace Algs {
	namespace Text {
		const QString SettingsGroupNo = "Группа уставок %1";

		const QString ActionOnSwitcher = "Действие на выключатель";
		const QString ActivateUrovTimer = "Активирует таймер УРОВ";
		const QString APV = "АПВ";
		const QString LZSH = "Формирует ЛЗШ";		
		const QString ActuationTime = "Время срабатывания (c)";
        const QString ActuationAmperage = "Ток срабатывания";
		const QString ActuationVoltage = "Напряжение срабатывания (В)";
		const QString ActuationType = "Тип порога срабатывания";
		const QString ActuationAngle = "Угол срабатывания";
		const QString ReturnRate = "Коэф. возврата";
		const QString DirectionalMode = "Направленный режим";
		const QString AngleMaxSensitivity = "Угол макс. чувствительности";
		const QString BlockVoltage = "Напряжение блокировки (В)";

		const QString DependencyAmperage = "Зависимость время-ток";
		const QString DependencyVoltage = "Зависимость время-напряжение";

		const QString AvtoKvitType = "Тип автоматической квитации";
	}

	namespace Colors {
		const QColor CurrentProfie = QColor("#97cfc1");
	}

	const ListEditorContainer ApvTypeList = { "Нет", "Однократное", "Двукратное" };
	const ListEditorContainer DependencyTypeList = { "Независимая", "Сильно инверсная", "Чрезвычайно инверсная", "Длительно инверсная", "Типа RI" };
	const ListEditorContainer VoltageTypeList = { "Фазное", "Линейное" };
	const ListEditorContainer ThresholdTypeList = { "Абсолютная", "Относительная" };	
	const ListEditorContainer AutoKvitTypeList = { "Выведена", "По востановлению нормального режима", "По снижению 3Uo", "Время суток", "Периодическая" };

	struct Addrs
	{
		Addrs(uint16_t base = 0) :
			inputs(base),
			outputs(base + 1),
			bytes(base + 2),
			floats(base + 3),
			words(base + 4)
		{}

		uint16_t inputs;
		uint16_t outputs;
		uint16_t bytes;
		uint16_t floats;
		uint16_t words;
	};
}
