#include "editor_node_data.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
EditorNodeData::EditorNodeData(QString ID)
{
	if (!ID.isEmpty()) {
		int id = ID.toUInt();
		if (id >= _max) _max = id;
		_uuid = ID;
	} else _uuid = QString::number(++_max);

	_node_type = FlexLogic::RZA_DEFAULT;
	_has_output = false;
	_inversion = false;
	_min_inputs = 0;
	_max_inputs = 0;
}

EditorNodeData::~EditorNodeData()
{
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void EditorNodeData::addParam(QString name, QString id, QVariant value, QString short_info)
{
	FlexLogic::NodeParam element({ name, id, value, short_info });
	_params_list.append(element);

	return;
}

void EditorNodeData::addParam(QString name, QString id, QVariant value)
{
	QString info = name + " = ";
	FlexLogic::NodeParam elem({ name, id, value, info });
	_params_list.append(elem);

	return;
}

FlexLogic::NodeParam EditorNodeData::param(QString id)
{
    for (auto& it : ParamsList()) if (it.index == id) return it;
	return FlexLogic::NodeParam();
}

void EditorNodeData::setParam(QString id, QVariant value)
{
	if (_params_list.isEmpty()) return;
	int size = _params_list.size();
	for (int i = 0; i < size; i++) {
		if (_params_list.at(i).index != id) continue;
		FlexLogic::NodeParam parameter = _params_list.at(i);
		parameter.value = value;
		_params_list.replace(i, parameter);
		return;	}

	return;
}

void EditorNodeData::setParam(int index, QVariant value)
{
	if (_params_list.isEmpty()) return;
	_params_list[index].value = value;

	return;
}
