#include "cfc_compiler.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================
#include <algorithm>

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QDebug>
#include <QFile>
#include <QTextStream>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "gui/forms/algorithms/custom/cfc_editor/cfc_socket.h"


//===================================================================================================================================================
//	Списки данных проекта
//===================================================================================================================================================
namespace {
	const uint8_t FILE_SIZE_POSITION = 4;
	const uint8_t BYTE_CODE_POSITION = 14;
	const uint8_t VARIABLE_BASE = 20;
	const uint8_t ELEMENT_BASE = 48;
	const uint8_t CONNECTION_BASE = 16;
	const uint8_t TITLE_SIZE = 128;
	const uint32_t HEADER_CRC = 0x02;
	const uint32_t RUNTIME_US = 40000;
	const uint32_t MAX_RUNTIME_US = 100000;
	const uint8_t BOOL_TAG = 0x83;
	const uint8_t INTEGER_TAG = 0x85;
	const uint8_t HEADER_TAG = 0x51;
	const uint8_t SCHEME_TAG = 0x54;
	const uint8_t VAR_ARRAY_TAG = 0x55;
	const uint8_t NODE_ARRAY_SIZE_TAG = 0x61;
	const uint8_t NODE_TAG = 0x63;
	const uint8_t NODE_TYPE_TAG = 0x64;
	const uint8_t NODE_PARAM_TAG = 0x65;
	const uint8_t NODE_INPUT_TAG = 0x66;
	const uint8_t NODE_OUTPUT_TAG = 0x67;
}

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
CfcCompiler::CfcCompiler(QObject* parent) : QObject(parent)
{
	//	Свойства класса
	_title = QString("first");
	_byte_code = new ByteCode();
}

