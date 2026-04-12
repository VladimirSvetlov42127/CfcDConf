#include "cfc_namespace.h"

namespace CfcNamespace
{
    //  Параметры вывода сцены
    bool grid_enable = false;                           //  Флаг активизации сетки
    uint8_t grid_step = 50;                             //  Крупный шаг сетки
    uint8_t grid_small_step = 10;                       //  Мелкий шаг сетки
    QColor grid_color = QColor(200, 200, 255, 125);     //  Цвет сетки
    uint8_t grid_move = 5;                              //  Шаг перемещения при привязке к сетке

    //  Расчет координат с привязкой к сетке
    QPointF gridPoint(const QPointF &point) {
        qreal x = floor(point.x()/grid_move)*grid_move;
        qreal y = floor(point.y()/grid_move)*grid_move;
        return QPointF(x, y);
    }


};
