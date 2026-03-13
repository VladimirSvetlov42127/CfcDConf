#include "CustomConversionDialog.h"

#include <qdebug.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qdir.h>

#include <dpc/sybus/utils.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

#include <gui/forms/functions/analogs/conversion/LinearSettingsWidget.h>
#include <gui/forms/functions/analogs/conversion/LinearPiecewiseSettingsWidget.h>

using namespace Dpc::Gui;

namespace {
    namespace Text 
    {
        const QString ConversionType = "Тип преобразования";
        const QString InputLowLimit = "Нижний предел входной величины";
        const QString InputHighimit = "Верхний предел входной величины";
    }

    const uint16_t FILE_ID = 0x4567;
    const QString CONVERSION_FILENAME = "Aconv";

    enum {
        ConversionTypeRole = Qt::UserRole,
        SettingsDataRole
    };
    
    enum CustomConversionType
    {
        LinearConvers = 1,
        LinearPiecewiseConvers = 2
    };

    ComboBoxDelegate g_ConversionTypes = { {"Линейная", LinearConvers }, {"Кусочно-линейная интерполяция", LinearPiecewiseConvers } };

#pragma pack(push, 1)
    struct FileHeader 
    {
        FileHeader(const QByteArray& definition = QByteArray()) : len(definition.size())
        {
            if (!definition.isEmpty())
                crc = Dpc::Sybus::crc16((uint8_t*)definition.data(), definition.size(), 0);
        }
        uint16_t defineId = FILE_ID;
        uint32_t len;
        uint16_t crc  = 0;

        QByteArray data() const { return QByteArray((const char*)this, sizeof(FileHeader)); }
    };

    struct ConversionDefinition
    {
        ConversionDefinition(const QByteArray& buff = QByteArray())
        {
            if (!buff.isEmpty())
                memcpy(this, buff.data(), sizeof(ConversionDefinition));
        }

        uint16_t conversionType  = LinearConvers;
        uint16_t itemsCount      = 0;    
        float	 input_LowLimit  = 0.f; 
        float    input_HighLimit = 0.f; 
        uint32_t options         = 0; 
        uint32_t dataPointer     = sizeof(ConversionDefinition);

        QByteArray data() const { return QByteArray((const char*)this, sizeof(ConversionDefinition)); }
    };
#pragma pack(pop)
    
    QWidget* createSettingsWidget(int type, const QByteArray& data)
    {
        switch (type)
        {
        case LinearConvers: return new LinearSettingsWidget(data);
        case LinearPiecewiseConvers: return new LinearPiecewiseSettingsWidget(data);
        default: return new QWidget;
        }
    }
}

CustomConversionDialog::CustomConversionDialog(DcController* device, uint16_t conversionIdx, QWidget *parent) :
    QDialog(parent),
    m_device(device), 
    m_idx(conversionIdx),
    m_conversionTypeComboBox(new ComboBox),
    m_inputLowSpinBox(new DoubleSpinBox),
    m_inputHighSpinBox(new DoubleSpinBox),
    m_fm(device)
{
    setWindowTitle(QString("Пользовательское преобразование №%1").arg(conversionIdx + 1));

    for (auto&& it : g_ConversionTypes.items()) {
        m_conversionTypeComboBox->addItem(it.text, it.value);
    }

    auto settingsGroupBox = new QGroupBox("Параметры");
    auto settingsGroupBoxLayout = new QGridLayout(settingsGroupBox);
    settingsGroupBoxLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    {
        m_currentSettingsWidget = createSettingsWidget(m_conversionTypeComboBox->currentData().toUInt(), QByteArray());
        settingsGroupBoxLayout->addWidget(m_currentSettingsWidget, 0, 0);

        auto onCurrentIndexChanged = [=]() mutable {
            auto type = m_conversionTypeComboBox->currentData().toUInt();
            auto settingsData = m_conversionTypeComboBox->currentData(SettingsDataRole).toByteArray();
            auto newSettingsWidget = createSettingsWidget(type, settingsData);
            settingsGroupBoxLayout->replaceWidget(m_currentSettingsWidget, newSettingsWidget);
            delete m_currentSettingsWidget;
            m_currentSettingsWidget = newSettingsWidget;
        };

        connect(m_conversionTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, onCurrentIndexChanged);
    }

    auto max = std::numeric_limits<float>::max();
    auto min = -max;
    m_inputLowSpinBox->setRange(min, max);
    m_inputHighSpinBox->setRange(min, max);

    auto okButton = new QPushButton("Ok");
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &CustomConversionDialog::onOkButton);

    auto cancelButton = new QPushButton("Отмена");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    auto buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    int row = 0;
    auto layout = new QGridLayout(this);    
    layout->addWidget(new QLabel(Text::ConversionType), row, 0);
    layout->addWidget(m_conversionTypeComboBox, row, 1);
    row++;

    layout->addWidget(new QLabel(Text::InputLowLimit), row, 0);
    layout->addWidget(m_inputLowSpinBox, row, 1);
    row++;

    layout->addWidget(new QLabel(Text::InputHighimit), row, 0);
    layout->addWidget(m_inputHighSpinBox, row, 1);
    row++;

    layout->addWidget(settingsGroupBox, row, 0, 2, 2);
    row++;
    layout->setRowStretch(row, 1);
    row++;

    layout->addLayout(buttonsLayout, row, 0, 1, 2);


    load();
}

