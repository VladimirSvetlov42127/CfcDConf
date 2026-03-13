#include "ChannelsDialog.h"
#include "dpc/dep_settings.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qdebug.h>
#include <qapplication.h>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <dpc/gui/dialogs/channel/VComSettingsWidget.h>
#include <dpc/gui/dialogs/channel/TcpSettingsWidget.h>
#include <dpc/gui/dialogs/channel/Rs485SettingsWidget.h>

namespace {
	namespace Text {
		namespace Error {
			const QString INVALID_LOGIN_PASSWORD = "Не задано имя пользователя или пароль";
			const QString UNKNOWN_CHANNEL_TYPE = "Неизвестный тип канала связи: %1";
		}
	}
}

namespace Dpc::Gui
{
	using namespace Dpc::Sybus;

    ChannelsDialog::ChannelsDialog(const SettingsType& settings, Dpc::Journal* journal, QWidget* parent) :
		QDialog(parent),
		m_settingsGroupBox(new QGroupBox("Параметры", this)),
		m_interfaceGroupBox(new QGroupBox("Интерфейс", this)),
		m_loginEdit(new QLineEdit(this)),
		m_passwordEdit(new QLineEdit(this)),
		m_connectButton(new QPushButton("Соединить", this)),
		m_authorizationGroupBox(new QGroupBox("Авторизация", this)),
		//m_authorizationLabel(new QLabel("Авторизация", this)),
        m_journal(journal),
		m_settings(settings),
		m_asyncMode(false),
		m_isAuthorizationRequired(true)
	{
#ifdef QT_DEBUG
		auto password = QString("admin");
#else
		auto password = settings.value(PasswordSetting).toString();
#endif	

		setWindowTitle("Выбор канала связи с устройством");

        auto interfaceGroupBoxLayout = new QVBoxLayout(m_interfaceGroupBox);

		m_settingsGroupBox->setLayout(new QVBoxLayout);
		m_settingsGroupBox->layout()->setContentsMargins(3, 3, 3, 3);

		auto vcomRadio = new QRadioButton("Виртуальный COM-порт (USB)");
		interfaceGroupBoxLayout->addWidget(vcomRadio);
		auto tcpRadio = new QRadioButton("TCP/IP");
		interfaceGroupBoxLayout->addWidget(tcpRadio);
		auto rs485Radio = new QRadioButton("Ведомый через RS-485");
		interfaceGroupBoxLayout->addWidget(rs485Radio);

		m_loginEdit->setMaximumWidth(150);
		m_loginEdit->setMaxLength(16);
		m_loginEdit->setPlaceholderText("Имя пользователя");
		auto login = settings.value(LoginSetting).toString();
		m_loginEdit->setText(!login.isEmpty() ? login : "admin");

		m_passwordEdit->setMaximumWidth(150);
		m_passwordEdit->setMaxLength(16);
		m_passwordEdit->setPlaceholderText("Пароль");
		m_passwordEdit->setEchoMode(QLineEdit::Password);
		m_passwordEdit->setText(password);

		m_connectButton->setDefault(true);
		m_connectButton->setMinimumSize(100, 24);
		QPushButton * canselButton = new QPushButton("Отмена", this);
		canselButton->setMinimumSize(100, 24);
		canselButton->setMaximumWidth(150);
		canselButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

		auto onRadioButton = [=]() {
			for (auto child : m_settingsGroupBox->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
				delete child;

			QWidget* settingsWidget = nullptr;
			if (vcomRadio->isChecked())
				settingsWidget = new VComSettingsWidget(settings, this);
			else if (tcpRadio->isChecked())
				settingsWidget = new TcpSettingsWidget(settings, this);
			else if (rs485Radio->isChecked())
				settingsWidget = new Rs485SettingsWidget(settings, this);

			if (!settingsWidget)
				return;

			m_settingsGroupBox->layout()->addWidget(settingsWidget);
			m_settingsGroupBox->setFocusProxy(settingsWidget);
		};

		connect(vcomRadio, &QRadioButton::toggled, this, onRadioButton);
		connect(tcpRadio, &QRadioButton::toggled, this, onRadioButton);
		connect(rs485Radio, &QRadioButton::toggled, this, onRadioButton);
		connect(m_connectButton, &QPushButton::clicked, this, &ChannelsDialog::onConnectButton);
		connect(canselButton, &QPushButton::clicked, this, &ChannelsDialog::reject);

		switch (settings.value(TypeSetting).toUInt())
		{
		case VCOM: vcomRadio->setChecked(true); break;
		case TCP: tcpRadio->setChecked(true); break;
		case RS485vTCP: 
		case RS485vVCOM: rs485Radio->setChecked(true); break;
		default: vcomRadio->setChecked(true); break;
		}

		auto channelLabel = new QLabel("Настройка канала");
		channelLabel->setStyleSheet("font-weight: bold;");

		//m_authorizationLabel->setStyleSheet("font-weight: bold;");
        m_authorizationGroupBox->setStyleSheet("QGroupBox  { font-weight: bold; }");
		m_authorizationGroupBox->setLayout(new QVBoxLayout(this));
		m_authorizationGroupBox->layout()->setContentsMargins(3, 3, 3, 3);
		m_authorizationGroupBox->layout()->addWidget(m_loginEdit);

		m_authorizationGroupBox->layout()->addWidget(m_passwordEdit);

		auto layout = new QGridLayout(this);
		int row = 0;

		layout->addWidget(channelLabel, row, 0, 1, 3, Qt::AlignCenter);
		row++;

		layout->addWidget(m_interfaceGroupBox, row, 0);
		layout->addWidget(m_settingsGroupBox, row, 1, 1, 2);
		row++;

        layout->addWidget(m_authorizationGroupBox, row, 0, 1, 3, Qt::AlignLeft);
		//layout->addWidget(m_authorizationLabel, row, 0, 1, 3, Qt::AlignCenter);
		row++;

		//layout->addWidget(m_loginEdit, row, 0);
		//row++;
		//layout->addWidget(m_passwordEdit, row, 0);
		//row++;

		layout->addWidget(m_connectButton, row, 2);
		layout->addWidget(canselButton, row, 0, Qt::AlignLeft);

		layout->setColumnStretch(1, 1);

		resize(500, 350);
	}

	ChannelsDialog::~ChannelsDialog()
	{
	}

	void ChannelsDialog::setAuthorizationRequired(bool required)
	{
		m_isAuthorizationRequired = required;

		bool visible = required && !isAsyncMode();
		m_authorizationGroupBox->setVisible(visible);
	}

	void ChannelsDialog::onConnectButton()
	{
		auto settingsWidget = m_settingsGroupBox->findChild<ISettingsWidget*>();
		if (!settingsWidget)
			return;

		auto error = settingsWidget->error();
		if (!error.isEmpty()) {
			MsgBox::error(error);
			return;
		}

		m_settings = settingsWidget->settings();
		if (isAuthorizationRequired()) {
			if (m_loginEdit->text().isEmpty()) {
				MsgBox::error(Text::Error::INVALID_LOGIN_PASSWORD);
				return;
			}

			if (m_passwordEdit->text().isEmpty()) {
				MsgBox::error(Text::Error::INVALID_LOGIN_PASSWORD);
				return;
			}

			m_settings[LoginSetting] = m_loginEdit->text();
			m_settings[PasswordSetting] = m_passwordEdit->text();
		}

		if (m_asyncMode) {
			createAsyncChannel();
			return;
		}

		if (!createChannel())
			return;

		accept();
	}

	QGroupBox* ChannelsDialog::gb_settings()
	{
		return m_settingsGroupBox;
	}

	QGroupBox* ChannelsDialog::gb_interface()
	{
		return m_interfaceGroupBox;
	}

	QGroupBox* ChannelsDialog::gb_authorization()
	{
		return m_authorizationGroupBox;
	}

    QString ChannelsDialog::connectionString(Sybus::SettingsType set)
	{
        set.remove(login_password);
		QString connection_string;
        quint8 type = set[connector_type].toInt();
        if (type == VCOM)
            connection_string = QString("Последовательный порт: ")
                                + set[com_port_name].toString();
        if (type == TCP)
            connection_string = QString("IP адрес: ")
                                + set[ip_address].toString()
                                + QString(" порт: ") + set[ip_port].toString();
        if (type == RS485vVCOM)
            connection_string = QString("SLAVE через последовательный порт: ")
                                + set[com_port_name].toString();
        if (type == RS485vTCP)
            connection_string = QString("SLAVE через IP адрес: ")
                                + set[ip_address].toString()
                                + QString(" порт: ")
                                + set[ip_port].toString();
		connection_string = '(' + connection_string + ')';
		return connection_string;
	}

	void ChannelsDialog::onChannelInfo(const QString& msg) const
	{
        if (!m_journal)
            return;

        m_journal->addInfoMessage(msg);
	}

	void ChannelsDialog::onChannelWarning(const QString& msg) const
	{
        if (!m_journal)
            return;

        m_journal->addWarningMessage(msg);
	}
	
	void ChannelsDialog::onChannelDebug(int level, const QString& msg) const
	{
        if (!m_journal)
            return;

        m_journal->addDebugMessage(msg, level);
	}

	void ChannelsDialog::onChannelError(Dpc::Sybus::Channel::ErrorType errType, int errCode, const QString& errMsg) const
	{
        if (!m_journal)
            return;

        m_journal->addErrorMessage(Channel::formatErrorMsg(errType, errCode, errMsg));
	}

	void ChannelsDialog::onAsyncChannelConnected()
	{
		disconnect(m_asyncChannel.get(), &AsyncChannel::connectedToDevice, this, &ChannelsDialog::onAsyncChannelConnected);
		disconnect(m_asyncChannel.get(), &AsyncChannel::infoMsg, this, &ChannelsDialog::onAsyncChannelInfo);
		disconnect(m_asyncChannel.get(), &AsyncChannel::error, this, &ChannelsDialog::onAsyncChannelError);	

		QApplication::restoreOverrideCursor();
		m_connectButton->setEnabled(true);
		accept();
	}

	void ChannelsDialog::onAsyncChannelInfo(const QString& msg)
	{
        m_journal->addDebugMessage(msg, 0);
	}

	void ChannelsDialog::onAsyncChannelError(Dpc::Sybus::ChannelReply::Error err, const QString& errMsg)
	{
        m_journal->addErrorMessage(QString("%1(%2)").arg(errMsg).arg(toHex(err)));
		QApplication::restoreOverrideCursor();
		m_connectButton->setEnabled(true);
		MsgBox::error(QString("%1(%2)").arg(errMsg).arg(toHex(err)));
	}

	bool ChannelsDialog::createChannel()
	{
		auto channel = ChannelsFactory::createChannel(m_settings);
		if (!channel) {
			MsgBox::error(Text::Error::UNKNOWN_CHANNEL_TYPE.arg(m_settings.value(TypeSetting).toString()));
			return false;
		}

		auto errorType = Channel::NoErrorType;
		QString errorMsg;
		{
			connect(channel.get(), &Channel::info, this, &ChannelsDialog::onChannelInfo);
			connect(channel.get(), &Channel::debug, this, &ChannelsDialog::onChannelDebug);
			connect(channel.get(), &Channel::error, this, &ChannelsDialog::onChannelError);

			QApplication::setOverrideCursor(Qt::WaitCursor);
			channel->connect(isAuthorizationRequired());
			errorType = channel->errorType();
			errorMsg = channel->errorMsg();
			channel->disconnect();
			QApplication::restoreOverrideCursor();

			disconnect(channel.get(), &Channel::info, this, &ChannelsDialog::onChannelInfo);
			disconnect(channel.get(), &Channel::debug, this, &ChannelsDialog::onChannelDebug);
			disconnect(channel.get(), &Channel::error, this, &ChannelsDialog::onChannelError);
		}

		if (Channel::NoErrorType != errorType) {
			MsgBox::error(errorMsg);
			return false;
		}

		m_channel = channel;
		return true;
	}

	void ChannelsDialog::createAsyncChannel()
	{
		auto asyncChannel = ChannelsFactory::createAsyncChannel(m_settings);
		if (!asyncChannel) {
			MsgBox::error(Text::Error::UNKNOWN_CHANNEL_TYPE.arg(m_settings.value(TypeSetting).toString()));
			return;
		}

		m_asyncChannel = asyncChannel;
		connect(m_asyncChannel.get(), &AsyncChannel::infoMsg, this, &ChannelsDialog::onAsyncChannelInfo);
		connect(m_asyncChannel.get(), &AsyncChannel::error, this, &ChannelsDialog::onAsyncChannelError);
		connect(m_asyncChannel.get(), &AsyncChannel::connectedToDevice, this, &ChannelsDialog::onAsyncChannelConnected);

		QApplication::setOverrideCursor(Qt::WaitCursor);
		m_connectButton->setEnabled(false);
		m_asyncChannel->connectToDevice();
	}
} // namespace
