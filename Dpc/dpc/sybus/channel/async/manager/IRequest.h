#pragma once

#include <dpc/sybus/channel/async/manager/AsyncChannelManager.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT IRequest : public QObject
	{
		Q_OBJECT

	public:
		enum RequestState {
			Idle = 0, 
			Running,
			Finished
		};

		IRequest(QObject *parent = nullptr);
		virtual ~IRequest();

		// Состояние запроса
		RequestState state() const;

		// Код ошибки запроса
		// При успешном выполнении запроса, возвращает 0
		uint16_t error() const;

		// Сообщение ошибки запроса
		// При успешном выполнении запроса, возвращает пустую строку
		QString errorString() const;

		// Отформатированная строка из сообщения и кода ошибки: "errorString(error)".
		// Код ошибки выводится в hex.
		// При успешном выполнении запроса, возвращает пустую строку
		QString errorFormated() const;

		// Начать выполнение запроса
		void start();

		// Имя запроса. Если не пустое значение, используется как префикс в сообщениях об ошибке без префикса.
		virtual QString name() const;

	signals:
		// сигнал о прогрессе выполнения
		void progress(int value);

		// сигнал о завершении выполнения запроса
		void finished();

		// сигнал с информационым сообщением
		void infoMsg(const QString& msg);

		// сигнал с текстом ошибки
		void errorMsg(const QString& msg);

		// сигнал с предупредительным сообщением
		void warningMsg(const QString& msg);

	protected:
		// Выполнение запроса, должен быть определен в наследниках
		// запускается из exec
		virtual void run() = 0;		

		// Менеджер канала связи, через который нужно выполнить запрос
		AsyncChannelManager* manager() const;

		// Выполнить запрос req через текущий менеджер
		// При forwardMsgSignals = true, сигналы с сообщениями от req будут испускаться от текущего обьекта
		void exec(IRequest* req, bool forwardMsgSignals = false);

		// Установка кода и сообщения ошибки
		void setError(uint16_t err, const QString& errStr, const QString &msgPrefix = QString(), const QString &msgPostfix = QString());

		// Завершить выполнение запроса
		// Испускает сигнал finished
		void finish();

		// Добавить значение прогресса
		void addProgressValue(int value);

		// Добавить информационое собщение
		void addInfoMsg(const QString& msg);

		// Добавить сообщение с текстом ошибки
		void addErrorMsg(const QString& msg);

		// Добавить сообщение с предупреждением
		void addWarningMsg(const QString& msg);

		static QString successString();

	private:
		// Установка состояния запроса
		void setState(RequestState state);

		// Установка менеджера канала связи
		void setManager(AsyncChannelManager* manager);

	private:
		friend class AsyncChannelManager;

		RequestState m_state;
		AsyncChannelManager* m_manager;
		uint16_t m_err;
		QString m_errStr;
	};

	using IRequestPtr = std::shared_ptr<IRequest>;

} // namespace