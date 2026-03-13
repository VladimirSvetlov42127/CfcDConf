#include "dc_alg_io_cfc.h"
#include <db/dc_db_manager.h>

DcAlgIOCfc::DcAlgIOCfc(int32_t uid, int32_t alg_id, int32_t alg_pin, DefIoDirection direction, const QString &name, DcController *device)
    : DcAlgIO(uid, alg_id, alg_pin, direction, name)
    , DcDbWrapper{device}
{
}

void DcAlgIOCfc::updateNamedb(void)
{
	update();
}

void DcAlgIOCfc::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE alg_cfc_io SET name = '%1' WHERE cfc_io_id = %2;";
    QString updateStr = updateStrTmp.arg(name(), QString::number(index()));
    gDbManager.execute(uid(), updateStr);
}

void DcAlgIOCfc::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO alg_cfc_io(cfc_io_id, cfc_alg_id, cfc_alg_pin, cfc_io_direction, name) VALUES(%1, %2, %3, %4, '%5');";
    QString insertStr = insertStrTmp.arg(QString::number(index()), QString::number(alg()), QString::number(pin()), QString::number((int)direction()), name());
    gDbManager.execute(uid(), insertStr);
}

void DcAlgIOCfc::remove()
{
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM alg_cfc_io WHERE cfc_io_id = %1;";
    QString removeStr = removeStrTmp.arg(QString::number(index()));
    gDbManager.execute(uid(), removeStr);
}
