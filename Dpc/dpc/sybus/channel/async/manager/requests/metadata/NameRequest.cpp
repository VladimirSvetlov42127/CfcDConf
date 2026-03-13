#include "NameRequest.h"

#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
	const QString TITLE = "Имена";
	constexpr uint16_t MAX_COUNT = std::numeric_limits<uint16_t>::max();
} // namespace

namespace Dpc::Sybus
{
	NameRequest::NameRequest(uint16_t addr, QObject* parent) :
		IMetadataRequest(TITLE, addr, parent),
		m_request(nullptr),
		m_index(0),
		m_count(MAX_COUNT)
	{
	}

	NameRequest::NameRequest(uint16_t addr, uint16_t index, QObject* parent) :
		IMetadataRequest(TITLE, addr, parent),
		m_request(nullptr),
		m_index(index),
		m_count(MAX_COUNT)
	{
	}

	NameRequest::NameRequest(uint16_t addr, uint16_t index, int count, QObject* parent) :
		IMetadataRequest(TITLE, addr, parent),
		m_request(nullptr),
		m_index(index),
		m_count(count < 0 ? MAX_COUNT : count)
	{
	}

	NameRequest::NameRequest(uint16_t addr, uint16_t index, int count, int board, QObject* parent) :
		IMetadataRequest(TITLE, addr, board, parent),
		m_request(nullptr),
		m_index(index),
		m_count(count < 0 ? MAX_COUNT : count)
	{
	}

	void NameRequest::run()
	{
		m_pack.reset();

		m_request = new ReadParamRequest(SP_PARNAME, index(), count(), board(), addr(), this);		
		runRequest(m_request);
	}

	bool NameRequest::onRequestFinishedSuccessfully()
	{
		m_pack = m_request->pack();
		addInfoMsg(QString("%1: %2(%3)").arg(name()).arg(m_pack->count()).arg(m_pack->toString()));
		return true;
	}

	QString NameRequest::subject() const
	{
		auto result = toHex(addr());
		result.append(QString("[%1-%2]").arg(index()).arg(index() + count() - 1));
		if (board() > -1)
			result.append(QString(", Board(%1)").arg(board()));

		return result;
	}
} // namespace