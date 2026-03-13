#include <db/dc_db_wrapper.h>

#include "data_model/dc_controller.h"

DcDbWrapper::DcDbWrapper(DcController *device)
    : m_device{device}
{
}

int32_t DcDbWrapper::uid() const
{
    return device() ? device()->uid() : 0;
}

const DcController *DcDbWrapper::device() const
{
    return m_device;
}

void DcDbWrapper::setDevice(DcController *device)
{
    m_device = device;
}
