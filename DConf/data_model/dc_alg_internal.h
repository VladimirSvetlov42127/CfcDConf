#pragma once
#include "dc_alg.h"
#include <db/dc_db_wrapper.h>
#include "dc_pool.h"
#include "dc_alg_io_internal.h"
#include <data_model/dc_properties.h>

class DcAlgInternal : public DcAlg, public DcDbWrapper {

public:
    DcAlgInternal(int32_t algid, int32_t position, const QString &name, const QString &properties, DcController *device = nullptr);
	~DcAlgInternal();

    void setDevice(DcController *device) override;
	
	DcPoolSingleKey<DcAlgIOInternal*> *ios();

    QString property(const QString& name) const;
    const DcProperties& properties() const;

    void updatedb() override;

	bool operator==(const DcAlgInternal &other) const;

private:
	DcPoolSingleKey<DcAlgIOInternal*> *m_ios; //inputs/outputs
	DcProperties m_properties;

public:
    void update() override;
    void insert() override;
    void remove() override;
};
