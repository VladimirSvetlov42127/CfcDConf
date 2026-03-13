#include "SizeRequest.h"

#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
	const QString TITLE = "Размер(байты)";
} // namespace

namespace Dpc::Sybus
{
	SizeRequest::SizeRequest(uint16_t addr, QObject* parent) :
		IMetadataRequest(TITLE, addr, parent),
		m_request(nullptr),
		m_size(0)
	{
	}

	SizeRequest::SizeRequest(uint16_t addr, int board, QObject* parent) :
		IMetadataRequest(TITLE, addr, board, parent),
		m_request(nullptr)
	{
	}

	void SizeRequest::run()
	{
		m_size = 0;

		m_request = new ReadParamRequest(SP_PARSIZE, addr(), 1, board(), this);
		runRequest(m_request);
	}

	bool SizeRequest::onRequestFinishedSuccessfully()
	{
		m_size = m_request->pack()->value<uint16_t>();
		addInfoMsg(QString("%1: %2").arg(name()).arg(m_size));
		return true;
	}
} // namespace