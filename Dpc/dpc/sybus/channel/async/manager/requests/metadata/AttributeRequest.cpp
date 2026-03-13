#include "AttributeRequest.h"

#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
	const QString TITLE = "Атрибуты";
} // namespace

namespace Dpc::Sybus
{
	AttributeRequest::AttributeRequest(uint16_t addr, QObject* parent) :
		IMetadataRequest(TITLE, addr, parent),
		m_request(nullptr)
	{
	}

	AttributeRequest::AttributeRequest(uint16_t addr, int board, QObject* parent) :
		IMetadataRequest(TITLE, addr, board, parent),
		m_request(nullptr)
	{
	}

	void AttributeRequest::run()
	{
		m_attr = ParamAttribute();

		m_request = new ReadParamRequest(SP_PARATTR, addr(), 1, board(), this);		
		runRequest(m_request);
	}

	bool AttributeRequest::onRequestFinishedSuccessfully()
	{
		m_attr = ParamAttribute(m_request->pack()->value<uint16_t>());
		addInfoMsg(QString("%1: %2").arg(name()).arg(m_attr.toString()));
		return true;
	}
} // namespace