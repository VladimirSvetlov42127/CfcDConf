#ifndef HELPER_H
#define HELPER_H

#include <QVariant>

#include <dpc/dpc_global.h>

namespace Dpc {

template<typename T>
QVariant fromPtr(T* ptr) noexcept {
    if (!ptr)
        return QVariant();

    return QVariant::fromValue(static_cast<void*>(ptr));
}

template<typename T>
T* toPtr(const QVariant &var) noexcept {
    if (!var.isValid() || var.userType() != QMetaType::VoidStar)
        return nullptr;

    return static_cast<T*>(var.value<void*>());
}

} // Dpc

#endif // HELPER_H