void CustomConversionDialog::onOkButton()
{
    if (save())
        accept();
}

void CustomConversionDialog::load()
{
    auto data = dataFromFile();
    ConversionDefinition def(data);
    data.remove(0, sizeof(ConversionDefinition));

    m_inputLowSpinBox->setValue(def.input_LowLimit);
    m_inputHighSpinBox->setValue(def.input_HighLimit);
    m_conversionTypeComboBox->setCurrentIndex(-1);
    for (size_t i = 0; i < m_conversionTypeComboBox->count(); i++)
        if (m_conversionTypeComboBox->itemData(i).toUInt() == def.conversionType) {
            m_conversionTypeComboBox->setItemData(i, data, SettingsDataRole);
            m_conversionTypeComboBox->setCurrentIndex(i);
        }
}

bool CustomConversionDialog::save()
{
    QString dir = m_fm.localPath();
    if (!QDir(dir).mkpath(".")) {
        MsgBox::error(QString("Не удалось создать папку %1").arg(dir));
        return false;
    }

    QString fileName = QString("%1_%2.dat").arg(CONVERSION_FILENAME).arg(m_idx);
    QString localFilePath = m_fm.localPath(fileName);
    QFile file(localFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        MsgBox::error(QString("Не удалось открыть файл %1: %2").arg(localFilePath).arg(file.errorString()));
        return false;
    }

    uint16_t itemsCount = 0;
    QByteArray settingsData;    
    if (auto sw = dynamic_cast<ICustomConversionSettingsWidget*>(m_currentSettingsWidget); sw) {
        settingsData = sw->settingsData();
        itemsCount = sw->itemsCount();
    }

    ConversionDefinition def;
    def.conversionType = m_conversionTypeComboBox->currentData().value<uint16_t>();
    def.itemsCount = itemsCount;
    def.input_LowLimit = m_inputLowSpinBox->value();
    def.input_HighLimit = m_inputHighSpinBox->value();
    QByteArray definitionData = def.data();

    QByteArray allData = definitionData + settingsData;
    FileHeader fh(allData);
    file.write(fh.data());
    file.write(allData);
    file.close();

    QString deviceFilePath = m_fm.devicePath(fileName);
    auto param = device()->paramsRegistry().element(SP_AIN_CONVERS_RULES_FILE, m_idx);
    param->updateValue(deviceFilePath);
    return true;
}

QByteArray CustomConversionDialog::dataFromFile() const
{
    auto param = device()->paramsRegistry().element(SP_AIN_CONVERS_RULES_FILE, m_idx);
    auto paramValue = param->value();
    if (paramValue.isEmpty())
        return QByteArray();

    auto filePath = m_fm.localPath(QFileInfo(paramValue).fileName());
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        MsgBox::error(QString("Не удалось открыть файл(%1): %2").arg(filePath).arg(file.errorString()));
        return QByteArray();
    }

    auto fileData = file.readAll();
    FileHeader fh;
    memcpy(&fh, fileData.data(), sizeof(fh));   
    fileData.remove(0, sizeof(fh));

    if (fh.defineId != FILE_ID) {
        MsgBox::error(QString("Не верный идентификатор файла '%1'").arg(fh.defineId));
        return QByteArray();
    }

    if (fh.len != fileData.size()) {
        MsgBox::error(QString("Не совпадает длина описания в объявлении(%1) и в файле(%2)").arg(fh.len).arg(fileData.size()));
        return QByteArray();
    }

    auto crc = Dpc::Sybus::crc16((uint8_t*)fileData.data(), fileData.size(), 0);
    if (fh.crc != crc) {
        MsgBox::error(QString("Не совпадает CRC описания в объявлении(%1) и CRC описания в файле(%2)").arg(fh.crc).arg(crc));
        return QByteArray();
    }

    return fileData;
}
