#pragma once
#include <stdint.h>
#include <qstring.h>

class DcAlg {
public:
	DcAlg(int32_t id, int32_t position, QString name);

public:
	int32_t index() const;
	int32_t position() const;
	QString name() const;
	bool updateName(QString newname);
	virtual void updatedb()=0;

private:
	int32_t m_id;
	int32_t m_position;
	QString m_name;
};