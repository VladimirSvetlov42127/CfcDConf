#include "parameter.h"

#include "db/dc_db_manager.h"

using Dpc::Sybus::ParamAttribute;

namespace {

constexpr const uint16_t g_maxProfilesCount = std::numeric_limits<uint8_t>::max() + 1;
constexpr const uint16_t g_max1DelementsCount = std::numeric_limits<uint16_t>::max();
constexpr const uint16_t g_max2DelementsCount = std::numeric_limits<uint8_t>::max() + 1;

}

Parameter::Parameter(const QString &name, uint8_t type, uint16_t addr, const Dpc::Sybus::ParamAttribute &attrs, uint16_t profilesCount, uint16_t elementsCount, uint16_t dataSize)
    : m_name{name}
    , m_type{type}
    , m_addr{addr}
    , m_attrs{attrs}
    , m_profiles{std::min(profilesCount, g_maxProfilesCount)}
    , m_elementsCount{std::min(elementsCount, maxElementsCount())}
    , m_dataSize{dataSize}
{
    for(size_t i = 0; i < m_profiles.size(); ++i) {
        auto &profile = m_profiles.at(i);
        profile.reserve(this->elementsCount());
        for(size_t j = 0; j < this->elementsCount(); ++j)
            profile.emplace_back(std::make_unique<ParameterElement>( i * g_maxProfilesCount + j, this->name(), QString(), this));
    }
}

Parameter::Parameter(uint8_t type, uint16_t addr, const Dpc::Sybus::ParamAttribute &attrs)
    : m_type{type}
    , m_addr{addr}
    , m_attrs{attrs}
    , m_profiles{1} // При отсутствии определений, считаем что всего 1 профиль элементов
    , m_elementsCount{0}
    , m_dataSize{0}    
    , m_isVirtual{true}
{
}

int32_t Parameter::uid() const
{
    return registry() ? registry()->uid() : 0;
}

QString Parameter::name() const
{
    return m_name;
}

void Parameter::setName(const QString &name)
{
    if (this->name() == name)
        return;

    m_name = name;
    update();
}

uint8_t Parameter::type() const
{
    return m_type;
}

void Parameter::setType(uint8_t type)
{
    if (this->type() == type)
        return;

    m_type = type;
    update();
}

uint16_t Parameter::addr() const
{
    return m_addr;
}

Dpc::Sybus::ParamAttribute Parameter::attributes() const
{
    return m_attrs;
}

void Parameter::setAttributes(const Dpc::Sybus::ParamAttribute &attr)
{
    if (this->attributes() == attr)
        return;

    auto was2D = this->attributes()[ParamAttribute::_2D];
    m_attrs = attr;
    update();

    auto nowIs2D = this->attributes()[ParamAttribute::_2D];
    if (was2D && !nowIs2D)
        setProfilesCount(1);
    if (!was2D && nowIs2D)
        setElementsCount(std::min(elementsCount(), maxElementsCount()));
}

uint16_t Parameter::profilesCount() const
{
    return m_profiles.size();
}

void Parameter::setProfilesCount(uint16_t count)
{
    // Нельзя превышать максимум
    count = std::min(count, g_maxProfilesCount);
    if (profilesCount() == count)
        return;

    // У вирутального и одномерного параметра нельзя поменять, всегда равен 1.
    if (isVirtual() || !attributes()[ParamAttribute::_2D])
        return;

    while(profilesCount() > count) {
        for(auto& element: m_profiles.back())
            element->remove();

        m_profiles.pop_back();
    }

    for(auto i = profilesCount(); i < count; ++i ) {
        auto& profile = m_profiles.emplace_back();
        profile.reserve(elementsCount());
        for(size_t j = 0; j < elementsCount(); ++j) {
            auto element = std::make_unique<ParameterElement>( i * g_maxProfilesCount + j, this->name(), QString(), this);
            element->insert();
            profile.emplace_back(std::move(element));
        }
    }

    update();
}

uint16_t Parameter::elementsCount() const
{
    if (isVirtual())
        return profile(0)->size();

    return m_elementsCount;
}

void Parameter::setElementsCount(uint16_t count)
{
    // Нельзя превышать максимум
    count = std::min(count, maxElementsCount());
    if (elementsCount() == count)
        return;

    // У вирутального  нельзя поменять.
    if (isVirtual())
        return;

    for(size_t i = 0; i < m_profiles.size(); ++i) {
        auto &profile = m_profiles.at(i);
        while(profile.size() > count) {
            profile.back()->remove();
            profile.pop_back();
        }

        profile.reserve(count);
        for(auto j = elementsCount(); j < count; ++j ) {
            auto element = std::make_unique<ParameterElement>( i * g_maxProfilesCount + j, this->name(), QString(), this);
            element->insert();
            profile.emplace_back(std::move(element));
        }
    }

    m_elementsCount = count;
    update();
}

uint16_t Parameter::dataSize() const
{
    return m_dataSize;
}

void Parameter::setDataSize(uint16_t size)
{
    if (this->dataSize() == size)
        return;

    m_dataSize = size;
    update();
}

bool Parameter::isValid() const
{
    if (!type())
        return false;

    return true;
}

const ParameterRegistry *Parameter::registry() const
{
    return m_registry;
}

void Parameter::setRegistry(ParameterRegistry *registry)
{
    m_registry = registry;
}

const Parameter::Profile *Parameter::profile(uint8_t idx) const
{
    if (idx >= profiles().size())
        return nullptr;

    return &profiles().at(idx);
}

const std::vector<Parameter::Profile> &Parameter::profiles() const
{
    return m_profiles;
}

