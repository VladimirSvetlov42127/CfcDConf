#pragma once

#include <dpc/sybus/channel/async/manager/IRequest.h>

namespace Dpc::Sybus
{
	// Множественый запрос.
	// В рамках одного запроса можно выполнять множетсво других.
	// Тип выполнения(последовательно или паралельно) задается отдельным свойством. По умолчанию последовательное.
	// Поведение при ошибке(Прервать или продолжить) задается отдельным свойством. По умолчанию прервать.
	// В случае неудачного выполнения любого из запросов, устанавливается ошибка от первого возникшего.
	// Берет на себя ответственость за удаление подзапросов, при удалении объекта. 

	class DPC_EXPORT MultipleRequest : public IRequest
	{
		Q_OBJECT

	public:
		enum ExecutionType
		{
			SequentialExecution = 0,
			ParallelExecution
		};

		enum ErrorPolicy 
		{
			TerminateOnError = 0,
			ContinueOnError
		};

		MultipleRequest(QObject *parent = nullptr);
		MultipleRequest(const QList<IRequest*> list, QObject* parent = nullptr);
		~MultipleRequest();

		void setExecutionType(ExecutionType type) { m_executionType = type; }
		ExecutionType executionType() const { return m_executionType; }

		void setErrorPolicy(ErrorPolicy policy) { m_errorPolicy = policy; }
		ErrorPolicy errorPolicy() const { return m_errorPolicy; }

		QList<IRequest*> requestsList() const { return m_list; }

		int finishedCount() const { return m_finishedCount; }

		void addRequest(IRequest* req);

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:

	private:
		void execNext();
		void execAll();

	private:
		ErrorPolicy m_errorPolicy;
		ExecutionType m_executionType;
		QList<IRequest*> m_list;
		int m_finishedCount;
		QString m_name;
	};
} // namespace