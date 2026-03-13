#pragma once

#include <memory>
#include <map>

#include <QString>

#include <db/dc_db_wrapper.h>

class DcSetting : public DcDbWrapper {

public:
    using UPtr = std::unique_ptr<DcSetting>;
    using UPtrMap = std::map<QString, UPtr>;

    DcSetting(const QString &name, const QString &value, DcController *device = nullptr);
    virtual ~DcSetting();

    QString name() const;
    QString value() const;
	bool updateValue(const QString & value);

    UPtr clone() const;
	
	void update();
	void insert();
	void remove();

private:
    QString m_name;
    QString m_value;
};
