#pragma once
#include <data_model/dc_alg_io.h>
#include <db/dc_db_wrapper.h>

class DcAlgIOInternal : public DcAlgIO, public DcDbWrapper
{
public:
    DcAlgIOInternal(int32_t uid, int32_t alg_id, int32_t alg_pin, DefIoDirection direction, const QString &name, DcController *device = nullptr);
	void updateNamedb(void);
public:
	void update();
	void insert();
	void remove();

};
