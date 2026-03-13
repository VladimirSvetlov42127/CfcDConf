#ifndef SM_FLAGS_MODEL_H
#define SM_FLAGS_MODEL_H

#include "gui/forms/main/base_tab_model.h"

class SmFlagsModel : public BaseTabModel
{
public:
    SmFlagsModel(uint16_t addr, uint16_t profile, const QString &columnValue, QObject *parent = nullptr, const DescriptionFunc &descriptionFunc = nullptr);

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
};

#endif // SM_FLAGS_MODEL_H
