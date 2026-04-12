#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include <map>

#include "service_manager/signals/din_signal.h"
#include "service_manager/signals/ain_signal.h"
#include "service_manager/signals/cin_signal.h"
#include "service_manager/signals/dout_signal.h"
#include "service_manager/signals/dpin_signal.h"

#include "service_manager/signals/din_virtual_signal.h"

class DcController;

class SignalManager
{
public:
    SignalManager(DcController* config);

    // Инициализация таблиц привязки. Настройка привязок выходов
    bool init();

    // Сброс всех привязок выходов
    void clearBindings();

    // Настройка привязок выходов
    void rebind();

    // Шаблонный метод для получения сигналов по типу параметра шаблона и маске подтипов
    template<typename SignalType>
    std::vector<SignalType*> getSignals(Signal::Subtypes mask = Signal::Subtype::Any) const;

    // Шаблонный метод для получения сигнала по типу и internalID
    template<typename SignalType>
    SignalType* getSignal(uint16_t internalID) const;

    // Список занятых виртуальных входов
    std::vector<DinVirtualSignal*> busyVDins() const;

    // Список свободных виртуальных входов
    std::vector<DinVirtualSignal*> freeVDins() const;

    // Методы для добавления сигналов
    void addSignal(DinSignal::UPtr signal);
    void addSignal(AinSignal::UPtr signal);
    void addSignal(CinSignal::UPtr signal);
    void addSignal(DoutSignal::UPtr signal);

    // Метод удаления сигнала по указателю
    bool removeSignal(Signal* signal);

    // Методы для получения количества сигналов
    size_t size() const { return dinSize() + ainSize() + cinSize() + doutSize(); }
    size_t dinSize() const { return m_dins.size(); }
    size_t ainSize() const { return m_ains.size(); }
    size_t cinSize() const { return m_cins.size(); }
    size_t doutSize() const { return m_douts.size(); }

    // Методы для работы с двухпозиционными сигналами
    size_t dpinSize() const { return m_dpins.size(); }
    std::vector<DPinSignal*> getDPinSignals() const;

private:
    template<typename SignalType>
    const auto& getContainer() const;

    DcController* config() const;

private:
    DcController* m_config = nullptr;

    std::map<uint16_t, DinSignal::UPtr> m_dins;
    std::map<uint16_t, DoutSignal::UPtr> m_douts;
    std::map<uint16_t, AinSignal::UPtr> m_ains;
    std::map<uint16_t, CinSignal::UPtr> m_cins;

    std::map<uint16_t, DPinSignal::UPtr> m_dpins;
};

template<typename T>
inline constexpr bool always_false_v = false;

template<typename SignalType>
const auto& SignalManager::getContainer() const
{
    if constexpr (std::is_base_of_v<DinSignal, SignalType>)
        return m_dins;
    else if constexpr (std::is_base_of_v<AinSignal, SignalType>)
        return m_ains;
    else if constexpr (std::is_base_of_v<CinSignal, SignalType>)
        return m_cins;
    else if constexpr (std::is_base_of_v<DoutSignal, SignalType>)
        return m_douts;
    else {
        static_assert(always_false_v<SignalType>, "Container not found");
    }
}

template<typename SignalType>
std::vector<SignalType*> SignalManager::getSignals(Signal::Subtypes mask) const
{
    const auto& container = getContainer<SignalType>();
    std::vector<SignalType*> result;
    result.reserve(container.size());
    for (const auto& [internalId, signal]: container) {
        if (signal->subtype() & mask) {
            if (auto* castedSignal = dynamic_cast<SignalType*>(signal.get())) {
                result.push_back(castedSignal);
            }
        }
    }

    return result;
}

template<typename SignalType>
SignalType* SignalManager::getSignal(uint16_t internalID) const
{
    const auto& container = getContainer<SignalType>();
    auto signalIt = container.find(internalID);
    if (signalIt != container.end())
        return dynamic_cast<SignalType*>(signalIt->second.get());

    return nullptr;
}

#endif // SIGNALMANAGER_H
