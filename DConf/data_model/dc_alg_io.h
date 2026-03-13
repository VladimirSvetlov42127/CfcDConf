#pragma once
#include <stdint.h>
#include <qstring.h>

#include <data_model/BindMatrixElem.h>

typedef enum DefIoDirection {
	IO_DIRECTION_UNDEF=0,
	IO_DIRECTION_INPUT = 1,
	IO_DIRECTION_OUTPUT = 2
} DefIoDirection;

class DcAlgIO : public BindMatrixElem {
	
public:
	DcAlgIO(int32_t uid, int32_t alg_id, int32_t alg_pin, DefIoDirection direction, QString name) :
		m_io_uid(uid),
		m_alg_uid(alg_id),
		m_alg_pin(alg_pin),
		m_direction(direction)
	{
		m_name = name;
	}
	virtual ~DcAlgIO() = default;

	virtual void updateNamedb() = 0;

	bool updateName(QString newname) {
		if (newname.isNull())
			return false;
		if (newname.isEmpty())
			return false;
		if (name().compare(newname) == 0)
			return true;
		m_name = newname;
		updateNamedb();
		return true;
	}

	int32_t index() {	return m_io_uid;}
	int32_t alg() { return m_alg_uid; }
	int32_t pin() { return m_alg_pin; }
	DefIoDirection direction() { return m_direction; }
	QString name() { return m_name; }

private:
	int32_t m_io_uid; // input/output id uniq per system	// НЕверно! Номер сигналов в данной таблице, сквозной для всех алгоритмов
	int32_t m_alg_uid; // algorithm index
	int32_t m_alg_pin; // input/output number uniq per algorithm
	DefIoDirection m_direction; //input or output
	QString m_name; //io name

	
};