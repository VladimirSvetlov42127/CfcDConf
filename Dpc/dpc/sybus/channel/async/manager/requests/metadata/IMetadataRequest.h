#pragma once

#include <dpc/sybus/channel/async/manager/IRequest.h>

namespace Dpc::Sybus
{
	class ReadParamRequest;

	class DPC_EXPORT IMetadataRequest : public IRequest
	{
		Q_OBJECT

	public:
		IMetadataRequest(const QString& title, uint16_t addr, QObject* parent = nullptr);
		IMetadataRequest(const QString& title, uint16_t addr, int board, QObject *parent = nullptr);

		uint16_t addr() const { return m_addr; }
		int board() const { return m_board; }

		virtual QString name() const override;

	protected:
		void runRequest(IRequest* req);

		// Если вернет true, запрос завершается
		virtual bool onRequestFinishedSuccessfully() = 0;	

		virtual QString subject() const;

	private slots:
		void onCurrentRequestFinished();

	private:
		QString title() const { return m_title; }

	private:
		IRequest* m_currentRequest;
		QString m_title;
		uint16_t m_addr;
		int m_board;
	};
} // namespace