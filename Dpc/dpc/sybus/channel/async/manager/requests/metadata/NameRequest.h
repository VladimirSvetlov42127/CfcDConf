#pragma once

#include <dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.h>
#include <dpc/sybus/ParamPack.h>

namespace Dpc::Sybus
{
	class ReadParamRequest;

	class DPC_EXPORT NameRequest : public IMetadataRequest
	{
		Q_OBJECT

	public:

		// Класс для чтения имен параметра
		// 1. Если размерность параметра 1, будет возвращено 1 значение.
		// 2. Если параметр имеет размерность больше 1, но при этом возвращено 1 значение, это означает что у всех подпараметров одно и тоже имя.
		// 3. Если параметр имеет размерность больше 1, то будет возвращено N + 1 для 1-мерных параметров и М * N + 1 для 2х-мерных парамтеров имён.
		// При этом имя под индексом 0 в пакете, обобщенное имя параметра, а имена подпараметров начинаются с 1го индекса. 

		NameRequest(uint16_t addr, QObject* parent = nullptr);
		NameRequest(uint16_t addr, uint16_t index, QObject* parent = nullptr);
		NameRequest(uint16_t addr, uint16_t index, int count, QObject* parent = nullptr);
		NameRequest(uint16_t addr, uint16_t index, int count, int board, QObject* parent = nullptr);

		uint16_t index() const { return m_index; }
		uint16_t count() const { return m_count; }

		ParamPackPtr pack() const { return m_pack; }

	protected:
		virtual void run() override;
		virtual bool onRequestFinishedSuccessfully();
		virtual QString subject() const override;

	private:
		ReadParamRequest* m_request;
		uint16_t m_index;
		uint16_t m_count;

		ParamPackPtr m_pack;
	};
} // namespace