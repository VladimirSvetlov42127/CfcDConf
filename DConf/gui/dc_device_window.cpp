#include "dc_device_window.h"

#include <QDebug>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QTreeView>
#include <QBoxLayout>
#include <QStackedWidget>
#include <QTableWidget>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QHeaderView>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QLabel>
#include <QProgressBar>

#include <dpc/gui/widgets/journal/JournalWidget.h>
#include <dpc/gui/dialogs/channel/ChannelsDialog.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "project/dc_device_node.h"
#include "data_model/dc_controller.h"

#include "device_operations/dc_operation_handler.h"
#include "device_operations/restart_operation.h"
#include "device_operations/reset_operation.h"
#include "device_operations/soft_update_operation.h"
#include "device_operations/config_read_operation.h"
#include "device_operations/config_write_operation.h"
#include "device_operations/config_passport_operation.h"
#include "device_operations/information_operation.h"

#include "gui/dc_device_view.h"
#include "gui/forms/DcFormFactory.h"
#include "gui/forms/default/DcDefaultForm.h"
#include "gui/forms/main/main_form.h"
#include "gui/forms/main/filesystem_dialog.h"
#include "gui/forms/main/passport_dialog.h"

using namespace Dpc;
using namespace Dpc::Gui;
using namespace Dpc::Sybus;

namespace {

constexpr const char *SETTING_CHANNEL = "channel";

class ReportDialog : public QDialog
{
public:
    ReportDialog(const QStringList &reportList, QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Предупреждения");
        setWindowIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning)));

        QTableWidget *table = new QTableWidget(this);
        table->setRowCount(reportList.size());
        table->setColumnCount(1);
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setHighlightSections(false);
        table->horizontalHeader()->setVisible(false);
        table->setWordWrap(false);

        for (int i = 0; i < reportList.size(); i++) {
            auto item = new QTableWidgetItem(reportList.at(i));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            table->setItem(i, 0, item);
        }

        QPushButton *okButton = new QPushButton("Ok");
        connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

        QHBoxLayout *buttonsLayout = new QHBoxLayout;
        buttonsLayout->addStretch();
        buttonsLayout->addWidget(okButton);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(table);
        mainLayout->addLayout(buttonsLayout);

        resize(800, 400);
    }
};

} // namespace

