#ifndef __DC_BOARD_H__
#define __DC_BOARD_H__

#include <memory>
#include <vector>

#include <QString>

#include "db/dc_db_wrapper.h"
#include "data_model/parameters/parameter_registry.h"
#include "data_model/dc_properties.h"

class DcBoard : public DcDbWrapper
{
public:
    using UPtr = std::unique_ptr<DcBoard>;
    using UPtrVector = std::vector<UPtr>;

    DcBoard(int32_t id, int32_t slot, const QString &type, int32_t instance, const QString &properties, DcController *device = nullptr);
    virtual ~DcBoard();

    ParameterRegistry& paramsRegistry() { return _params; }
    const ParameterRegistry& paramsRegistry() const { return _params; }

    void setDevice(DcController* device) override;

    // Идентификатор устройства к которому принадлежит плата
    int32_t uid() const ;

    int32_t id() const { return _id; }
	int32_t slot() const { return _slot; }
	QString type() const { return _type; }
	int32_t inst() const { return _inst; }
    uint16_t DinsCount() const { return _dins_count; }
    uint16_t DoutsCount() const { return _douts_count; }
    uint16_t ainsOscCount() const { return _ains_osc; }
    bool ToBoard() const { return _slot > - 1; }
    QString properties() const;

    void insert() override;
    void update() override;
    void remove() override;

    static void updateStorage(int32_t uid);

private:
    ParameterRegistry _params;
    int32_t _id;
	int32_t _slot;
	QString _type;
	int32_t _inst;

    DcProperties m_properties;
	uint16_t _dins_count;	//	Количество дискретных входов
	uint16_t _douts_count;	//	Количество дискретных выходов
    uint16_t _ains_osc;     //  Количество физических аналогов которые осциллографируются всегда
};

#endif	//	__DC_BOARD_H__
