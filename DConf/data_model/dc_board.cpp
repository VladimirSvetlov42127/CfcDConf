#include "dc_board.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QHash>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <db/dc_db_manager.h>

//===================================================================================================================================================
//	namespace и списки переменных
//===================================================================================================================================================
namespace
{
    const char* PROPERTY_DIN = "Din";
    const char* PROPERTY_DOUT = "Dout";
    const char* PROPERTY_AINS_OSC = "AinsOsc";
}  // namespace


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
DcBoard::DcBoard(int32_t id, int32_t slot, const QString &type, int32_t instance, const QString &properties, DcController *device)
    : DcDbWrapper{device}
    , _id(id)
    , _slot(slot)
    , _type(type)
    , _inst(instance)
{
    _params.setDevice(device);

    m_properties.fromJson(properties);
    _dins_count = m_properties.get(PROPERTY_DIN).toUInt();
    _douts_count = m_properties.get(PROPERTY_DOUT).toUInt();
    _ains_osc = m_properties.get(PROPERTY_AINS_OSC).toUInt();
}

DcBoard::~DcBoard()
{
}

void DcBoard::setDevice(DcController *device)
{
    DcDbWrapper::setDevice(device);
    _params.setDevice(device);
}

int32_t DcBoard::uid() const
{
    return device() ? device()->uid() : 0;
}

QString DcBoard::properties() const
{
    return m_properties.toJson();
}

//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void DcBoard::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO boards(board_id, slot, type, inst, properties) VALUES(%1, %2, '%3', %4, '%5');";
    QString insertStr = insertStrTmp.arg(id()).arg(slot()).arg(type()).arg(inst()).arg(properties());
    gDbManager.execute(uid(), insertStr);
}

void DcBoard::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE boards SET slot = %1, type = '%2', inst = %3, properties = '%4' WHERE board_id = %5;";
    QString updateStr = updateStrTmp.arg(slot()).arg(type()).arg(inst()).arg(properties()).arg(id());
	gDbManager.execute(uid(), updateStr);
}

void DcBoard::remove()
{
    if (!uid())
        return;

    QString deleteStrTmp = "DELETE FROM boards WHERE board_id = %1;";
    QString deleteStr = deleteStrTmp.arg(id());
    gDbManager.execute(uid(), deleteStr);
}

void DcBoard::updateStorage(int32_t uid)
{
    QString str = "ALTER TABLE boards ADD COLUMN properties TEXT;";
    gDbManager.execute(uid, str);
}
