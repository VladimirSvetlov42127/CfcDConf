#include "restart_dialog.h"

#include <QDebug>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>

using namespace Dpc::Sybus;

RestartDialog::RestartDialog(bool hasT2compatibilityMode, QWidget *parent)
    : QDialog{parent}
    , m_restartModeComboBox{new QComboBox(this)}
    , m_workModeComboBox{new QComboBox(this)}
    , m_T2ModeComboBox{new QComboBox(this)}
    , m_workModeCheckBox{new QCheckBox("Режим работы", this)}
    , m_T2ModeCheckBox{new QCheckBox("Режим совместимости", this)}
{
    setWindowTitle("Перезагрузка устройства");

    m_restartModeComboBox->addItem("Нормальный", Channel::HardReset);
    m_restartModeComboBox->addItem("Минимальный", Channel::MinimalModeReset);

    m_workModeComboBox->addItem("Рабочий", Channel::Work);
    m_workModeComboBox->addItem("Подготовка", Channel::Prepare);
    m_workModeComboBox->setEnabled(false);
    connect(m_workModeCheckBox, &QCheckBox::toggled, m_workModeComboBox, &QComboBox::setEnabled);

    m_T2ModeComboBox->addItem("Деконт", Channel::Decont);
    m_T2ModeComboBox->addItem("Деконт T2", Channel::DecontT2);
    m_T2ModeComboBox->setEnabled(false);
    connect(m_T2ModeCheckBox, &QCheckBox::toggled, m_T2ModeComboBox, &QComboBox::setEnabled);
    m_T2ModeCheckBox->setVisible(hasT2compatibilityMode);
    m_T2ModeComboBox->setVisible(hasT2compatibilityMode);

    auto okButton = new QPushButton("Ок", this);
    connect(okButton, &QPushButton::clicked, this, [=] { accept(); });
    auto cancelButton = new QPushButton("Отмена", this);
    connect(cancelButton, &QPushButton::clicked, this, [=] { reject(); });

    auto layout = new QGridLayout(this);
    int row = 0;
    layout->addWidget(new QLabel("Режим перезагрузки"), row, 0);
    layout->addWidget(m_restartModeComboBox, row, 1, 1, 2);

    ++row;
    layout->addWidget(m_workModeCheckBox, row, 0);
    layout->addWidget(m_workModeComboBox, row, 1, 1, 2);

    ++row;
    layout->addWidget(m_T2ModeCheckBox, row, 0);
    layout->addWidget(m_T2ModeComboBox, row, 1, 1, 2);

    ++row;
    layout->setRowStretch(row, 1);

    ++row;
    layout->addWidget(okButton, row, 1);
    layout->addWidget(cancelButton, row, 2);
    layout->setColumnStretch(0, 1);
}

RestartOperation::RestartMode RestartDialog::mode() const
{
    auto mode = static_cast<Channel::ResetMode>(m_restartModeComboBox->currentData().toUInt());
    return mode;
}

RestartOperation::WorkMode RestartDialog::workMode() const
{
    if (!m_workModeComboBox->isEnabled())
        return RestartOperation::WorkMode();

    auto mode = static_cast<Channel::WorkingMode>(m_workModeComboBox->currentData().toUInt());
    return RestartOperation::WorkMode(mode);
}

RestartOperation::DecontMode RestartDialog::decontMode() const
{
    if (!m_T2ModeComboBox->isEnabled())
        return RestartOperation::DecontMode();

    auto mode = static_cast<Channel::DecontMode>(m_T2ModeComboBox->currentData().toUInt());
    return RestartOperation::DecontMode(mode);
}
