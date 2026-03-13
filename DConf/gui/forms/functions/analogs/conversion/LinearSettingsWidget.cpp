#include "LinearSettingsWidget.h"

#include <qdebug.h>
#include <qlabel.h>
#include <qlayout.h>

using namespace Dpc::Gui;

namespace {
#pragma pack(push, 1)
    struct LinearCnv
    {
        LinearCnv(const QByteArray& buff = QByteArray()) {
            if (!buff.isEmpty())
                memcpy(this, buff.data(), sizeof(LinearCnv));
        }

        float k0 = 0.f;
        float k1 = 0.f;

        QByteArray data() const { return QByteArray((const char*)this, sizeof(LinearCnv)); }
    };
#pragma pack(pop)      
}

LinearSettingsWidget::LinearSettingsWidget(const QByteArray&data, QWidget *parent) :
    ICustomConversionSettingsWidget(parent),
    m_k0SpinBox(new DoubleSpinBox),
    m_k1SpinBox(new DoubleSpinBox)
{
    auto max = std::numeric_limits<float>::max();
    auto min = -max;
    auto decimals = 3;   
    
    m_k0SpinBox->setRange(min, max);
    m_k0SpinBox->setDecimals(decimals);
   
    m_k1SpinBox->setRange(min, max);
    m_k1SpinBox->setDecimals(decimals);
    
    LinearCnv st(data);
    m_k0SpinBox->setValue(st.k0);
    m_k1SpinBox->setValue(st.k1);

    auto font = this->font();
    font.setItalic(true);
    font.setPointSize(12);

    auto formulaLabel = new QLabel("F(x) = K1*(X + K0)", this);
    formulaLabel->setFont(font);
    formulaLabel->setStyleSheet("QLabel { color: blue;}");

    auto layout = new QGridLayout(this);
    int row = 0;
    layout->addWidget(new QLabel("K0"), row, 0);
    layout->addWidget(m_k0SpinBox, row, 1);

    layout->addWidget(formulaLabel, row, 2, 2, 1, Qt::AlignCenter);
    row++;

    layout->addWidget(new QLabel("K1"), row, 0);
    layout->addWidget(m_k1SpinBox, row, 1);
    row++;

    layout->setRowStretch(row, 1);
    layout->setColumnStretch(2, 1);
}

uint16_t LinearSettingsWidget::itemsCount() const
{
    return 2;
}

QByteArray LinearSettingsWidget::settingsData() const
{
    LinearCnv st;
    st.k0 = m_k0SpinBox->value();
    st.k1 = m_k1SpinBox->value();

    return st.data();
}
