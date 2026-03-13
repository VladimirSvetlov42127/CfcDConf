#ifndef __FLEXLOGIC_STRUCT_H__
#define __FLEXLOGIC_STRUCT_H__


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "flexlogic_namespace.h"


//===================================================================================================================================================
//	Списки данных и структуры проекта
//===================================================================================================================================================
typedef enum ElementType
{
    TYPE_DEFAULT = 0,
    TYPE_AND,					//	And
    TYPE_OR,					//	Or
    TYPE_DELAY_ON,				//	TriggerDelay
    TYPE_DELAY_OFF,				//	ReturnDelay
    TYPE_TRIGGER,				//	RsTrigger
    TYPE_PULSE_REPETITIVE,		//	Generator
    TYPE_IMPULSE_LEADING,		//	ImpulsePF
    TYPE_IMPULSE_TRAILING,		//	ImpulseZF
    TYPE_MAX,					//	Max
    TYPE_MIN,					//	Min
    TYPE_THRESHOLD,				//	Threshold
    TYPE_XOR,					//	Xor
    TYPE_INVERS,				//	Not
    TYPE_INVERSE_DELAY,			//	RPICKUP_DELAY_DEPEND	????
    TYPE_READ = 61,
    TYPE_WRITE = 62,
    TYPE_LOAD = 131,			//	BI или BO	????
    TYPE_TIME = 153
} ElementType;

//typedef enum {
//    VARIABLE_DISCRETE = 0,
//    VARIABLE_ANALOG = 1,
//    VARIABLE_INT32 = 2
//} VariableType;

struct NodeParam {
    QString name;
    QString index;
    QVariant value;
    QString info;
};

//struct CompileLink {
//    QString socket_id;
//    QStringList links_list;
//    VariableType type;
//    bool inversion;
//    int pin_number;
//    int16_t link_number;
//};

//struct CompileElement {
//    ElementType type;               //  Тип узла
//    QList<int32_t> params;          //  Список значений параметров
//    QList<CompileLink> inputs;      //  Список входов
//    QList<CompileLink> outputs;     //  Список выходов
//};

//===================================================================================================================================================
//	Структуры для копирования/вставки элементов
//===================================================================================================================================================
struct MemoryLink {
    QPointF position;               //  Положение на сцене
    QList<QPointF> points;          //  Список узлов соединения
    int from_node;                  //  Номер узла источника
    int from_socket;                //  Номер сокета источника
    int to_node;                    //  Номер узла цели
    int to_socket;                  //  Номер сокета цели
};

struct MemorySocket {
    QList<int> links;               //  Список номеров связей
};

struct MemoryNode {
    QPointF position;               //  Положение на сцене
    QString type;                   //  Тип узла
    QVariantList params;            //  Список значений параметров
    QList<MemorySocket> sockets;    //  Список сокетов
};


#endif	//	__FLEXLOGIC_STRUCT_H__