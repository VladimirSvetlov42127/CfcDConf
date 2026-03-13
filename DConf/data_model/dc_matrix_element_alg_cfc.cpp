#include "dc_matrix_element_alg_cfc.h"
#include <db/dc_db_manager.h>

DcMatrixElementAlgCfc::DcMatrixElementAlgCfc(int32_t src, int32_t dst, const QString &properties, DcController *device)
    : DcMatrixElement(src, dst, properties)
    , DcDbWrapper{device}
{
}

void DcMatrixElementAlgCfc::updatedb()
{
	update();
}

void DcMatrixElementAlgCfc::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO matrix_alg_cfc(alg_cfc_io_id, signal_id) VALUES(%1, %2);";

    QString insertStr = insertStrTmp.arg(QString::number(src()), QString::number(dst()));
    gDbManager.execute(uid(), insertStr);
}

void DcMatrixElementAlgCfc::update()
{
    if (!uid())
        return;

    /*
    QString updateStrTmp = "UPDATE matrix_alg_cfc SET properties = '%1' WHERE alg_cfc_io_id = %2 and signal_id = %3;";

    QString updateStr = updateStrTmp.arg("", QString::number(src()), QString::number(dst()));
    gDbManager.execute(uid(), updateStr);
    */ //TODO properties not used yet in matrix
}

void DcMatrixElementAlgCfc::remove()
{
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM matrix_alg_cfc WHERE alg_cfc_io_id = %1 and signal_id=%2;";
    QString removeStr = removeStrTmp.arg(QString::number(src()), QString::number(dst()));
    gDbManager.execute(uid(), removeStr);
}
