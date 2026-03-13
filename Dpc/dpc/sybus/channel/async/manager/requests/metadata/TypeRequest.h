#pragma once

#include <dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.h>

namespace Dpc::Sybus
{
	class ReadParamRequest;

	// Класс для чтения типа параметра

	class DPC_EXPORT TypeRequest : public IMetadataRequest
	{
		Q_OBJECT

	public:
		TypeRequest(uint16_t addr, QObject* parent = nullptr);
		TypeRequest(uint16_t addr, int board, QObject *parent = nullptr);

		uint8_t type() const { return m_type; }

	protected:
		virtual void run() override;
		virtual bool onRequestFinishedSuccessfully() override;

	private:
		ReadParamRequest* m_request;
		uint8_t m_type;
	};
} // namespace