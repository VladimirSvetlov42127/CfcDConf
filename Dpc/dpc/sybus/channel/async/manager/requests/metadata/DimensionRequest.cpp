#include "DimensionRequest.h"

#include <dpc/sybus/channel/async/manager/requests/metadata/AttributeRequest.h>
#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
	const QString TITLE = "Размерность";
} // namespace

namespace Dpc::Sybus
{
	DimensionRequest::DimensionRequest(uint16_t addr, QObject* parent) : 
		IMetadataRequest(TITLE, addr, parent),
		m_request(nullptr)
	{
	}

	DimensionRequest::DimensionRequest(uint16_t addr, int board, QObject* parent) :
		IMetadataRequest(TITLE, addr, board, parent),
		m_request(nullptr)
	{
	}

	void DimensionRequest::run()
	{
		m_attr = ParamAttribute();
		m_dim = ParamDimension();	

		m_request = new AttributeRequest(addr(), board(), this);
		runRequest(m_request);
	}	

	bool DimensionRequest::onRequestFinishedSuccessfully()
	{
		if (auto attrReq = dynamic_cast<AttributeRequest*>(m_request); attrReq) {
			m_attr = attrReq->attribute();

			m_request = new ReadParamRequest(SP_PARDIM, addr(), 1, board(), this);
			runRequest(m_request);
			return false;
		}

		auto readParamReq = static_cast<ReadParamRequest*>(m_request);
		auto p = readParamReq->pack();
		uint8_t profileCount = m_attr[ParamAttribute::_2D] ? p->value<uint8_t>(1) : 1;
		uint16_t subProfileCount = m_attr[ParamAttribute::_2D] ? p->value<uint8_t>(0) : p->value<uint16_t>();
		m_dim = ParamDimension(profileCount, subProfileCount);

		addInfoMsg(QString("%1: %2").arg(name()).arg(m_dim.toString()));
		return true;
	}

} // namespace