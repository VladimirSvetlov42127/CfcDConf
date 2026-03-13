#pragma once
#include "dc_matrix_element.h"
#include <db/dc_db_wrapper.h>

class DcMatrixElementSignal : public DcMatrixElement, public DcDbWrapper
{
public:
    DcMatrixElementSignal(int32_t src, int32_t dst, const QString &properties, DcController *device = nullptr);

	void updatedb();

	DefMatrixElementType type(void) { return DEF_MATRIX_TYPE_SIGNAL; }

	void insert();
	void update();
	void remove();
};
