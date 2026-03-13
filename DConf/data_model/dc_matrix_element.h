#pragma once
#include <stdint.h>
#include <qstring.h>
#include <data_model/dc_properties.h>

typedef enum DefMatrixElementType {
	DEF_MATRIX_TYPE_UNDEF = 0,
	DEF_MATRIX_TYPE_ALG = 1,
	DEF_MATRIX_TYPE_ALG_CFC = 2,
	DEF_MATRIX_TYPE_SIGNAL = 3
} DefMatrixElementType;

class DcMatrixElement {
public:

	DcMatrixElement(int32_t src, int32_t dst, QString properties);

	int32_t src();
	int32_t dst();
	QString property(QString propName);

	bool updateProperty(QString name, QString value);
	virtual void updatedb()=0;
	virtual DefMatrixElementType type()=0;

private:
	int32_t m_src;
	int32_t m_dst;
	DcProperties m_properties;

};