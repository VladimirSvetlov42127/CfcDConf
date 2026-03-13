#include "IMetadataRequest.h"

namespace {
} // namespace

namespace Dpc::Sybus
{
	IMetadataRequest::IMetadataRequest(const QString& title, uint16_t addr, QObject* parent) :
		IRequest(parent),
		m_currentRequest(nullptr),
		m_title(title),
		m_addr(addr),
		m_board(-1)
	{
	}

	IMetadataRequest::IMetadataRequest(const QString& title, uint16_t addr, int board, QObject* parent) :
		IRequest(parent),
		m_currentRequest(nullptr),
		m_title(title),
		m_addr(addr),
		m_board(board)
	{
	}

	QString IMetadataRequest::name() const
	{
		return QString("%1 %2").arg(title()).arg(subject());
	}

	void IMetadataRequest::runRequest(IRequest* req)
	{
		m_currentRequest = req;
		connect(m_currentRequest, &IRequest::finished, this, &IMetadataRequest::onCurrentRequestFinished, Qt::UniqueConnection);
		exec(m_currentRequest);
	}

	QString IMetadataRequest::subject() const
	{
		auto res = toHex(addr());
		if (board() > -1)
			res.append(QString(", Board(%1)").arg(board()));

		return res;
	}

	void IMetadataRequest::onCurrentRequestFinished()
	{
		m_currentRequest->deleteLater();
		if (m_currentRequest->error()) {
			setError(m_currentRequest->error(), m_currentRequest->errorString());
			finish();
			return;
		}

		if (!onRequestFinishedSuccessfully())
			return;
		
		finish();
	}

} // namespace