#include "dc_alg_io_internal.h"
#include <db/dc_db_manager.h>

DcAlgIOInternal::DcAlgIOInternal(int32_t uid, int32_t alg_id, int32_t alg_pin, DefIoDirection direction, const QString &name, DcController *device)
    : DcAlgIO(uid, alg_id, alg_pin, direction, name)
    , DcDbWrapper{device}
{
	
}

void DcAlgIOInternal::updateNamedb(void) {
	update();
}

void DcAlgIOInternal::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE alg_io SET name = '%1' WHERE io_id = %2;";
    QString updateStr = updateStrTmp.arg(name(), index());
    gDbManager.execute(uid(), updateStr);
}

void DcAlgIOInternal::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO alg_io(io_id, alg_id, alg_pin, io_direction, name) VALUES(%1, %2, %3, %4, '%5');";
    QString insertStr = insertStrTmp.arg(QString::number(index()), QString::number(alg()), QString::number(pin()), QString::number((int)direction()), name());
    gDbManager.execute(uid(), insertStr);
}

void DcAlgIOInternal::remove()
{
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM alg_io WHERE io_id = %1;";
    QString removeStr = removeStrTmp.arg(QString::number(index()));
    gDbManager.execute(uid(), removeStr);
}
