#pragma once

#include <qstring.h>

class DcController;

class DcDbWrapper {
public:
    DcDbWrapper(DcController* device = nullptr);
    virtual ~DcDbWrapper() = default;

    int32_t uid() const ;

    const DcController* device() const;
    virtual void setDevice(DcController* device);

	virtual void insert() = 0;
	virtual void update() = 0;
    virtual void remove() = 0;

private:
    DcController* m_device;
};