CfcCompiler::~CfcCompiler()
{
	delete _byte_code;
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
bool CfcCompiler::compile(CfcParser* parser)
{
    connect(parser, &CfcParser::errorToLog, this, &CfcCompiler::onParserError);
    connect(parser, &CfcParser::infoToLog, this, &CfcCompiler::onParserInfo);

    if (!parser->validate())
        return false;



	//	Генерация списка элементов
    QList<CompileElement> nodes_list = getNodes(parser->nodes());
	if (nodes_list.isEmpty()) {
		emit errorToLog("Ошибка получения списка элементов алгоритма гибкой логики.");
		return false; }

	//	Генерация связей между элементами
    if (!setLinks(parser->links(), nodes_list)) {
		emit errorToLog("Ошибка генерации связей между элементами.");
		return false; }

	//	Компиляция файла ByteCode
	if (!createByteCode(nodes_list)) {
		emit errorToLog("Ошибка компиляции файла алгоритмов.");
		return false; }
	emit infoToLog("Компиляция алгоритма прошла успешно.");

    return true;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
QList<CompileElement> CfcCompiler::getNodes(QList<CfcNode*> nodes)
{
    QList<CompileElement> list;
	bool input_exist = false;
	bool output_exist = false;
    CompileElement input_node, output_node;

	//	Создание входного элемента
    for (int i = 0; i < nodes.count(); i++) {
        if (nodes.at(i)->name() != "BI")
            continue;
		//	Создание элемента
		if (!input_exist) {
            input_node.type = RZA_READ;
            for (int ii = 0; ii < 3; ii++)
                input_node.params.append(0);
            input_exist = true;
        }

		//	Добавление выходов
        for (int ii = 0; ii < nodes.at(i)->sockets().count(); ii++) {
            CompileLink link;
            link.socket_id = nodes.at(i)->sockets().at(ii)->id();
            for (int k = 0; k < nodes.at(i)->sockets().at(ii)->links().count(); k++)
                link.links_list.append(nodes.at(i)->sockets().at(ii)->links().at(k)->id());

            link.type = VARIABLE_DISCRETE;
			link.inversion = false;
            link.pin_number = nodes.at(i)->param("alg_pin").value.toInt();
			link.link_number = -1;
            input_node.outputs.append(link);
        }
	}
	list.append(input_node);

	//	Заполнение списка стандартных элементов
	for (int i = 0; i < nodes.count(); i++) {
        CfcNode* node = nodes.at(i);
        if (node->nodeType() == RZA_LOAD)
            continue;
        CompileElement compile_node;
        compile_node.type = node->nodeType();
        for (int ii = 0; ii < 3; ii++)
            compile_node.params.append(0);
        for (int ii = 0; ii < node->paramsList().count(); ii++)
            compile_node.params[ii] = node->paramsList().at(ii).value.toInt();

        for (int ii = 0; ii < node->sockets().count(); ii++) {
            CompileLink link;

            link.type = VARIABLE_DISCRETE;
            link.socket_id = node->sockets().at(ii)->id();
            for (int k = 0; k < nodes.at(i)->sockets().at(ii)->links().count(); k++)
                link.links_list.append(nodes.at(i)->sockets().at(ii)->links().at(k)->id());

            link.inversion = false;
			link.pin_number = ii + 1;
			link.link_number = -1;

            if (node->sockets().at(ii)->socketType() == CfcSocket::OUTPUT_SOCKET)
                compile_node.outputs.append(link);
            if (node->sockets().at(ii)->socketType() == CfcSocket::INPUT_SOCKET)
                compile_node.inputs.append(link);
        }
        list.append(compile_node);
    }

	//	Создание выходного элемента
	for (int i = 0; i < nodes.count(); i++) {
        if (nodes.at(i)->name() != "BO") continue;
		//	Создание элемента
		if (!output_exist) {
            output_node.type = RZA_WRITE;
            for (int ii = 0; ii < 3; ii++)
                output_node.params.append(0);
            output_exist = true;
        }

		//	Добавление входов
        for (int ii = 0; ii < nodes.at(i)->sockets().count(); ii++) {
            CompileLink link;
            link.socket_id = nodes.at(i)->sockets().at(ii)->id();
            for (int k = 0; k < nodes.at(i)->sockets().at(ii)->links().count(); k++)
                link.links_list.append(nodes.at(i)->sockets().at(ii)->links().at(k)->id());

            link.type = VARIABLE_DISCRETE;
			link.inversion = false;
            link.pin_number = nodes.at(i)->param("alg_pin").value.toInt();
			link.link_number = -1;
            output_node.inputs.append(link);
        }
	}
    list.append(output_node);

	//	Сортировка входов и выходов элементов по номерам пинов
	for (int i = 0; i < list.count(); i++) {
		std::sort(list[i].inputs.begin(), list[i].inputs.end(),
            [](const CompileLink& a, const CompileLink& b) {
				return a.pin_number < b.pin_number;	});
		std::sort(list[i].outputs.begin(), list[i].outputs.end(),
            [](const CompileLink& a, const CompileLink& b) {
				return a.pin_number < b.pin_number;	});
	}

    return list;
}

bool CfcCompiler::setLinks(QList<CfcLink*> links, QList<CompileElement>& nodes_list)
{
	//	Инициализация переменных
	int default_num = -1;
	int set_num = 0;
	
	//	Цикл по элементам
	for (int i = 0; i < nodes_list.count(); i++) {
		for (int ii = 0; ii < nodes_list.at(i).outputs.count(); ii++) {
			if (nodes_list.at(i).outputs.at(ii).link_number < 0) {
				default_num++;
				set_num = default_num;
                nodes_list[i].outputs[ii].link_number = set_num;
            } else set_num = nodes_list.at(i).outputs.at(ii).link_number;

            //	Цикл по линкам
			for (int lnk = 0; lnk < nodes_list.at(i).outputs.at(ii).links_list.count(); lnk++) {
				QString link_id = nodes_list.at(i).outputs.at(ii).links_list.at(lnk);
				QString target_id = QString();

				//	Получение id сокета цели
				for (int nn = 0; nn < links.count(); nn++) {
                    if (links.at(nn)->id() != link_id)
                        continue;
                    target_id = links.at(nn)->target()->id();
                    break;
                }
				if (target_id == QString()) { 
					emit errorToLog("Ошибка получения id сокета цели.");
                    return false;
                }

				//	Поиск элемента с заданным сокетом
				int to_node = -1, to_pin = -1;
				for (int kk = 0; kk < nodes_list.count(); kk++) {
					for (int kkk = 0; kkk < nodes_list.at(kk).inputs.count(); kkk++) {
                        if (nodes_list.at(kk).inputs.at(kkk).socket_id != target_id)
                            continue;
						to_node = kk;
						to_pin = kkk;
                        break;
                    }
				}
                if (to_node < 0 || to_pin < 0) {
                    emit errorToLog("Не найден узел или сокет цели.");
                    return false;
                }
				nodes_list[to_node].inputs[to_pin].link_number = set_num; }
		}
	}

    return true;
}


//===================================================================================================================================================
//	Методы работы с байт-кодом
//===================================================================================================================================================
bool CfcCompiler::createByteCode(QList<CompileElement>& nodes_list)
{
	//	Получение необходимых размеров
	uint16_t max_vars_size = maxVarsSize(nodes_list);
	uint32_t total_connections = totalConnections(nodes_list);
	uint32_t ram_size = 2 * max_vars_size * VARIABLE_BASE + nodes_list.count() * ELEMENT_BASE + total_connections * CONNECTION_BASE + 
		4 * (sizeof(float) + sizeof(float) + 2 * sizeof(uint32_t));

	//	Создание заголовка схемы
	createHeader(ram_size, _title);
    int header_size = byteCode()->size();

	//  Добавление кода схемы
	byteCode()->append(SCHEME_TAG);

	//	Добавление размера схемы
	uint16_t scheme_length = 0x00;
    uint16_t scheme_length_position = _byte_code->size();
	byteCode()->appendComplexLength(scheme_length);

	//  Добавление заголовка массива элементов
	byteCode()->append(VAR_ARRAY_TAG);
	byteCode()->appendComplexLength(uint16_t(2));
	byteCode()->appendData(max_vars_size);
	byteCode()->append(NODE_ARRAY_SIZE_TAG);
	byteCode()->appendComplexLength(uint16_t(1));
	byteCode()->append(uint8_t(nodes_list.count()));

	//	Добавление элементов схемы
    for (int i = 0; i < nodes_list.count(); i++)
        createNode(nodes_list.at(i));

	//	Обновление размера файла
    byteCode()->setData(FILE_SIZE_POSITION, uint32_t(byteCode()->size() + 4));

	//	Обновление размера схемы
    scheme_length = byteCode()->size() - scheme_length_position - 3;
	byteCode()->setComplexLength(scheme_length_position, scheme_length);

	//	Обновление размера байт-кода
    uint32_t byte_code_size = byteCode()->size() - header_size + 4;
	byteCode()->setData(BYTE_CODE_POSITION, byte_code_size);

	//	Вычисление контрольной суммы
	uint32_t crc = getCRC(byteCode(), uint32_t(0));
	byteCode()->appendData(crc);

	return true;
}

void CfcCompiler::createHeader(uint32_t ramsize, QString title)
{
	//	Размер заголовка алгоритма алгоритма
	uint16_t header_length = 0x00;
    uint16_t header_length_position = _byte_code->size();
	byteCode()->append(HEADER_TAG);
	byteCode()->appendComplexLength(header_length);

	//	Общий размер файла
	uint32_t file_length = 0x00;
	byteCode()->appendData(file_length);

	//	Контрольная сумма заголовка
	byteCode()->appendData(HEADER_CRC);

	//	Размер преамбулы
	uint16_t preambule = 0x00;
    uint16_t preambule_position = byteCode()->size();
	byteCode()->appendData(preambule);

	//	Размер байт-кода
	uint32_t byte_code_size = 0x00;
	byteCode()->appendData(byte_code_size);

	//	Объем выделенной памяти
	byteCode()->appendData(ramsize);

	//	Частота запуска программы
	byteCode()->appendData(RUNTIME_US);

	//	Максимальное время работы алгоритма
	byteCode()->appendData(MAX_RUNTIME_US);

	//	Приоритет задачи 
	uint8_t priority = 0;
	byteCode()->append(priority);

	//	Добавление названия схемы
    if (title.size() > TITLE_SIZE) title = title.left(TITLE_SIZE);
    for (int i = 0; i < title.size(); i++)
        byteCode()->append(title.toLocal8Bit().at(i));
    for (int i = title.size(); i < TITLE_SIZE; i++)
        byteCode()->append(char(0x00));

	//  Фиксация длины заголовка
    header_length = byteCode()->size() - header_length_position -4;
	byteCode()->setComplexLength(1, header_length);

	//  Фиксация длины преамбулы
    preambule = byteCode()->size() - 4;
	byteCode()->setData(preambule_position, preambule);

	return;
}

uint32_t CfcCompiler::maxVarsSize(QList<CompileElement>& nodes_list)
{
	int max_vars_size = 0;
	int pn;
	for (int ind = 0; ind < nodes_list.count(); ind++) {
		for (pn = 0; pn < nodes_list[ind].inputs.size(); pn++) {
			if (nodes_list[ind].inputs[pn].link_number > max_vars_size)
                max_vars_size = nodes_list[ind].inputs[pn].link_number;
        }
		for (pn = 0; pn < nodes_list[ind].outputs.size(); pn++) {
			if (nodes_list[ind].outputs[pn].link_number > max_vars_size)
                max_vars_size = nodes_list[ind].outputs[pn].link_number;
        }
	}

	return max_vars_size + 1;
}

uint32_t CfcCompiler::totalConnections(QList<CompileElement>& nodes_list)
{
	uint32_t total_connections = 0;
	for (int i = 0; i < nodes_list.count(); i++) {
        for (int pn = 0; pn < nodes_list.at(i).inputs.size(); pn++)
            total_connections++;
        for (int pn = 0; pn < nodes_list.at(i).outputs.size(); pn++)
            total_connections++; }

	return total_connections;
}

void CfcCompiler::createNode(const CompileElement& node)
{
	byteCode()->append(NODE_TAG);
    uint16_t node_length_position = byteCode()->size();
	uint16_t node_length = 0x00;
	byteCode()->appendComplexLength(node_length);

	node_length += setNodeType(node.type);
	node_length += setNodeParams(node.params);
	node_length += setNodeIO(node.inputs, false);
	node_length += setNodeIO(node.outputs, true);
	node_length += 4;
	byteCode()->setComplexLength(node_length_position, node_length);

	return;
}

int CfcCompiler::setNodeType(uint8_t type)
{
    if (type == 0)
        return 0;

    byteCode()->append(NODE_TYPE_TAG);
    byteCode()->appendComplexLength(uint16_t(1));
    byteCode()->append(type);

	return 5;
}

int CfcCompiler::setNodeParams(const QList<int32_t>& params)
{
	byteCode()->append(NODE_PARAM_TAG);
	uint16_t param_length = 0;
    uint16_t param_length_position = byteCode()->size();
	byteCode()->appendComplexLength(param_length);

    for (int i = 0; i < params.count(); i++) {
		byteCode()->append(INTEGER_TAG);
		byteCode()->append(0x04);
		uint32_t param = params.at(i);
        byteCode()->appendData(param);
    }

    param_length = byteCode()->size() - param_length_position - 3;
	byteCode()->setComplexLength(param_length_position, param_length);

	return param_length;
}

int CfcCompiler::setNodeIO(const QList<CompileLink> channel, bool channel_type)
{
	channel_type ? byteCode()->append(NODE_OUTPUT_TAG) : byteCode()->append(NODE_INPUT_TAG);
	uint16_t channel_length = 0;
    uint16_t channel_length_position = _byte_code->size();
	byteCode()->appendComplexLength(channel_length);
	byteCode()->append(INTEGER_TAG);
	byteCode()->append(0x04);
	uint32_t channel_size = channel.size();
	byteCode()->appendData(channel_size);

    for (uint32_t i = 0; i < channel_size; i++) {
		byteCode()->append(INTEGER_TAG);
		byteCode()->append(0x04);
		uint32_t link_number = channel.at(i).link_number;
		byteCode()->appendData(link_number);

		byteCode()->append(INTEGER_TAG);
		byteCode()->append(0x04);
		uint32_t pin_number = channel.at(i).pin_number;
		byteCode()->appendData(pin_number);

		byteCode()->append(INTEGER_TAG);
		byteCode()->append(0x04);
		uint32_t type = channel.at(i).type;
		byteCode()->appendData(type);

		byteCode()->append(BOOL_TAG);
		byteCode()->append(0x01);
        byteCode()->append(uint8_t(channel.at(i).inversion));
    }

    channel_length = byteCode()->size() - channel_length_position - 3;
	byteCode()->setComplexLength(channel_length_position, channel_length);

	return channel_length + 4;
}

uint32_t CfcCompiler::getCRC(ByteCode* byte_code, uint32_t init)
{
	uint32_t crc = init ^ 0xFFFFFFFFUL;

    int size = byte_code->size();
	for (int ii = 0; ii < size; ii++) {
		uint32_t ti = byte_code->at(ii);
		ti = (crc ^ ti) & 0xFF;
		for (int i = 0; i < 8; i++) ti = ti & 1 ? (ti >> 1) ^ 0xEDB88320UL : ti >> 1;
        crc = ti ^ (crc >> 8);
    }
	crc ^= 0xFFFFFFFFUL;

	return crc;
}


//===================================================================================================================================================
//	Обработка сигналов парсера
//===================================================================================================================================================
void CfcCompiler::onParserError(const QString& message)
{
    emit errorToLog(message);
}

void CfcCompiler::onParserInfo(const QString& message)
{
    emit infoToLog(message);
}
