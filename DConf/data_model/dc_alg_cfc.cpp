#include "dc_alg_cfc.h"
#include <db/dc_db_manager.h>

DcAlgCfc::DcAlgCfc(int32_t algid, int32_t position, const QString &name, const QString &properties, DcController *device)
    : DcAlg(algid, position, name)
    , DcDbWrapper{device}
{
    m_ios = new DcPoolSingleKey<DcAlgIOCfc*>();
    m_properties.fromJson(properties);
}

DcAlgCfc::~DcAlgCfc()
{
    m_ios->clear();
}

void DcAlgCfc::setDevice(DcController *device)
{
    DcDbWrapper::setDevice(device);
    for(size_t i = 0; i < m_ios->size(); ++i)
        m_ios->get(i)->setDevice(device);
}

DcPoolSingleKey<DcAlgIOCfc*>* DcAlgCfc::ios()
{
    return m_ios;
}

QString DcAlgCfc::property(const QString &name) const
{
    return m_properties.get(name);
}

const DcProperties& DcAlgCfc::properties() const
{
    return m_properties;
}

bool DcAlgCfc::updateProperty(const QString & name, const QString & value)
{
    if (name.isEmpty())
        return false;
    if (value.isEmpty())
        return false;

    m_properties.set(name, value);
    update(); // generate update sql and push to sql queue
    return true;
}

void DcAlgCfc::updatedb() {
    update();
}

void DcAlgCfc::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE algs_cfc SET name = '%1', properties = '%2' WHERE cfc_alg_id = %3;";
    QString updateStr = updateStrTmp.arg(name(), properties().toJson(), QString::number(index()));
    gDbManager.execute(uid(), updateStr);
}

void DcAlgCfc::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO algs_cfc(cfc_alg_id, cfc_position, name, properties) VALUES(%1, %2, '%3', '%4');";
    QString insertStr = insertStrTmp.arg(QString::number(index()), QString::number(position()), name(), properties().toJson());
    gDbManager.execute(uid(), insertStr);
}

void DcAlgCfc::remove()
{
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM algs_cfc WHERE cfc_alg_id = %1;";
    QString removeStr = removeStrTmp.arg(QString::number(index()));
    gDbManager.execute(uid(), removeStr);
}
