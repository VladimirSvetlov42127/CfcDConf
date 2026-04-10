#ifndef SIGNAL_H
#define SIGNAL_H

#include <QString>
#include <QFlags>

#include "data_model/dc_properties.h"

class DcController;
class ParameterElement;

class Signal
{
public:    
    enum class Type : uint8_t {
        None,
        Din,
        Ain,
        Cin,
        Dout
    };

    enum Subtype : uint16_t {
        None = 0x0,
        Undef = 0x1,
        // Acp на устройстве входит в состав Физических. Обладает свойствами физических сигналов.
        Acp = 0x2,
        // Led формальная подкатегория "Физических выходов", которая введена на уровне DConf. Никак не обозначена в устройстве.
        Led = 0x4,
        Physical = 0x8,
        PhysicalIn = Acp | Physical,
        PhysicalOut = Led | Physical,
        Logical = 0x10,
        Virtual = 0x20,
        Remote = 0x40,
        // ArchiveVirtual На устройстве входит в состав виртуальных в количественном подсчёте, но не обладает свойствами виртуальных сигналов.
        ArchiveVirtual = 0x80,
        RemoteInternal = 0x100,
        Any = 0xFFFF
    };
    Q_DECLARE_FLAGS(Subtypes, Subtype)

    struct Config {
        QString name;
        uint32_t globalID;
        uint16_t internalID;
        uint16_t subtypeID;
        QString properties;
    };

    Signal(uint32_t globalID, uint16_t internalID, uint16_t subtypeID, const QString &name, const QString &properties);
    Signal(const Signal::Config &config);
    virtual ~Signal() = default;

    // Глобальный сквозной ID сигнала, в рамках всего устройства.
    // Начинается с 1. Нужен для обратной совместимости в механизмах привязки
    uint32_t globalID() const;

    // Индекс сигнала в рамках типа сигнала (Дискретный вход, аналог, счётчик, дискретный выход)
    // Начинается с 0. Соответсвует индексу в устройстве.
    uint16_t internalID() const;

    // Индекс сигнала в рамках подтипа(Физический, логический, виртуальный и т.д.) типа(вход, аналог, счётчик, выход) сигнала.
    // Начинается с 0. Соответсвует индексу в устройстве.
    uint16_t subtypeID() const;

    virtual Signal::Type type() const = 0;
    virtual Signal::Subtype subtype() const = 0;

    virtual QString name() const;
    virtual QString text() const;
    virtual QString fullText() const;
    virtual bool init(DcController *config);

    template<typename Derived>
    const Derived* to() const { return dynamic_cast<const Derived*>(this); }

    template<typename Derived>
    Derived* to() { return dynamic_cast<Derived*>(this); }

    DcProperties& properties() { return m_properties; }
    const DcProperties& properties() const { return m_properties; }       

private:    
    uint32_t m_globalID;
    uint16_t m_internalID;
    uint16_t m_subtypeID;
    QString m_name;
    DcProperties m_properties;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Signal::Subtypes)

#endif // SIGNAL_H
