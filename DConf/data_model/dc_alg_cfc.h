#pragma once
#include <stdint.h>
#include "dc_alg.h"
#include <db/dc_db_wrapper.h>
#include "dc_pool.h"
#include "dc_alg_io_cfc.h"
#include <data_model/dc_properties.h>

class DcAlgCfc : public DcAlg, public DcDbWrapper {

public:
    DcAlgCfc(int32_t algid, int32_t position, const QString &name, const QString &properties, DcController *device = nullptr);
	~DcAlgCfc();

    void setDevice(DcController *device) override;

	DcPoolSingleKey<DcAlgIOCfc*> *ios();
    QString property(const QString& name) const;
    const DcProperties& properties() const;
	bool updateProperty(const QString &name, const QString &value);
    void updatedb() override;

private:
	DcPoolSingleKey<DcAlgIOCfc*> *m_ios; //inputs/outputs
	DcProperties m_properties;

public:
    void update() override;
    void insert() override;
    void remove() override;
};
