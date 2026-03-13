#include "date_time_tab.h"

#include <QFormLayout>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTimeZone>
#include <QLineEdit>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <dpc/gui/gui.h>

#include "data_model/dc_controller.h"
#include "device_operations/information_operation.h"
#include "device_operations/datetime_operation.h"

namespace {

const std::array g_TimeSyncProtocol = {"IEEE1588v2 UDP", "IEEE1588v2 802.3", "SNTPv4", "1PPS", "SYBUS/RTU",
                                                 "МЭК 60870-5-101/104 via GSM", "МЭК 60870-5-104", "SYBUS/TCP", "MODBUS/RTU", "MODBUS/TCP",
                                                 "SNTP/GPRS", "МЭК 60870-5-103", "GSM сеть"};

const std::array g_TypeSync = {"Нет", "Локальный", "Глобальный"};

QString toString(const QVariant &offset)
{
    if (offset.isNull())
        return QString();

    auto value = offset.toInt();
    return value > 0 ? QString("+%1").arg(value) : QString::number(value);
}

} // namespace

DateTimeTab::DateTimeTab(DcController *device, QWidget *parent)
    : AbstractDeviceTab(parent)
    , m_dateEdit(new QDateEdit)
    , m_timeEdit(new QTimeEdit)
    , m_offsetComboBox(new QComboBox)
    , m_protocolSync(new QLineEdit)
    , m_typeSync(new QLineEdit)
    , m_device(device)
{
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("dd.MM.yyyy");
    m_dateEdit->setDate(QDate());

    m_timeEdit->setTime(QTime());
    m_timeEdit->setCalendarPopup(true);
    m_timeEdit->setDisplayFormat("hh:mm:ss");

    for (int i = -12; i <= 14; i++)
        m_offsetComboBox->addItem(::toString(i), i);
    m_offsetComboBox->setCurrentIndex(-1);

    auto setDTButton = new QPushButton("Установить заданное время");
    connect(setDTButton, &QPushButton::clicked, this, [=]() {
        setDateTime(QDateTime(m_dateEdit->date(), m_timeEdit->time(), Qt::UTC), m_offsetComboBox->currentData());
    });

    auto setSysDTButton = new QPushButton("Установить системное время");
    connect(setSysDTButton, &QPushButton::clicked, this, [=]() {
        auto dt = QDateTime::currentDateTime();
        setDateTime(QDateTime(dt.date(), dt.time(), Qt::UTC), dt.offsetFromUtc() / 3600);
    });

    QFormLayout *layout = new QFormLayout(this);
    m_dateEdit->setMaximumWidth(100);
    m_timeEdit->setMaximumWidth(100);
    m_offsetComboBox->setMaximumWidth(100);
    m_protocolSync->setMaximumWidth(100);
    m_protocolSync->setReadOnly(true);
    m_typeSync->setMaximumWidth(100);
    m_typeSync->setReadOnly(true);
    setDTButton->setMaximumWidth(235);
    setSysDTButton->setMaximumWidth(235);

    layout->addRow("Дата", m_dateEdit);
    layout->addRow("Время", m_timeEdit);
    layout->addRow("Часовой пояс", m_offsetComboBox);
    layout->addRow("Протокол синхронизации", m_protocolSync);
    layout->addRow("Тип синхронизации", m_typeSync);
    layout->addRow(setDTButton);
    layout->addRow(setSysDTButton);
}

DateTimeTab::~DateTimeTab()
{
}

void DateTimeTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        auto dt = info->dateTime();
        if (dt.isValid()) {
            m_dateEdit->setDate(dt.date());
            m_timeEdit->setTime(dt.time());
        }

        auto offset = info->offset();
        m_offsetComboBox->setEnabled(offset.isValid());
        m_offsetComboBox->setCurrentIndex(-1);
        m_offsetComboBox->setToolTip(QString());
        if (!offset.isValid()) {
            m_offsetComboBox->setToolTip("Не поддерживается протоколом синхронизации");
            return;
        }

        for (int i = 0; i < m_offsetComboBox->count(); i++) {
            if (m_offsetComboBox->itemData(i).toInt() == offset.toInt()) {
                m_offsetComboBox->setCurrentIndex(i);
                break;
            }
        }

        const auto &container = info->params();
        const auto &protocolSyncIt = container.find(SP_PROTOCOL);
        if (protocolSyncIt != container.end()) {
            if (!protocolSyncIt->second.empty()) {
                const auto &protocolSyncVecIt = protocolSyncIt->second.at(0).find(0);
                if (protocolSyncVecIt != protocolSyncIt->second.at(0).end()) {
                    const auto &timeSyncParam = protocolSyncVecIt->second.value;
                    if (timeSyncParam.toUInt() == 255) {
                        m_protocolSync->setText("Не поддерживается");
                    }
                    else if (timeSyncParam.toUInt() < g_TimeSyncProtocol.size()) {
                        auto protocolSync = g_TimeSyncProtocol.at(timeSyncParam.toUInt());
                        m_protocolSync->setText(protocolSync);
                    }
                }
            }
        }

        const auto &typeSyncIt = container.find(SP_DIAGPARAMS8);
        if (typeSyncIt != container.end()) {
            if (!typeSyncIt->second.empty()) {
                const auto &typeSyncVecIt = typeSyncIt->second.at(0).find(0);
                if (typeSyncVecIt != typeSyncIt->second.at(0).end()) {
                    const auto &typeSync = typeSyncVecIt->second.value;
                    if (typeSync.toUInt() < g_TypeSync.size()) {
                        auto typeSyncText = g_TypeSync.at(typeSync.toUInt());
                        m_typeSync->setText(typeSyncText);
                    }
                }
            }
        }
    }

}

QList<uint16_t> DateTimeTab::addrList() const
{
    QList<uint16_t> requiredParams = {SP_PROTOCOL, SP_DIAGPARAMS8};
    return requiredParams;
}

void DateTimeTab::setDateTime(const QDateTime& dt, const QVariant& offset)
{
    emit operationRequest(std::make_shared<DatetimeOperation>(dt, offset));
}
