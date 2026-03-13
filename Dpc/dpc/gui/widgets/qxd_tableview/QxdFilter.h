#ifndef QXDFILTER_H
#define QXDFILTER_H

#include <QVariant>

#include <dpc/dpc_global.h>

namespace Qxd {

enum FilterType {
    None = 0,
    List,
    Numeric,
    Time,
    Date,
    DateTime,
    Text,

    User
};

enum CompareOperator {
    Unknown = 0,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual
};

enum Combination {
    AND,
    OR
};

inline bool compare(Qxd::CompareOperator op, const QVariant & val1, const QVariant & val2)
{
    if (op == Qxd::Unknown)
        return true;
    if (op == Qxd::Equal) {
        if (val1.isNull() && val2.isNull())
            return true;
        return val1 == val2;
    }
    if (op == Qxd::NotEqual) {
        if (val1.isNull() && val2.isNull())
            return false;
        return val1 != val2;
    }
    if (val1.isNull())
        return false;
    if (val2.isNull())
        return true;

    switch (op) {
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    case Qxd::Less: return (QVariant::compare(val1, val2) < 0);
    case Qxd::LessEqual: return (QVariant::compare(val1, val2) <= 0);
    case Qxd::Greater: return (QVariant::compare(val1, val2) > 0);
    case Qxd::GreaterEqual: return (QVariant::compare(val1, val2) >= 0);
#else
    case Qxd::Less: return val1 < val2;
    case Qxd::LessEqual: return val1 <= val2;
    case Qxd::Greater: return val1 > val2;
    case Qxd::GreaterEqual: return val1 >= val2;
#endif
    default: return false;
    }
}

} // Qxd

class DPC_EXPORT QxdIFilter
{
public:
    QxdIFilter();
    virtual ~QxdIFilter();

	virtual int type() const = 0;
    virtual bool accept(const QModelIndex &index, int filterRole) const = 0;    
    virtual void reset() {}

    void setEqualTo(const QList<QVariant> &list);

    void setEnabled(bool enabled);
    bool isEnabled() const;

protected:
    virtual void onSetEqualTo(const QList<QVariant> &list) = 0;

private:
	bool m_enabled;
};

#endif // QXDFILTER_H
