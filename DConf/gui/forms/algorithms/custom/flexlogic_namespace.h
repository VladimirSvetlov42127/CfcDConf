#ifndef __FLEXLOGIC_NAMESPACE_H__
#define __FLEXLOGIC_NAMESPACE_H__

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================
#include <cstdint>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QList>
#include <QString>
#include <QVariant>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QFont>
#include <QGraphicsSceneMouseEvent>


//===================================================================================================================================================
//	namespace и списки данных
//===================================================================================================================================================
namespace FlexLogic {
    enum class ProgramMode { NORMAL_MODE, DEBUG_MODE, SIMULATOR_MODE, MONITORING_MODE };
    enum class SocketType { INPUT_SOCKET, OUTPUT_SOCKET };
    enum class SceneStatus { IS_NOTHING, IS_MOVING, IS_DRAWING };
    enum class LineTypes { NONE, HORIZONTAL, VERTICAL, DIAGONAL };
    enum class LineNodes { NONE, START, END, MIDDLE };

    enum VariableType : uint8_t { VARIABLE_DISCRETE = 0, VARIABLE_ANALOG = 1, VARIABLE_INT32 = 2 };
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
        RZA_TIME = 153 };

    struct NodeParam {
        QString name;
        QString index;
        QVariant value;
        QString info; };

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

    struct CompileLink {
        QString socket_id;
        QStringList links_list;
        uint8_t type = 0;
        bool inversion = false;
        int pin_number = -1;
        int16_t link_number = -1;
    };

    struct CompileElement {
        uint8_t type = 0;               //  Тип узла
        QList<int32_t> params;          //  Список значений параметров
        QList<CompileLink> inputs;      //  Список входов
        QList<CompileLink> outputs;     //  Список выходов
    };

    //  Общие параметры программы
    extern ProgramMode PROGRAM_MODE;
    extern QString VERSION_MAJOR;
    extern QString VERSION_MINOR;
    extern QString VERSION_REVISION;
    extern QString VERSION_BUILD;
    extern QString PROGRAM_VERSION;
    extern QString PROGRAM_FILE_VERSION;
    extern int CFC_VERSION;

    //  Параметры расположения текста на элементах
    extern QFont CHANNEL_TEXT_FONT;
    extern QFont LABEL_TEXT_FONT;
    extern QFont INTO_TEXT_FONT;
    extern uint8_t FROM_LEFT_TO_TEXT;
    extern uint8_t FROM_BOTTOM_TO_TEXT;
    extern uint8_t FROM_TOP_TO_TEXT;

    //  Параметры вывода сцены
    extern QColor scene_bkcolor;
    extern bool grid_enable;
    extern uint8_t grid_step;
    extern uint8_t grid_small_step;
    extern QColor grid_color;

    //  Параметры Zoom
    extern double zoom_factor_base;
    extern double zoom_current_factor;
    extern double zoom_factor_min;
    extern double zoom_factor_max;
    extern QPointF target_scene_pos;
    extern QPointF target_viewport_pos;
    extern int delta;

    //  Параметры выделения элементов
    extern uint8_t select_shape;
    extern uint8_t select_box;
    extern QColor select_color;

    //  Параметры рисования элемента
    extern QColor shape_color;
    extern QColor shape_bkcolor;
    extern QColor shape_bkcolor_binded;
    extern int shape_width;

    //  Параметры сокета
    extern int socket_radius;
    extern QColor input_bkcolor;
    extern QColor output_bkcolor;
    extern QColor socket_select_color;
    extern QColor socket_select_bkcolor;

    //  Параметры соединений
    extern QColor connection_color;
    extern int connection_width;
    extern int connection_radius;
    extern int intersection_radius;

};  //  FlexLogic


#endif	//	__FLEXLOGIC_NAMESPACE_H__
