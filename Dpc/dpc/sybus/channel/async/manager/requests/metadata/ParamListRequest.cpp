#include "ParamListRequest.h"

#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
	const QString TITLE = "Список параметров";
} // namespace

namespace Dpc::Sybus
{
	ParamListRequest::ParamListRequest(QObject* parent) :
		IMetadataRequest(TITLE, 0, parent),
		m_request(nullptr)
	{
	}

	ParamListRequest::ParamListRequest(int board, QObject* parent) :
		IMetadataRequest(TITLE, 0, board, parent),
		m_request(nullptr)
	{
	}

	void ParamListRequest::run()
	{
		m_list.clear();

		m_request = new ReadParamRequest(SP_PARLIST, 0, std::numeric_limits<uint16_t>::max(), board(), this);
		runRequest(m_request);
	}

	bool ParamListRequest::onRequestFinishedSuccessfully()
	{
		QString valueStr;
		auto pack = m_request->pack();
		for (size_t i = 0; i < pack->count(); i++) {
			auto value = pack->value<uint16_t>(i);
			m_list << value;

			if (i)
				valueStr.append(", ");
			valueStr.append(toHex(value));
		}

		addInfoMsg(QString("%1: %2(%3)").arg(name()).arg(m_list.count()).arg(valueStr));
		return true;
	}
	QString ParamListRequest::subject() const
	{
		QString res;
		if (board() > -1)
			res.append(QString(" Board(%1)").arg(board()));

		return res;
	}
} // namespace