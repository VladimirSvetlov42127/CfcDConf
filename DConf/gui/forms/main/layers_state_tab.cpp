#include "layers_state_tab.h"

#include <QGridLayout>

#include "device_operations/information_operation.h"
#include "gui/forms/main/layers_state_model.h"

using namespace Dpc::Gui;

namespace {

using FlagDescription = std::vector< std::pair<uint32_t, QString> >;
using Descriptions = std::unordered_map<uint16_t, FlagDescription>;

constexpr const char *COLUMN_VALUE = "Значение";

const FlagDescription fd_AllLayers {
    {0x10000000, "Повторяющаяся  фатальная ошибка"},
    {0x20000000, "Ошибка выделения ресурсов RTOS (создание очередей, семафоров)"},
    {0x40000000, "Недостаточно памяти"},
    {0x80000000, "Ошибка при инициализации слоя (например-задача не создана)"}
};

const FlagDescription fd_MainSystemService {
    {0x00000001, "Ошибка создания сервиса прикладных таймеров"},
    {0x00000002, "Ошибка создания сервиса светодиодной индикации"},
    {0x00000004, "Ошибка наследования параметров при обновлениии"},
    {0x00000008, "Ошибка чтения прикладной конфигурации"},
    {0x00000010, "Ошибка чтения производственной  конфигурации"},
    {0x00000020, "Ошибка чтения пользовательских данных"},
    {0x00000040, "Ошибка чтения блока динамических данных"},
    {0x00000080, "Потеряны данные счетчиков ЭЭ"},
    {0x00000100, "Ошибка системного файла"},
    {0x00000200, "Наложение областей eMMC"},
    {0x00000400, "Несовпадение контрольной суммы MD5 внутреннего ПО"}
};

const FlagDescription fd_SyncTime {
    {0x00000001, "Аппаратная ошибка PTP таймера"}
};

const FlagDescription fd_MaintainceSystemDrive {
    {0x00000001, "Ошибка обмена с системным накопителем"}
};

const FlagDescription fd_ServiceEMMC {
    {0x00000001, "Ошибка инициализации eMMC"},
    {0x00000002, "Ошибка удаления файла в файловой системе"},
    {0x00000004, "Ошибка создания директории"},
    {0x00000008, "Ошибка сектора- необходимо форматировать том"}
};

const FlagDescription fd_FyleSystem {
    {0x00000001, "Ошибка удаления файла"},
    {0x00000002, "Ошибка создания директории"},
    {0x00000004, "Ошибка сектора- необходимо форматировать том"}
};

const FlagDescription fd_EventLog {
    {0x00000001, "Потеряна информация о записях в ОЗУпод батарейкой"},
    {0x00000002, "Несовпадает размерность массива предопределенных строк"}
};

const FlagDescription fd_ServiceOscil {
    {0x00000001, "Устройство не поддерживает осциллогафирование, а функция вызывается"}
};

const FlagDescription fd_ProtocolTcpIp {
    {0x00000001, "Аппаратная ошибка Ethernet"},
    {0x00000002, "Ошибка работы с сокетом"}
};

const FlagDescription fd_ProtocolIEC61850 {
    {0x00000001, "Ошибка значения режима работы логического узла для GOOSE"},
    {0x00000002, "Ошибка в наборе данных исходящего GOOSE"},
    {0x00000004, "Ошибка файла конфигурации MMS"},
    {0x00000008, "Длина пакета исходящего GOOSE сообщения больше допустимой"},
    {0x00000010, "Ошибка в блоке управления GOOSE сообщений"},
    {0x00000020, "Ошибка описании блока управления входящих GOOSE"},
    {0x00000040, "Ошибка в наборе данных входящих GOOSE"}
};

const FlagDescription fd_InputDiscret {
    {0x00000001, "Ошибка описания двухпозиционных входов"},
    {0x00000002, "Ошибка описания счетчика переходов состояний дискретных входов"},
    {0x00000004, "Ошибка значения окна измерений на АЦП для обработки состояний дискретных входов"},
    {0x00000008, "Ошибка номера базового канала в паре разностных"},
    {0x00000010, "Неверное значение параметра обработки дребезга"}
};

const FlagDescription fd_OutputDiscret {
    {0x00000001, "Ошибка описания двухпозиционных выходов"},
    {0x00000002, "Ошибка в функции виртуального выходного дискрета"},
    {0x00000004, "Ошибка значения номера канала управления"},
    {0x00000008, "Ошибка обмена с платой управления на SPI"},
    {0x00000010, "Неверное значение параметра выходного дискрета"}
};

const FlagDescription fd_ProtocolUsart {
    {0x00000100, "Неверный формат символа"},
    {0x00000200, "Неверный код протокола"},
    {0x00000400, "Неверный адрес"},
    {0x00000800, "Ошибка семафора"},
    {0x00001000, "Ошибка описания слейва"},
    {0x00002000, "Превышение количества внешних переменных"},
    {0x00004000, "Превышение конфигурации системной шины"}
};

const FlagDescription fd_ProtocolIEC60870_5 {
    {0x00000001, "Неверное значение kAPDU"},
    {0x00000002, "Неверное значение wAPDU"}
};

const FlagDescription fd_ProtocolModbus {
    {0x00000001, "Неверный номер порта шлюза RS485 для Modbus/TCP"},
    {0x00000002, "Ошибка конфигурации запроса Modbus мастера на чтение счетчиков"}
};

const FlagDescription fd_ProtocolSpodes {
    {0x00000001, "Ошибка конфигурации запроса мастера на чтение аналогов"},
    {0x00000002, "Ошибка конфигурации запроса мастера на чтение дискрктов"},
    {0x00000004, "Ошибка конфигурации запроса мастера на чтение счетчиков"},
    {0x00000008, "Ошибка конфигурации запроса мастера на управление выходами"},
    {0x00000010, "Ошибка конфигурации описания синхронизации времени"}
};

const FlagDescription fd_DigitalSignalService {
    {0x00000001, "Несовпадение типа платы аналоговых измерений в позиции крейта"},
    {0x00000002, "Ошибка обмена по SPI на объединительной плате"}
};

const FlagDescription fd_AsincEventTracking {
    {0x00000001, "Ошибка номера интерфейса для виртуального дискрета связи"},
    {0x00000002, "Ошибка номера абонента для виртуального дискрета связи"},
    {0x00000004, "Ошибка номера  виртуального дискрета связи"}
};

const FlagDescription fd_CommunicationBackupService {
    {0x00000001, "Ошибка обмена с FPGA"}
};

const FlagDescription fd_RelayProtectingEmbeddedAlgorithms {
    {0x00000001, "Недопустимый номер дискрета отображающего состояние алгоритма АВР-ВНР"},
    {0x00000002, "Недопустимый номер дискрета отображающего запрет заботы АВР"},
    {0x00000004, "Недопустимый номер дискрета управления СВН"},
    {0x00000008, "Недопустимый номер дискрета управления ШВН-А"},
    {0x00000010, "Недопустимый номер дискрета управления ШВН-Б"},
    {0x00000020, "Недопустимый номер дискрета положения ШВН-А"},
    {0x00000040, "Недопустимый номер дискрета положения ШВН-Б"},
    {0x00000080, "Недопустимый номер дискрета состояния АВР"},
    {0x00000100, "Недопустимый номер дискрета положения СВН"},
    {0x00000200, "Недопустимый номер дискрета наличия напряжения Ua секции А"},
    {0x00000400, "Недопустимый номер дискрета наличия напряжения Ub секции А"},
    {0x00000800, "Недопустимый номер дискрета наличия напряжения Uc секции А"},
    {0x00001000, "Недопустимый номер дискрета наличия напряжения Ua секции Б"},
    {0x00002000, "Недопустимый номер дискрета наличия напряжения Ub секции Б"},
    {0x00004000, "Недопустимый номер дискрета наличия напряжения Uc секции Б"},
    {0x00008000, "Недопустимый номер дискрета наличия напряжения оперативных цепей"},
    {0x00010000, "Нет привязки/контроля напряжения присоединения А"},
    {0x00020000, "Нет привязки/контроля напряжения присоединения В"},
    {0x00040000, "Привязки контроля напряжений присоединений А и Б пересекаются"},
    {0x00080000, "Привязка управления ШВН-А и ШВН-Б совпадает"},
    {0x00100000, "Привязка управления ШВН-А или ШВН-Б совпадает с управлением СВН"},
    {0x00200000, "Привязки контроля состояний ШВН-А и ШВН-Б совпадают"},
    {0x00400000, "Недопустимый номер дискрета управления оперативным питанием"},
    {0x01000001, QString()},
    {0x01000002, QString()},
    {0x01000004, QString()},
    {0x02000001, "Ошибка кода типа сравнения аналога с порогом"},
    {0x02000002, "Ошибка величины гистерезиса (отрицательный) для сравнения аналога с порогом"},
    {0x02000004, "Ошибка времени сравнения (отрицательное) для сравнения аналога с порогом"},
    {0x02000008, "Ошибка номера дискрета для сравнения аналога с порогом"},
    {0x03000001, "Назначенная схема включения напряжений не поддерживается"},
    {0x03000002, "Назначенная схема включения токов не поддерживается"},
    {0x03000004, "Ошибка описания выключателя"},
    {0x03000008, "Ошибка конфигурации АВР"},
    {0x03000010, "Ошибка конфигурации ЗМН1"},
    {0x03000020, "Ошибка конфигурации ЗМН-МКС"},
    {0x03000040, "Ошибка конфигурации ЛЗШ"},
    {0x03000080, "Ошибка конфигурации ускоренной МТЗ"},
    {0x03000100, "Ошибка конфигурации ДТО"},
    {0x03000200, "Ошибка конфигурации ДЗТ"},
    {0x03000400, "Ошибка конфигурации УРОВ"},
    {0x03000800, "Ошибка конфигурации ТО ВН"},
    {0x03001000, "Ошибка сервиса переключения профиля уставок"},
    {0x03002000, "Ошибка конфигурации БКИ"},
    {0x04000001, "Ошибка конфигурациипреобразования аналогов"},
    {0x04000002, "Ошибка файла конфигурации преобразования аналогов"},
    {0x04000004, "Ошибка выделения памяти для конфигурации преобразования аналогов"}
};

const Descriptions rowFlagDescriptions {
    {0, fd_MainSystemService},
    {2, fd_SyncTime},
    {4, fd_MaintainceSystemDrive},
    {5, fd_ServiceEMMC},
    {7, fd_FyleSystem},
    {8, fd_InputDiscret},
    {11, fd_OutputDiscret},
    {12, fd_DigitalSignalService},
    {13, fd_RelayProtectingEmbeddedAlgorithms},
    {14, fd_EventLog},
    {15, fd_ServiceOscil},
    {17, fd_CommunicationBackupService},
    {18, fd_ProtocolUsart},
    {20, fd_ProtocolIEC60870_5},
    {21, fd_ProtocolModbus},
    {22, fd_ProtocolTcpIp},
    {23, fd_ProtocolIEC61850},
    {25, fd_AsincEventTracking},
    {27, fd_ProtocolSpodes}
};

QString stateDescription(uint16_t position, uint32_t state)
{
    QStringList descriptions;

    for (const auto &bitDesc : fd_AllLayers) {
        if (state & bitDesc.first) {
            descriptions << bitDesc.second;
        }
    }

    const auto &it = rowFlagDescriptions.find(position);
    if (it != rowFlagDescriptions.end()) {
        for (const auto &bitDesc : it->second) {
            if (state & bitDesc.first) {
                descriptions << bitDesc.second;
            }
        }
    }

    if(descriptions.empty())
        return QString();

    return descriptions.join("\n");
}

} // namespace

LayersStateTab::LayersStateTab(QWidget *parent)
    : AbstractDeviceTab(parent)
{
    m_layersStateModel = new LayersStateModel(SP_APP_ENTITY_STATUS, 0, COLUMN_VALUE, this, ::stateDescription);
    m_layersStateView = new TableView(m_layersStateModel, this);

    QGridLayout *tabLayout = new QGridLayout(this);
    tabLayout->addWidget(m_layersStateView);
}

void LayersStateTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        m_layersStateModel->updateInfo(info->params());
    }

    m_layersStateView->verticalHeader()->setMinimumSectionSize(30);
    m_layersStateView->resizeColumnsToContents();
    m_layersStateView->resizeRowsToContents();
    m_layersStateView->horizontalHeader()->resizeSection(LayersStateModel::Value, 100);
}

QList<uint16_t> LayersStateTab::addrList() const
{
    const QList<uint16_t> requiredParams = {SP_APP_ENTITY_STATUS};
    return requiredParams;
}

bool LayersStateTab::hasError() const
{
    return m_layersStateModel->hasError();
}
