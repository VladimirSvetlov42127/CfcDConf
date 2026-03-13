#include "dc_alg_internal.h"
#include <db/dc_db_manager.h>

DcAlgInternal::DcAlgInternal(int32_t algid, int32_t position, const QString &name, const QString &properties, DcController *device)
    : DcAlg(algid, position, name)
    , DcDbWrapper{device}
{
    m_ios = new DcPoolSingleKey<DcAlgIOInternal*>();
	m_properties.fromJson(properties);
}

DcAlgInternal::~DcAlgInternal()
{
    m_ios->clear();
}

void DcAlgInternal::setDevice(DcController *device)
{
    DcDbWrapper::setDevice(device);
    for(size_t i = 0; i < m_ios->size(); ++i)
        m_ios->get(i)->setDevice(device);
}

DcPoolSingleKey<DcAlgIOInternal*>* DcAlgInternal::ios()
{
	return m_ios;
}

QString DcAlgInternal::property(const QString &name) const
{
	return m_properties.get(name);
}

const DcProperties &DcAlgInternal::properties() const
{
    return m_properties;
}

void DcAlgInternal::updatedb() {
	update();
}

bool DcAlgInternal::operator==(const DcAlgInternal & other) const
{
	if (position() == other.position() && 
		name().trimmed().toUpper() == other.name().trimmed().toUpper() && 
		m_properties == other.m_properties)
		return true;

	return false;
}

//bool DcAlgInternal::updateProperty(QString & name, QString & value)
//{
//	if (name.isEmpty())
//		return false;
//	if (value.isEmpty())
//		return false;

//    m_properties.set(name, value);
//	update(); // generate update sql and push to sql queue
//	return true;
//}

void DcAlgInternal::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE algs SET name = '%1', properties = '%2' WHERE alg_id = %3;";
    QString updateStr = updateStrTmp.arg(name(), properties().toJson(), QString::number(index()));
	gDbManager.execute(uid(), updateStr);
}

void DcAlgInternal::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO algs(alg_id, position, name, properties) VALUES(%1, %2, '%3', '%4');";
    QString insertStr = insertStrTmp.arg(QString::number(index()), QString::number(position()), name(), properties().toJson());
    gDbManager.execute(uid(), insertStr);
}

void DcAlgInternal::remove()
{
    m_ios->removeAll();
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM algs WHERE alg_id = %1;";
    QString removeStr = removeStrTmp.arg(QString::number(index()));
    gDbManager.execute(uid(), removeStr);
}
