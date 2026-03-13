#pragma once
#include <db/dc_db_wrapper.h>
#include <stdint.h>
#include <data_model/dc_properties.h>
#include <data_model/BindMatrixElem.h>



#define VIRTUAL_DEFAULT_NAME "Виртуальный вход"

typedef enum DefSignalDirection {
	DEF_SIG_DIRECTION_UNDEF=0,
	DEF_SIG_DIRECTION_INPUT=1,
	DEF_SIG_DIRECTION_OUTPUT=2
} DefSignalDirection;

typedef enum DefSignalType {
	DEF_SIG_TYPE_UNDEF = 0,
	DEF_SIG_TYPE_DISCRETE = 1,
	DEF_SIG_TYPE_ANALOG = 2,
	DEF_SIG_TYPE_COUNTER = 3
} DefSignalType;

typedef enum DefSignalSubType {
	DEF_SIG_SUBTYPE_UNDEF = 0,
	DEF_SIG_SUBTYPE_PHIS = 1,
	DEF_SIG_SUBTYPE_LOGIC = 2,
	DEF_SIG_SUBTYPE_VIRTUAL = 3,
	DEF_SIG_SUBTYPE_REMOTE = 4,
	DEF_SIG_SUBTYPE_LED_AD = 5,    // led output, входы на внутреннем АЦП
	DEF_SIG_SUBTYPE_VIRTUAL_ARCHIVE = 6,
	DEF_SIG_SUBTYPE_REMOTE_INTERNAL = 7
}DefSignalSubType;

class DcSignal : public DcDbWrapper, public BindMatrixElem {
public:
    DcSignal(int32_t id, int32_t internalid, DefSignalDirection direction, DefSignalType type,
             DefSignalSubType subtype, const QString &name, const QString &properties, DcController *device = nullptr);
	virtual ~DcSignal() = default;

	int32_t index();		// сквозной номер в пределах контроллера
	int32_t internalId(); // это по сути номер параметра по определённому адресу... допустим для дискретных входов это 0x0030.
	DefSignalDirection direction();
	DefSignalType type();
	DefSignalSubType subtype();
	QString name();
    QString property(QString name) const;
	bool updateProperty(const QString &name, const QString &value);
	bool updateName(const QString &newname);
	bool isFixed() const;
	bool isCloningEnabled() const;
    const DcProperties& properties() const;

private:
	int32_t m_id;
	int32_t m_internal_id;
	DefSignalDirection m_direction;
	DefSignalType m_type;
	DefSignalSubType m_subtype;
	QString m_name;
	DcProperties m_properties;
	
public:
	void update();
	void insert();
	void remove();
};
