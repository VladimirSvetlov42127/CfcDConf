#pragma once

#include <dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.h>
#include <dpc/sybus/ParamAttribute.h>

namespace Dpc::Sybus
{
	class ReadParamRequest;

	// Класс для чтения атирбутов параметра

	class DPC_EXPORT AttributeRequest : public IMetadataRequest
	{
		Q_OBJECT

	public:
		AttributeRequest(uint16_t addr, QObject* parent = nullptr);
		AttributeRequest(uint16_t addr, int board, QObject *parent = nullptr);

		ParamAttribute attribute() const { return m_attr; }

	protected:
		virtual void run() override;
		virtual bool onRequestFinishedSuccessfully() override;

	private:
		ReadParamRequest* m_request;
		ParamAttribute m_attr;
	};
} // namespace