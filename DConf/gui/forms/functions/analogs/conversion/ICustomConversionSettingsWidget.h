#pragma once

#include <qwidget.h>

class ICustomConversionSettingsWidget : public QWidget
{
public:
    explicit ICustomConversionSettingsWidget(QWidget* parent = 0) : QWidget(parent) {}

    virtual uint16_t itemsCount() const = 0;
    virtual QByteArray settingsData() const = 0;
};

