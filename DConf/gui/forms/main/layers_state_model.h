#ifndef BASE_TAB_ERROR_MODEL_H
#define BASE_TAB_ERROR_MODEL_H

#include "gui/forms/main/base_tab_model.h"

class LayersStateModel : public BaseTabModel
{
public:
    LayersStateModel(uint16_t addr, uint16_t profile, const QString &columnValue, QObject *parent = nullptr, const DescriptionFunc &descriptionFunc = nullptr);
    virtual QVariant data(const QModelIndex &index, int role) const override;
};

#endif // BASE_TAB_ERROR_MODEL_H