DcDeviceWindow::DcDeviceWindow(DcDeviceNode *device_node, QWidget *parent)
    : QMainWindow{ parent }
    , m_deviceNode{ device_node }
    , m_view{ new DcDeviceView(config(), this) }
    , m_formsStackedWidget{ new QStackedWidget(this) }
    , m_mainForm{ new MainForm(config()) }
    , m_operationHeandler{ new DcOperationHandler(this) }
    , m_journal{ new Journal(this) }
    , m_formTitleLabel{ new QLabel(this) }
    , m_progressBar{ new QProgressBar(this) }
    , m_filesystemDialog{ nullptr }
{
    setWindowTitle(device_node->fullText());
    setWindowIcon(config() ? config()->icon() : QIcon());

    connect(node(), &DcNode::fullTextChanged, this, &DcDeviceWindow::onNodeTextChanged);
    connect(m_view, &DcDeviceView::itemSelected, this, &DcDeviceWindow::onViewItemSelected);

    m_formTitleLabel->setStyleSheet("font-weight: bold; padding: 5px 0px 10px 0px;");
    QFont font = m_formTitleLabel->font();
    font.setPixelSize(14);
    m_formTitleLabel->setFont(font);

    m_progressBar->setFixedWidth(250);
    m_progressBar->setRange(0, 100);
    m_progressBar->setVisible(false);

    connect(m_operationHeandler, &DcOperationHandler::infoMsg, this, &DcDeviceWindow::addJournalInfo);
    connect(m_operationHeandler, &DcOperationHandler::debugMsg, this, &DcDeviceWindow::addJournalDebug);
    connect(m_operationHeandler, &DcOperationHandler::warningMsg, this, &DcDeviceWindow::addJournalWarning);
    connect(m_operationHeandler, &DcOperationHandler::errorMsg, this, &DcDeviceWindow::addJournalError);
    connect(m_operationHeandler, &DcOperationHandler::progress, this, &DcDeviceWindow::onOHProgress);
    connect(m_operationHeandler, &DcOperationHandler::finished, this, &DcDeviceWindow::onOHFinished);    
    connect(m_mainForm, &MainForm::operationRequest, this, &DcDeviceWindow::execOperation);
    m_formsStackedWidget->addWidget(m_mainForm);

    auto formsTitleLayout = new QGridLayout;
    formsTitleLayout->setContentsMargins(10, 5, 10, 0);
    formsTitleLayout->addWidget(m_formTitleLabel, 0, 0, Qt::AlignVCenter);
    formsTitleLayout->addWidget(m_progressBar, 0, 2);
    formsTitleLayout->setColumnStretch(1, 2);
    formsTitleLayout->setColumnStretch(2, 1);

    auto formsAreaWidget = new QWidget(this);
    auto formsAreaLayout = new QVBoxLayout(formsAreaWidget);
    formsAreaLayout->setContentsMargins(0, 0, 0, 0);
    formsAreaLayout->setSpacing(0);
    formsAreaLayout->addLayout(formsTitleLayout);
    formsAreaLayout->addWidget(m_formsStackedWidget);

    auto journalWidget = new JournalWidget(m_journal, this);
    journalWidget->setSourceBoxVisible(false);
    journalWidget->setVisible(false);

    auto workAreaSplitter = new QSplitter(Qt::Vertical);
    workAreaSplitter->addWidget(formsAreaWidget);
    workAreaSplitter->addWidget(journalWidget);
    workAreaSplitter->setStretchFactor(0, 1);

    auto mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(m_view);
    mainSplitter->addWidget(workAreaSplitter);
    mainSplitter->setStretchFactor(1, 1);
    setCentralWidget(mainSplitter);

    setStatusBar(new QStatusBar(this));

    auto toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
//    toolBar->setIconSize(QSize(32, 32));
    toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolBar->setAllowedAreas(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, toolBar);
    createActions();

    toolBar->addAction(m_connectAction);
    toolBar->addAction(m_disconnectAction);
    toolBar->addSeparator();
    toolBar->addAction(m_reloadInfoAction);
    toolBar->addAction(m_restartAction);
    toolBar->addSeparator();
    toolBar->addAction(m_readConifAction);
    toolBar->addAction(m_resetAction);
    toolBar->addAction(m_writeConfigAction);
    toolBar->addSeparator();
    toolBar->addAction(m_filesystemAction);
    toolBar->addAction(m_firmwareUpdateAction);
    toolBar->addAction(m_passportAction);

    auto journalViewAction = new QAction("Журанл соединения", this);
    journalViewAction->setCheckable(true);
    journalViewAction->setChecked(journalWidget->isVisible());
    connect(journalViewAction, &QAction::toggled, journalWidget, [=](bool visible) {
        journalWidget->setVisible(visible);
        if (visible)
            journalWidget->start();
        else
            journalWidget->stop();
    } );

    auto configMenu = menuBar()->addMenu("Конфигурация");
    configMenu->addAction(m_saveAction);
    configMenu->addAction(m_defaultAction);
    configMenu->addSeparator();
    configMenu->addAction(m_quitAction);

    auto editMenu = menuBar()->addMenu("Правка");
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);

    auto viewMenu = menuBar()->addMenu("Вид");
    viewMenu->addAction(journalViewAction);

    auto deviceMenu = menuBar()->addMenu("Устройство");
    deviceMenu->addAction(m_connectAction);
    deviceMenu->addAction(m_disconnectAction);
    deviceMenu->addSeparator();
    auto deviceConfigMenu = deviceMenu->addMenu("Конфигурация");
    deviceMenu->addSeparator();
    deviceMenu->addAction(m_restartAction);
    deviceMenu->addSeparator();
    deviceMenu->addAction(m_filesystemAction);
    deviceMenu->addAction(m_firmwareUpdateAction);
    deviceConfigMenu->addAction(m_readConifAction);
    deviceConfigMenu->addAction(m_writeConfigAction);
    deviceConfigMenu->addAction(m_resetAction);
    deviceConfigMenu->addAction(m_passportAction);

    m_saveAction->setEnabled(false);
    m_defaultAction->setEnabled(false);
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);

    setDeviceActionsEnabled(false);
    setCurrentForm(m_mainForm);
}

DcDeviceWindow::~DcDeviceWindow()
{
}

DcDeviceNode *DcDeviceWindow::node() const
{
    return m_deviceNode;
}

void DcDeviceWindow::onNodeTextChanged()
{
    setWindowTitle(node()->fullText());
}

void DcDeviceWindow::onQuitAction()
{
    this->close();
}

void DcDeviceWindow::onSaveAction()
{

}

void DcDeviceWindow::onDefaultAction()
{

}

void DcDeviceWindow::onUndoAction()
{

}

void DcDeviceWindow::onRedoAction()
{

}

