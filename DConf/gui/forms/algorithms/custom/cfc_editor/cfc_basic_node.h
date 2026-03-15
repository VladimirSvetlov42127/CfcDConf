#ifndef __CFC_BASIC_NODE_H__
#define __CFC_BASIC_NODE_H__


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QString>
#include <QGraphicsItem>
#include <QDomNode>
#include <QList>
#include <QSizeF>
#include <QVariant>
#include <QRectF>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
//class CfcSocket;
#include "gui/forms/algorithms/custom/cfc_editor/cfc_socket.h"

//===================================================================================================================================================
//	Параметры элемента
//===================================================================================================================================================
struct NodeParam {
    QString name;
    QString index;
    QVariant value;
    QString info; };

//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    enum RzaTypes : uint8_t {
        RZA_DEFAULT = 0,
        RZA_AND,					//	And
        RZA_OR,                     //	Or
        RZA_DELAY_ON,				//	TriggerDelay
        RZA_DELAY_OFF,				//	ReturnDelay
        RZA_TRIGGER,				//	RsTrigger
        RZA_PULSE_REPETITIVE,		//	Generator
        RZA_IMPULSE_LEADING,		//	ImpulsePF
        RZA_IMPULSE_TRAILING,		//	ImpulseZF
        RZA_MAX,					//	Max
        RZA_MIN,					//	Min
        RZA_THRESHOLD,				//	Threshold
        RZA_XOR,					//	Xor
        RZA_INVERS,                 //	Not
        RZA_INVERSE_DELAY,			//	RPICKUP_DELAY_DEPEND	????
        RZA_READ = 61,
        RZA_WRITE = 62,
        RZA_LOAD = 131,             //	BI или BO
        RZA_TIME = 153
    };
}


//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
//	Базовый класс для всех элементов гибкой логики
//  Методы работы со свойствами класса
//===================================================================================================================================================
class CfcBasicNode : public QGraphicsObject
{
public:
    //===============================================================================================================================================
    //	Конструкторы класса
    //===============================================================================================================================================
    CfcBasicNode(QString id = QString(), QGraphicsItem* parent = nullptr);
    CfcBasicNode(QDomNode xml, QGraphicsItem* parent = nullptr);

    //===============================================================================================================================================
    //	Открытые методы класса
    //===============================================================================================================================================
    QString id() const { return _id; }
    QString name() const { return _name; }
    void setName(const QString& name) { _name = name; }
    uint8_t nodeType() const { return _type; }
    void setNodeType(uint8_t type) { _type = type; }
    bool inversion() const { return _inversion; }
    void setInversion(bool flag) { _inversion = flag; }
    QSizeF size() const { return _size; }
    void setSize(const QSizeF& size) { _size = size; }
    uint8_t minInputs() const { return _min_inputs; }
    uint8_t maxInputs() const { return _max_inputs; }
    virtual QList<NodeParam> paramsList() const { return _params_list; }
    QList<CfcSocket*> sockets() const { return _sockets; }
    QStringList paramNames() const { return _param_names; }
    QStringList paramValues() const { return _param_values; }
    QRectF availableRect() const { return QRectF(QPointF(0, 0), _size); }

    uint8_t inputs();
    void initInputs(uint8_t min, uint8_t max);
    bool hasOutput() const;
    void setOutput();
    void setInputs(uint8_t count);
    void addInput();
    void removeInput();

    //===============================================================================================================================================
    //	Параметры элемента
    //===============================================================================================================================================
    void addParam(QString name, QString id, QVariant value, QString short_info);
    void addParam(QString name, QString id, QVariant value);
    NodeParam param(QString id) const;
    void setParam(QString id, QVariant value);
    void setParam(int index, QVariant value);

    //===============================================================================================================================================
    //	Сериализация элемента
    //===============================================================================================================================================
    virtual QDomNode toXML();

private:
    //===============================================================================================================================================
    //	Свойства класса
    //===============================================================================================================================================
    static inline int _counter { 1 };       //  Счетчик для формирования идентификатора элемента
    QString _id;                            //	Уникальный идентификатор элемента (QString для обратной совместимости)
    QString _name;                          //  Название элемента
    uint8_t _type;                          //  Тип элемента (для компилятора)
    bool _inversion;                        //  Признак инверсии выхода
    QSizeF _size;                           //  Размер элемента
    uint8_t _min_inputs;                    //  Минимальное количество входных сокетов
    uint8_t _max_inputs;                    //  Максимальное количество входных сокетов
    QList<CfcSocket*> _sockets;             //  Список сокетов (если есть выходной сокет - он всегда первый)
    QList<NodeParam> _params_list;          //  Список параметров элнмента
    QStringList _param_names;               //  Список названий параметров (для парсера XML)
    QStringList _param_values;              //  Список значений параметров (для парсера XML)
};

#endif // __CFC_BASIC_NODE_H__
