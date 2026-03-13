#pragma once

#include <dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.h>

namespace Dpc::Sybus
{
	class ReadParamRequest;

	// Класс для чтения списка параметров.

	class DPC_EXPORT ParamListRequest : public IMetadataRequest
	{
		Q_OBJECT

	public:
		ParamListRequest(QObject* parent = nullptr);
		ParamListRequest(int board, QObject *parent = nullptr);

		QList<uint16_t> list() const { return m_list; }

	protected:
		virtual void run() override;
		virtual bool onRequestFinishedSuccessfully() override;
		virtual QString subject() const override;

	private:
		ReadParamRequest* m_request;
		QList<uint16_t> m_list;
	};
} // namespace