void DcDeviceWindow::onConnectAction()
{
    auto channelSettings = settingsFromString(config()->settingValue(SETTING_CHANNEL));
    ChannelsDialog dlg(channelSettings, m_journal, this);
    if (QDialog::Accepted != dlg.exec())
        return;

    auto channel = dlg.channel();
    channelSettings = channel->settings();
    channelSettings[PasswordSetting] = QString();
    config()->setSetting(SETTING_CHANNEL, settingsToString(channelSettings));

    //	27.02.2024	//	Соединение с контроллером
//    config()->channel()->connect(true);
//    uint8_t rtu_mode = 255;
//    Dpc::Sybus::ParamPackPtr pack = config()->channel()->param(SP_ALTERNATE_MODE);
//    if (pack)
//        rtu_mode = pack->value<uint8_t>();
//    if (rtu_mode == 0) {
//        QString message = "Контроллер работает в режиме RTU-3M. Режим RTU-3M не поддерживается текущей версией DConf. Вы хотите перевести устройство в режим depRTU?";
//        bool ask = MsgBox::question(message);
//        if (!ask) {
//            config()->channel()->disconnect();
//            config()->setChannel(ChannelPtr());

//            return;
//        }

//        //	Перевод контроллера в режим depRTU
//        QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//        auto pItem = ParamPack::create(T_BYTE, 0x0FE3, 0);
//        pItem->appendValue(0);
//        auto res = config()->channel()->setParam(pItem);

//        pItem = ParamPack::create(T_BYTE, SP_ALTERNATE_MODE, 0);
//        pItem->appendValue(1);
//        res = config()->channel()->setParam(pItem);

//        pItem = ParamPack::create(T_BYTE, SP_SAVECFG, 0);
//        pItem->appendValue(1);
//        res = config()->channel()->setParam(pItem);
//        QThread::sleep(20);

//        pItem = ParamPack::create(T_BYTE, SP_RESET_DEVICE, 0);
//        pItem->appendValue(1);
//        res = config()->channel()->setParam(pItem);
//        QThread::sleep(10);
//    }
//    QGuiApplication::restoreOverrideCursor();
//    config()->channel()->disconnect();
//    //	27.02.2024	//	Отключение от контроллера

//    for (int i = 1; i < m_tabWidget->count(); i++) {
//        m_tabWidget->setTabVisible(i, true);
//    }

//    updateInfo();


    m_mainForm->setTabsActive(true);
    setDeviceActionsEnabled(true);

    m_operationHeandler->setChannel(channel);
    onReloadInfoAction();
}

void DcDeviceWindow::onDisconnectAction()
{
    m_mainForm->setTabsActive(false);
    setDeviceActionsEnabled(false);

    m_operationHeandler->setChannel(ChannelPtr());    
}

void DcDeviceWindow::onReloadInfoAction()
{
    auto op = std::make_shared<InformationOperation>(m_mainForm->paramAddrList());
    execOperation(op);
}

void DcDeviceWindow::onReadConfigAction()
{
    if (!MsgBox::question(QString("Вы уверены что хотите продолжить? "
                                  "Текущая конфигурация устройства в папке проекта будет полностью удалена, "
                                  "включая все конфигурационные файлы")))
        return;

    auto op = std::make_shared<ConfigReadOperation>(config());
    execOperation(op);
}

void DcDeviceWindow::onWriteConfigAction()
{
    if (!MsgBox::question(QString("Вы уверены что хотите продолжить? "
                                  "Все параметры и конфигурационные файлы на устройстве будут перезаписаны")))
        return;

    // Защита от случайного перезаписывания ip-адреса соединения
    auto channel = m_operationHeandler->channel();
    if (ChannelType::TCP == channel->type()) {
        auto connectionIp = channel->settings().value(IpAddrSetting).toString().trimmed();
        auto configIp = config()->paramsRegistry().element(SP_IP4_ADDR, 0);
        if (configIp && configIp->value().trimmed() != connectionIp)
            if (!MsgBox::question(
                    QString("Текущий IP-адрес устройства %1, "
                            "отличается от адреса в записываемой конфигурации %2, продолжить?")
                        .arg(connectionIp, configIp->value())))
                return;
    }

    auto op = std::make_shared<ConfigWriteOperation>(config());
    execOperation(op);
}

void DcDeviceWindow::onRestartAction()
{
    auto op = std::make_shared<RestartOperation>(Channel::HardReset);
    execOperation(op);
}

void DcDeviceWindow::onResetAction()
{
    if (!MsgBox::question(QString("Вы уверены что хотите продолжить? "
                                  "Вся конфигурация на устройстве будет сброшена к заводским настройкам")))
        return;

    auto op = std::make_shared<ResetOperation>();
    execOperation(op);
}

