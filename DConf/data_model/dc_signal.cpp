#include "dc_signal.h"
#include <db/dc_db_manager.h>

namespace {

constexpr const char* PROPERTY_FIXED = "fixed";
constexpr const char* PROPERTY_DISABLE_CLONING = "disable_cloning";

} // namespace

DcSignal::DcSignal(int32_t id, int32_t internalid, DefSignalDirection direction,
    DefSignalType type, DefSignalSubType subtype, const QString &name, const QString &properties, DcController *device)
    : DcDbWrapper{device}
    , m_id(id)
    , m_internal_id(internalid)
    , m_direction(direction)
    , m_type(type)
    , m_subtype(subtype)
    , m_name(name)
{
	m_properties.fromJson(properties);
}

int32_t DcSignal::index() {
	return m_id;
}

int32_t DcSignal::internalId() {
	return m_internal_id;
}

DefSignalDirection DcSignal::direction() {
	return m_direction;
}

DefSignalType DcSignal::type() {
	return m_type;
}

DefSignalSubType DcSignal::subtype() {
	return m_subtype;
}

QString DcSignal::name(){
	return m_name;
}

QString DcSignal::property(QString name) const {
	return m_properties.get(name);
}

bool DcSignal::updateProperty(const QString &name, const QString &value) {
	if (name.isEmpty())
		return false;
	if (value.isEmpty())
		return false;

    m_properties.set(name, value);
	update(); // generate update sql and push to sql queue
	return true;
}

bool DcSignal::updateName(const QString & newname)
{
	if (newname.isEmpty()) return false;
	if (m_name.compare(newname) == 0) return true;
	m_name = newname;
	update();
	return true;
}

bool DcSignal::isFixed() const
{
	return property(PROPERTY_FIXED).toInt();
}

bool DcSignal::isCloningEnabled() const
{
    return !property(PROPERTY_DISABLE_CLONING).toUInt();
}

const DcProperties &DcSignal::properties() const
{
    return m_properties;
}

void DcSignal::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE signals SET name = '%1', properties = '%2' WHERE signal_id = %3;";
    QString updateStr = updateStrTmp.arg(name(), properties().toJson(), QString::number(index()));
    gDbManager.execute(uid(), updateStr);
}

void DcSignal::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO signals(signal_id, internal_id, direction, type, sub_type, name, properties) VALUES(%1, %2, %3, %4, %5, '%6', '%7');";
    QString insertStr = insertStrTmp.arg(QString::number(index()),
                                         QString::number(internalId()),
                                         QString::number(direction()),
                                         QString::number(type()),
                                         QString::number(subtype()),
                                         name(),
                                         properties().toJson());
    gDbManager.execute(uid(), insertStr);

}

void DcSignal::remove()
{
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM signals WHERE signal_id = %1;";
    QString removeStr = removeStrTmp.arg(QString::number(index()));
    gDbManager.execute(uid(), removeStr);
}
