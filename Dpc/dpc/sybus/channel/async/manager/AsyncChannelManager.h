#pragma once

#include <dpc/sybus/channel/async/AsyncChannel.h>

namespace Dpc::Sybus
{
	class IRequest;
	class IFileSystemCtrlRequest;
	class IFileSystemDataRequest;

	class DPC_EXPORT AsyncChannelManager : public QObject
	{
		Q_OBJECT

	public:
		AsyncChannelManager(QObject *parent = nullptr);
		virtual ~AsyncChannelManager();

		// Установка канала связи с устройством
		AsyncChannelPtr channel() const;
		void setChannel(const AsyncChannelPtr& newChannel);		

		// Размер буфера чтения
		uint16_t readBufferSize() const;

		// Размер буфера записи
		uint16_t writeBufferSize() const;

		// Размер буфера для операций с файлами
		uint16_t fileDataBufferSize() const;

		// Установлено ли соединение с устройством
		bool isConnectedToDevice() const;

		// Установить соединение с устройством
		void connectToDevice();

		// Разорвать соединение с устройством
		void disconnectFromDevice();

		// Выполнить запрос req
		// При forwardMsgSignals = true, сигналы с сообщениями от req будут также испускаться от текущего обьекта
		void exec(IRequest* req, bool forwardMsgSignals = true);

	signals:
		void connectedToDevice();
		void disconnectedFromDevice();
		void channelChanged();

		// Сигнал с информационым сообщением
		void infoMsg(const QString& msg);

		// Сигнал с текстом ошибки
		void errorMsg(const QString& msg);

		// Сигнал с предупредительным сообщением
		void warningMsg(const QString& msg);

	private:
		enum FSLoginState 
		{
			NotLoggedInState = 0,
			LoggingInState,
			LoggedInState
		};

		enum FSRequestType 
		{
			FSCtrlRequestType = 0x1,
			FSDataRequestType = 0x2,
			FSAnyRequestType = FSCtrlRequestType | FSDataRequestType
		};

	private slots:
		void onChannelConnected();
		void onChannelDisconnected();
		void onChannelError(ChannelReply::Error err, const QString& errMsg);

		void onFSCtrlRequestFinished();
		void onFSDataRequestFinished();

	private:
		bool isConnecting() const;

		void addInfoMsg(const QString& msg);
		void addErrorMsg(const QString& msg);
		void addWarningMsg(const QString& msg);

		void resetBuffers();
		void setReadBufferSize(uint16_t size);
		void setWriteBufferSize(uint16_t size);
		void setFileDataBufferSize(uint16_t size);

		void start(IRequest* req);

		template<class Functor>
		void start(IRequest* req, Functor function);

		void addFSCtrlRequest(IFileSystemCtrlRequest* req);
		void addFSDataRequest(IFileSystemDataRequest* req);
		void execNextFSRequest(FSRequestType type);

	private:
		AsyncChannelPtr m_channel;
		bool m_isConnected;
		bool m_isConnecting;

		uint16_t m_readBufferSize;
		uint16_t m_writeBufferSize;
		uint16_t m_fileDataBufferSize;

		bool m_gotRWBuffersSize;
		bool m_gotFileDataBuffersSize;

		std::queue<IRequest*> m_pendingRequestQueue;
		std::queue<IFileSystemCtrlRequest*> m_fsCtrlQueue;
		std::queue<IFileSystemDataRequest*> m_fsDataQueue;

		FSLoginState m_fsLoginState;
	};

	using AsyncChannelManagerPtr = std::shared_ptr<AsyncChannelManager>;	
} // namespace