void DcDeviceWindow::onPassportAction()
{
    DcPassportDialog dlg(config(), this);
    if (QDialog::Rejected == dlg.exec())
        return;

    QSettings settings;
    auto filePathKey = QString("PassportConfig/FilePath");
    auto dir = settings.value(filePathKey, "/home").toString();
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить в файл", dir, "*.html;;*.odt;;*.pdf");
    if (fileName.isEmpty())
        return;

    settings.setValue(filePathKey, QFileInfo(fileName).absolutePath());
    auto op = std::make_shared<ConfigPassportOperation>(config(), dlg.structure(), fileName);
    execOperation(op);
}

void DcDeviceWindow::onFilesystemAction()
{
    if (!m_filesystemDialog) {
        m_filesystemDialog = new FilesystemDiaolog(this);
        connect(m_filesystemDialog, &FilesystemDiaolog::operationRequest, this, &DcDeviceWindow::execOperation);
        m_filesystemDialog->start();
    }

    m_filesystemDialog->setModal(true);
    m_filesystemDialog->show();

    if (!m_operationHeandler->isBusy())
        m_filesystemDialog->start();
}

void DcDeviceWindow::onFirmwareUpdateAction()
{
    auto fileName = QFileDialog::getOpenFileName(0, "Выбрать файлы", "", "*.img");
    if (fileName.isEmpty())
        return;

    auto op = std::make_shared<SoftUpdateOperation>(fileName);
    execOperation(op);
}

void DcDeviceWindow::onViewItemSelected(const QString &itemIdx, bool isConfigItem)
{
    DcForm* nextForm = nullptr;
    if (!isConfigItem) {
        nextForm = m_mainForm;
    }
    else {
        auto newForm = DcFormFactory::instance().build(itemIdx, config());
        if (!newForm)
            newForm = std::make_unique<DcDefaultForm>(config());

        nextForm = newForm.get();
        newForm->setObjectName(itemIdx);
        newForm->setJournal(m_journal);
        m_formsStackedWidget->addWidget(newForm.get());
        m_currentConfigWidget = std::move(newForm);
    }

    setCurrentForm(nextForm);
}

void DcDeviceWindow::execOperation(AbstractOperation::SPtr operation)
{
    if (m_operationHeandler->isBusy()) {
        MsgBox::info("В данный момент устройство занято выполнением другой операции");
        return;
    }

    if (!m_operationHeandler->exec(operation))
        return;

    setDeviceActionsEnabled(false);
    setProgressIndicator(!operation->hasProgress());
    m_progressBar->setVisible(true);    
    statusBar()->showMessage("Выполнение ...");    
}

void DcDeviceWindow::onOHProgress(int value)
{
//    qDebug() << "Progress" << value;
    m_progressBar->setValue(value);
}

void DcDeviceWindow::onOHFinished(AbstractOperation *operation)
{
    m_progressBar->setVisible(false);
    statusBar()->showMessage("Готово");
    setDeviceActionsEnabled(true);

    switch (operation->state()) {
    case AbstractOperation::ErrorState:
        MsgBox::error("Операция была прервана из-за наличия ошибок");
        break;
    case AbstractOperation::WarningState:
        ReportDialog(operation->reportList(), this).exec();
        [[fallthrough]];
    case AbstractOperation::NoErrorState:
        operationFinished(operation);
        break;
    }
}

void DcDeviceWindow::addJournalInfo(const QString &msg)
{
    m_journal->addInfoMessage(msg);
}

void DcDeviceWindow::addJournalDebug(const QString &msg, int level)
{
    m_journal->addDebugMessage(msg, level);
}

void DcDeviceWindow::addJournalWarning(const QString &msg)
{
    m_journal->addWarningMessage(msg);
}

void DcDeviceWindow::addJournalError(const QString &msg)
{
    m_journal->addErrorMessage(msg);
}

DcController *DcDeviceWindow::config() const
{
    return node()->config();
}

