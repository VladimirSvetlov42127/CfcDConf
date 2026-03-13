#include "flexlogic_namespace.h"

namespace FlexLogic
{
    //  Общие параметры программы
    ProgramMode PROGRAM_MODE = ProgramMode::NORMAL_MODE;
    QString VERSION_MAJOR = "1";
    QString VERSION_MINOR = "0";
    QString VERSION_REVISION = "0";
    QString VERSION_BUILD = "0";
    QString PROGRAM_VERSION = VERSION_MAJOR + "." + VERSION_MINOR + "." + VERSION_REVISION;
    QString PROGRAM_FILE_VERSION = PROGRAM_VERSION + "." + VERSION_BUILD;
    int CFC_VERSION = 0;

    //  Параметры расположения текста на элементах
    QFont CHANNEL_TEXT_FONT = QFont("Arial", 11);
    QFont LABEL_TEXT_FONT = QFont("Arial", 11, QFont::Bold);
    QFont INTO_TEXT_FONT = QFont("Arial", 8);
    uint8_t FROM_LEFT_TO_TEXT = 8;
    uint8_t FROM_BOTTOM_TO_TEXT = 5;
    uint8_t FROM_TOP_TO_TEXT = 11;

    //  Параметры вывода сцены
    QColor scene_bkcolor = QColor(255,255,255);
    bool grid_enable = false;                           //  Флаг активизации сетки
    uint8_t grid_step = 50;                             //  Крупный шаг сетки
    uint8_t grid_small_step = 10;                       //  Мелкий шаг сетки
    QColor grid_color = QColor(200, 200, 255, 125);     //  Цвет сетки

    //  Параметры Zoom
    double zoom_factor_base = 1.0015;
    double zoom_current_factor = 1.0;
    double zoom_factor_min = 0.01;
    double zoom_factor_max = 100.;
    QPointF target_scene_pos = QPointF(0, 0);
    QPointF target_viewport_pos = QPointF(0, 0);
    int delta = 5;

    //  Параметры выделения элементов
    uint8_t select_shape = 10;
    uint8_t select_box = 4;
    QColor select_color = QColor(Qt::blue);

    //  Параметры рисования элемента
    QColor shape_color = QColor(70, 100, 120);
    QColor shape_bkcolor = QColor(245, 245, 245);
    QColor shape_bkcolor_binded = QColor(255, 168, 87);
    int shape_width = 2;

    //  Параметры сокета
    int socket_radius = 3;
    QColor input_bkcolor = QColor(Qt::cyan);
    QColor output_bkcolor = QColor(Qt::yellow);
    QColor socket_select_color = QColor(255, 0, 0);
    QColor socket_select_bkcolor = QColor(255, 150, 150);

    //  Параметры соединений
    QColor connection_color = QColor(220, 95, 30);
    int connection_width = 2;
    int connection_radius = 3;
    int intersection_radius = 5;

};  //  FlexLogicNamespace

