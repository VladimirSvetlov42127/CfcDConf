#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include "dpc/sybus/ParamPack.h"
#include <QStringList>

struct ParamInfo
{
    QString paramName;
    QStringList name;
    QString addrHex;
    uint8_t type;
    QString elementsCount;
    uint16_t lenthInBytes;
    QString attr;
    QList<Dpc::Sybus::ParamPackPtr> value;
    QList<uint16_t> index;
};

#endif // DATA_TRANSFER_H