void DcDeviceWindow::createActions()
{
    m_quitAction = new QAction("Выход", this);
    m_quitAction->setShortcut(QKeySequence::Quit);
    connect(m_quitAction, &QAction::triggered, this, &DcDeviceWindow::onQuitAction);

    m_saveAction = new QAction("Сохранить", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    connect(m_saveAction, &QAction::triggered, this, &DcDeviceWindow::onSaveAction);

    m_defaultAction = new QAction("По умолчанию", this);
    m_defaultAction->setShortcut(QKeySequence::Save);
    connect(m_defaultAction, &QAction::triggered, this, &DcDeviceWindow::onDefaultAction);

    m_undoAction = new QAction("Отменить", this);
    m_undoAction->setShortcut(QKeySequence::Save);
    connect(m_undoAction, &QAction::triggered, this, &DcDeviceWindow::onUndoAction);

    m_redoAction = new QAction("Повторить", this);
    m_redoAction->setShortcut(QKeySequence::Save);
    connect(m_redoAction, &QAction::triggered, this, &DcDeviceWindow::onRedoAction);

    m_connectAction = new QAction(QIcon(":/icons/connect_on.svg"), "Соединить", this);
    connect(m_connectAction, &QAction::triggered, this, &DcDeviceWindow::onConnectAction);

    m_disconnectAction = new QAction(QIcon(":/icons/connect_off.svg"), "Разъединить", this);
    connect(m_disconnectAction, &QAction::triggered, this, &DcDeviceWindow::onDisconnectAction);

    m_writeConfigAction = new QAction(QIcon(":/icons/in.svg"), "Записать", this);
    m_writeConfigAction->setToolTip("Записать конфигурацию в устройство");
    connect(m_writeConfigAction, &QAction::triggered, this, &DcDeviceWindow::onWriteConfigAction);

    m_readConifAction = new QAction(QIcon(":/icons/out.svg"), "Прочитать", this);
    m_readConifAction->setToolTip("Прочитать конфигурацию из устройства");
    connect(m_readConifAction, &QAction::triggered, this, &DcDeviceWindow::onReadConfigAction);

    m_reloadInfoAction = new QAction(QIcon(":/icons/r_reload.svg"), "Обновить", this);
    m_reloadInfoAction->setToolTip("Перечитать иформационые и диагностические данные из устройства");
    connect(m_reloadInfoAction, &QAction::triggered, this, &DcDeviceWindow::onReloadInfoAction);

    m_restartAction = new QAction(QIcon(":/icons/loading.svg"), "Перезагрузка", this);
    m_restartAction->setToolTip("Перезагрузка устройства в выбраном режиме");
    connect(m_restartAction, &QAction::triggered, this, &DcDeviceWindow::onRestartAction);

    m_resetAction = new QAction(QIcon(":/icons/factoryset.svg"), "Сброс", this);
    m_resetAction->setToolTip("Сброс устройства к заводской конфигурации");
    connect(m_resetAction, &QAction::triggered, this, &DcDeviceWindow::onResetAction);

    m_filesystemAction = new QAction(QIcon(":/icons/file_system.svg"), "Файловая система", this);
    m_filesystemAction->setToolTip("Просмотр файловой системы устройства");
    connect(m_filesystemAction, &QAction::triggered, this, &DcDeviceWindow::onFilesystemAction);

    m_firmwareUpdateAction = new QAction(QIcon(":/icons/setting.svg"), "Обновление", this);
    m_firmwareUpdateAction->setToolTip("Обновление системы устройства");
    connect(m_firmwareUpdateAction, &QAction::triggered, this, &DcDeviceWindow::onFirmwareUpdateAction);

    m_passportAction = new QAction(QIcon(":/icons/list.svg"), "Паспорт", this);
    m_passportAction->setToolTip("Создание отчёта об конфигурации устройства");
    connect(m_passportAction, &QAction::triggered, this, &DcDeviceWindow::onPassportAction);
}

void DcDeviceWindow::setDeviceActionsEnabled(bool enabled)
{
    m_disconnectAction->setEnabled(enabled);
    m_writeConfigAction->setEnabled(enabled);
    m_readConifAction->setEnabled(enabled);
    m_reloadInfoAction->setEnabled(enabled);
    m_resetAction->setEnabled(enabled);
    m_restartAction->setEnabled(enabled);
    m_filesystemAction->setEnabled(enabled);
    m_firmwareUpdateAction->setEnabled(enabled);
    m_passportAction->setEnabled(enabled);
}

void DcDeviceWindow::operationFinished(AbstractOperation *operation)
{
    m_mainForm->operationFinished(operation);

    if (operation->flags() & AbstractOperation::Filesystem) {
        m_filesystemDialog->operationFinished(operation);
        return;
    }

    if (operation->flags() & AbstractOperation::Read)
        return;

    onReloadInfoAction();
}

void DcDeviceWindow::setCurrentForm(DcForm *form)
{
    m_formTitleLabel->setText(form ? form->title() : QString());
    m_formsStackedWidget->setCurrentWidget(form);
}

void DcDeviceWindow::setProgressIndicator(bool busyIndicator)
{
    m_progressBar->setMaximum(busyIndicator ? 0 : 100);
}
