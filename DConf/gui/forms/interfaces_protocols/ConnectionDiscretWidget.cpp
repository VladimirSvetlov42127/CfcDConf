#include "ConnectionDiscretWidget.h"

#include <qgridlayout.h>
#include <qcombobox.h>
#include <qdebug.h>

#include <dpc/helper.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "data_model/dc_controller.h"

using namespace Dpc::Gui;

namespace {

constexpr const uint8_t INVALID_VALUE_BYTE = 0xFF;

} // namespace

ConnectionDiscretWidget::ConnectionDiscretWidget(DcController *controller, int interfaceIdx, int deviceIdx, const QString &deviceName, QWidget *parent) :
	QWidget(parent),
	m_controller(controller),
	m_interfaceIdx(interfaceIdx),
	m_deviceIdx(deviceIdx),
	m_deviceName(deviceName),
	m_comboBox(new QComboBox(this))
{
    m_connectionService = controller->serviceManager()->algManager().alg(SP_INDCON_WORDIN_PARAM);
	updateList();

	connect(m_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConnectionDiscretWidget::onComboBoxIndexChanged);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(m_comboBox);
	layout->setContentsMargins(0, 0, 0, 0);
}

ConnectionDiscretWidget::~ConnectionDiscretWidget()
{
}

void ConnectionDiscretWidget::updateList()
{
    if (!m_controller || !m_connectionService)
        return;

    auto subscriberParameter = m_controller->paramsRegistry().parameter(SP_INDCON_BYTE_PARAM);
    if (!subscriberParameter)
        return;

    auto sp = subscriberProfile(m_interfaceIdx, m_deviceIdx);
    VirtualInputSignal* currentVDin = nullptr;
    if (sp && sp.value() < m_connectionService->outputs().size())
        currentVDin = m_connectionService->outputs().at(sp.value())->target();

    // Очищаем и добавляем в комбобокс элемент нет.
    bool block = m_comboBox->blockSignals(true);
    m_comboBox->clear();
    m_comboBox->addItem("Не установлено", QVariant());

    for(auto vdin: m_controller->serviceManager()->vdins()) {
        if (vdin->source() && vdin != currentVDin)
            continue;

        m_comboBox->addItem(vdin->baseText(), Dpc::fromPtr(vdin));
        if (vdin == currentVDin)
            m_comboBox->setCurrentIndex(m_comboBox->count() - 1);
    }
    m_comboBox->blockSignals(block);
    m_previousComboBoxIdx = m_comboBox->currentIndex();
}

void ConnectionDiscretWidget::onComboBoxIndexChanged(int idx)
{
    auto sp = subscriberProfile(m_interfaceIdx, m_deviceIdx);
    if (!sp)
        sp = subscriberProfile(INVALID_VALUE_BYTE, INVALID_VALUE_BYTE);
    if (!sp) {
		MsgBox::error(QString("Не осталось слотов под дискрет связи(интерфейс = %1, устройство = %2)").arg(m_interfaceIdx).arg(m_deviceIdx));
		return;
    }

    auto vdin = Dpc::toPtr<VirtualInputSignal>(m_comboBox->itemData(idx));
    auto serviceOutput = m_connectionService->outputs().at(sp.value()).get();
    serviceOutput->setTarget(vdin);

    // Обновить текст текущего
    m_comboBox->setItemText(idx, vdin->text());

    // Обновить текст предыдущего
    auto prevVdin = Dpc::toPtr<VirtualInputSignal>(m_comboBox->itemData(m_previousComboBoxIdx));
    if (prevVdin)
        m_comboBox->setItemText(m_previousComboBoxIdx, prevVdin->text());
    m_previousComboBoxIdx = idx;

    auto subscriberParameter = m_controller->paramsRegistry().parameter(SP_INDCON_BYTE_PARAM);
    auto subscriberInterfaceParam = subscriberParameter->element(sp.value(), 0);
    auto subscriberDeviceParam = subscriberParameter->element(sp.value(), 1);

    subscriberInterfaceParam->updateValue(QString::number(vdin ? m_interfaceIdx : INVALID_VALUE_BYTE));
    subscriberDeviceParam->updateValue(QString::number(vdin ? m_deviceIdx : INVALID_VALUE_BYTE));
}

std::optional<uint8_t> ConnectionDiscretWidget::subscriberProfile(uint8_t interfaceIdx, uint8_t deviceIdx) const
{
    auto subscriberParam = getParam(SP_INDCON_BYTE_PARAM, 0);
	if (subscriberParam)
        for (uint8_t i = 0; i < subscriberParam->parameter()->profilesCount(); i++) {
            auto subscriberInterfaceParam = getParam(SP_INDCON_BYTE_PARAM, i * PROFILE_SIZE);
            auto subscriberDeviceParam = getParam(SP_INDCON_BYTE_PARAM, i * PROFILE_SIZE + 1);
			if (subscriberInterfaceParam->value().toUInt() == interfaceIdx && subscriberDeviceParam->value().toUInt() == deviceIdx)
				return i;
		}

    return {};
}

ParameterElement *ConnectionDiscretWidget::getParam(uint32_t addr, uint32_t idx, bool showError) const
{
    auto p = m_controller->paramsRegistry().element(addr, idx);
	if (!p && showError) {
		MsgBox::error(QString("Не удалось прочитать параметр(%1:%2): параметр не найден в шаблоне устройства")
			.arg(QString("0x%1").arg(QString("%1").arg(addr, 4, 16, QChar('0')).toUpper()))
			.arg(idx));
		return nullptr;
	}

	return p;
}
