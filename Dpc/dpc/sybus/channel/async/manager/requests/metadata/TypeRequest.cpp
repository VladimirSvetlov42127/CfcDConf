#include "TypeRequest.h"

#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
	const QString TITLE = "Тип";
} // namespace

namespace Dpc::Sybus
{
	TypeRequest::TypeRequest(uint16_t addr, QObject* parent) :
		IMetadataRequest(TITLE, addr, parent),
		m_request(nullptr),
		m_type(0)
	{
	}

	TypeRequest::TypeRequest(uint16_t addr, int board, QObject* parent) :
		IMetadataRequest(TITLE, addr, board, parent),
		m_request(nullptr)
	{
	}

	void TypeRequest::run()
	{
		m_type = 0;

		m_request = new ReadParamRequest(SP_PARTYPE, addr(), 1, board(), this);
		runRequest(m_request);
	}

	bool TypeRequest::onRequestFinishedSuccessfully()
	{
		m_type = m_request->pack()->value<uint8_t>();
		addInfoMsg(QString("%1: %2").arg(name()).arg(toHex(m_type)));
		return true;
	}
} // namespace