ParameterElement *Parameter::append(ParameterElement::UPtr element)
{
    if (!element || !isVirtual())
        return nullptr;

    auto profileIdx = profileIndex(element->position());
    if (profileIdx >= m_profiles.size())
        return nullptr;

    auto& pr = m_profiles.at(profileIdx);
    auto elementIdx = elementIndexPerProfile(element->position());
    if (elementIdx != pr.size())
        return nullptr;

    auto raw_ptr = pr.emplace_back(std::move(element)).get();
    raw_ptr->setParameter(this);
    raw_ptr->insert();
    return raw_ptr;
}

ParameterElement *Parameter::element(uint16_t position)
{
    return const_cast<ParameterElement*>(
                static_cast<const Parameter*>(this)->element(position)
                );
}

const ParameterElement *Parameter::element(uint16_t position) const
{
    auto profileIdx = profileIndex(position);
    auto elementIdx = elementIndexPerProfile(position);
    return element(profileIdx, elementIdx);
}

ParameterElement *Parameter::element(uint8_t profileIdx, uint16_t idx)
{
    return const_cast<ParameterElement*>(
                static_cast<const Parameter*>(this)->element(profileIdx, idx)
                );
}

const ParameterElement *Parameter::element(uint8_t profileIdx, uint16_t idx) const
{
    auto pr = profile(profileIdx);
    if (!pr)
        return nullptr;

    if (pr->size() <= idx)
        return nullptr;

    return pr->at(idx).get();
}

void Parameter::insert()
{
    for(auto &profile: profiles())
        for(auto &element: profile)
            element->insert();

    if (isVirtual())
        return;

    if (!uid())
        return;

    QString temp = "INSERT INTO cfg_parameters_headline(name, type, address, attributes, profiles_count, subProfiles_count, subProfile_size_in_bytes ) VALUES('%1', %2, %3, '%4', %5, %6, %7);";
    QString insertStr = temp.arg(name())
            .arg(type())
            .arg(addr())
            .arg(attributes().toString())
            .arg(profilesCount())
            .arg(elementsCount())
            .arg(dataSize());

    gDbManager.execute(uid(), insertStr);
}

void Parameter::update()
{
    if (isVirtual())
        return;

    if (!uid())
        return;

    QString temp = "UPDATE cfg_parameters_headline SET name = '%1', type = %2, attributes = '%3', profiles_count = %4, subProfiles_count = %5, subProfile_size_in_bytes = %6 WHERE address = %7;";
    QString updateStr = temp.arg(name())
            .arg(type())
            .arg(attributes().toString())
            .arg(profilesCount())
            .arg(elementsCount())
            .arg(dataSize())
            .arg(addr());

    gDbManager.execute(uid(), updateStr);
}

void Parameter::remove()
{
    for(auto& profile: profiles()) {
        for(auto &element: profile) {
            element->remove();
        }
    }

    if (isVirtual())
        return;

    if (!uid())
        return;

    QString temp = "DELETE FROM cfg_parameters_headline WHERE address = %1;";
    QString removeStr = temp.arg(addr());
    gDbManager.execute(uid(), removeStr);
}

Parameter::UPtr Parameter::clone() const
{
    auto copy = isVirtual() ?
                std::make_unique<Parameter>(type(), addr(), attributes()) :
                std::make_unique<Parameter>(name(), type(), addr(), attributes(), profilesCount(), elementsCount(), dataSize());

    for(size_t i = 0; i < profilesCount(); ++i) {
        auto& profile = m_profiles.at(i);
        auto& copyProfile = copy->m_profiles.at(i);
        for(size_t j = 0; j < profile.size(); ++j) {
            auto element = profile.at(j).get();
            if (isVirtual())
                copy->append(element->clone());
            else {
                copyProfile[j] = element->clone();
                copyProfile[j]->setParameter(copy.get());
            }
        }
    }

    return copy;
}

QString Parameter::toString() const
{
    QString res = QString("0x%1 Type(%2), Attr(%3), PCount(%4), SPCount(%5), DSize(%6), Name(%7):")
            .arg(QString::number(addr(), 16))
            .arg(type())
            .arg(attributes().toString())
            .arg(profilesCount())
            .arg(elementsCount())
            .arg(dataSize())
            .arg(name());

    for(auto &profile: profiles()) {
        for(auto &element: profile)
            res.append(QString("\n%1").arg(element->toString()));
    }

    return res;
}

bool Parameter::isEqual(Parameter *other) const
{
    auto res = addr() == other->addr()
            && type() == other->type()
            && attributes() == other->attributes()
            && dataSize() == other->dataSize()
            && name() == other->name()
            && profilesCount() == other->profilesCount()
            && elementsCount() == other->elementsCount();

    if (!res)
        return false;

    for(size_t i = 0; i < profilesCount(); ++i) {
        for(size_t j = 0; j < elementsCount(); ++j)
            if (!profile(i)->at(j)->isEqual(other->profile(i)->at(j).get()))
                return false;
    }

    return true;
}

uint16_t Parameter::maxElementsCount() const
{
    return attributes()[ParamAttribute::_2D] ? g_max2DelementsCount : g_max1DelementsCount;
}

//ParameterProfile *Parameter::profile(uint8_t idx)
//{
//    return const_cast<ParameterProfile*>(
//                static_cast<const Parameter*>(this)->profile(idx)
//                );
//}

uint8_t Parameter::profileIndex(uint16_t position) const
{
    return attributes()[ParamAttribute::_2D] ? position / g_max2DelementsCount : 0;
}

uint16_t Parameter::elementIndexPerProfile(uint16_t position) const
{
    return attributes()[ParamAttribute::_2D] ? position % g_max2DelementsCount : position;
}

bool Parameter::isVirtual() const
{
    return m_isVirtual;
}
