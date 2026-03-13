#include "dc_matrix_element_alg.h"
#include <db/dc_db_manager.h>

DcMatrixElementAlg::DcMatrixElementAlg(int32_t src, int32_t dst, const QString &properties, DcController *device)
    : DcMatrixElement(src, dst, properties)
    , DcDbWrapper{device}

{
}

void DcMatrixElementAlg::updatedb()
{
	update();
}

void DcMatrixElementAlg::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO matrix_alg(alg_io_id, signal_id) VALUES(%1, %2);";

    QString insertStr = insertStrTmp.arg(QString::number(src()), QString::number(dst()));
    gDbManager.execute(uid(), insertStr);
}

void DcMatrixElementAlg::update()
{
    if (!uid())
        return;

    /*
    QString updateStrTmp = "UPDATE matrix_alg SET properties = '%1' WHERE alg_io_id = %2 and signal_id = %3;";

    QString updateStr = updateStrTmp.arg("", QString::number(src()), QString::number(dst()));
    gDbManager.execute(uid(), updateStr);
    */ //TODO properties not used yet in matrix
}

void DcMatrixElementAlg::remove()
{
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM matrix_alg WHERE alg_io_id = %1 and signal_id=%2;";
    QString removeStr = removeStrTmp.arg(QString::number(src()), QString::number(dst()));
    gDbManager.execute(uid(), removeStr);
}
