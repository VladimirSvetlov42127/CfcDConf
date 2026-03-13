#pragma once

#include <dpc/sybus/channel/async/manager/requests/metadata/IMetadataRequest.h>
#include <dpc/sybus/ParamAttribute.h>
#include <dpc/sybus/ParamDimension.h>

namespace Dpc::Sybus
{
	// Класс для чтения размерности параметра.
	// для того чтобы прочитать размерность параметра, нужно сначала прочитать его атрибуты.
	// в зависимости от атрибута 2D, по разному трактуется полученое значение размерности.

	class DPC_EXPORT DimensionRequest : public IMetadataRequest
	{
		Q_OBJECT

	public:
		DimensionRequest(uint16_t addr, QObject* parent = nullptr);
		DimensionRequest(uint16_t addr, int board, QObject *parent = nullptr);

		ParamAttribute attribute() const { return m_attr; }
		ParamDimension dimension() const { return m_dim; }

	protected:
		virtual void run() override;
		virtual bool onRequestFinishedSuccessfully();

	private:
		IRequest* m_request;

		ParamAttribute m_attr;
		ParamDimension m_dim;
	};
} // namespace