#pragma once

#include <qdialog.h>

#include <dpc/sybus/channel/ChannelsFactory.h>
#include <dpc/journal/Journal.h>

class QLineEdit;
class QLabel;
class QGroupBox;

namespace Dpc::Gui
{
	class DPC_EXPORT ChannelsDialog : public QDialog
	{
		Q_OBJECT

	public:
        ChannelsDialog(const Sybus::SettingsType &settings, Dpc::Journal* journal = nullptr, QWidget *parent = nullptr);
		~ChannelsDialog();

		Sybus::ChannelPtr channel() const { return m_channel; }
		Sybus::AsyncChannelPtr asyncChannel() const { return m_asyncChannel; }

		bool isAsyncMode() const { return m_asyncMode; }
		void setAsyncMode(bool mode) { m_asyncMode = mode; }

		bool isAuthorizationRequired() const { return m_isAuthorizationRequired; }
		void setAuthorizationRequired(bool required);

		Sybus::SettingsType settings() const { return m_settings; }

		QGroupBox* gb_settings();
		QGroupBox* gb_interface();
		QGroupBox* gb_authorization();

        static QString connectionString(Sybus::SettingsType settings);
	public slots:
        void onConnectButton();
        void onAsyncChannelConnected();

	private slots:
		void onChannelInfo(const QString& msg) const;
		void onChannelWarning(const QString& msg) const;
		void onChannelDebug(int level, const QString& msg) const;
		void onChannelError(Dpc::Sybus::Channel::ErrorType errType, int errCode, const QString& errMsg) const;

		void onAsyncChannelInfo(const QString& msg);
		void onAsyncChannelError(Dpc::Sybus::ChannelReply::Error err, const QString& errMsg);

	private:
		bool createChannel();
		void createAsyncChannel();

	private:
		QGroupBox* m_settingsGroupBox;
		QGroupBox* m_interfaceGroupBox;
		QLineEdit* m_loginEdit;
		QLineEdit* m_passwordEdit;
		QPushButton* m_connectButton;
		QGroupBox* m_authorizationGroupBox;
		//QLabel* m_authorizationLabel;

        Dpc::Journal* m_journal;

		Sybus::SettingsType m_settings;
		Sybus::ChannelPtr m_channel;
		Sybus::AsyncChannelPtr m_asyncChannel;
		bool m_asyncMode;
		bool m_isAuthorizationRequired;
	};
} // namespace
