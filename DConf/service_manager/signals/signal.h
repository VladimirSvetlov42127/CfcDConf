#ifndef SIGNAL_H
#define SIGNAL_H

#include <QString>
#include <QDebug>

class Signal
{
public:
    enum DefSignalDirection : uint8_t {
        DEF_SIG_DIRECTION_UNDEF = 0,
        DEF_SIG_DIRECTION_INPUT = 1,
        DEF_SIG_DIRECTION_OUTPUT = 2
    };

    enum DefSignalType :uint8_t {
        DEF_SIG_TYPE_UNDEF = 0,
        DEF_SIG_TYPE_DISCRETE = 1,
        DEF_SIG_TYPE_ANALOG = 2,
        DEF_SIG_TYPE_COUNTER = 3
    };

    enum DefSignalSubType : uint8_t {
        DEF_SIG_SUBTYPE_UNDEF = 0,
        DEF_SIG_SUBTYPE_PHIS = 1,
        DEF_SIG_SUBTYPE_LOGIC = 2,
        DEF_SIG_SUBTYPE_VIRTUAL = 3,
        DEF_SIG_SUBTYPE_REMOTE = 4,
        DEF_SIG_SUBTYPE_LED_AD = 5,    // led output, входы на внутреннем АЦП
        DEF_SIG_SUBTYPE_VIRTUAL_ARCHIVE = 6,
        DEF_SIG_SUBTYPE_REMOTE_INTERNAL = 7
    };

    Signal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID = 0);
    virtual ~Signal() = default;

    uint8_t type() const;
    uint8_t subType() const;
    uint16_t subTypeID() const { return _syb_type_id; };

    virtual int32_t globalID() const;
    virtual uint16_t internalID() const;
    virtual QString name() const;
    virtual QString text() const;
    virtual QString fullText() const;

    template<typename Derived>
    const Derived* to() const { return dynamic_cast<const Derived*>(this); }

private:
    int32_t m_id;
    int32_t m_internal_id;
    uint8_t m_direction;
    uint8_t m_type;
    uint8_t m_subtype;
    QString m_name;
    uint16_t _syb_type_id;
};

#endif // SIGNAL_H
