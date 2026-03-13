#pragma once

#include <dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.h>

namespace Dpc::Sybus
{
	class ReadParamRequest;

	// Класс для чтения размера параметра в байтах. 
	// Полученое значение это размер одного элемента параметра в байтах, не путать с размерностью.

	class DPC_EXPORT SizeRequest : public IMetadataRequest
	{
		Q_OBJECT

	public:
		SizeRequest(uint16_t addr, QObject* parent = nullptr);
		SizeRequest(uint16_t addr, int board, QObject *parent = nullptr);

		uint16_t size() const { return m_size; }

	protected:
		virtual void run() override;
		virtual bool onRequestFinishedSuccessfully() override;

	private:
		ReadParamRequest* m_request;
		uint16_t m_size;
	};
} // namespace