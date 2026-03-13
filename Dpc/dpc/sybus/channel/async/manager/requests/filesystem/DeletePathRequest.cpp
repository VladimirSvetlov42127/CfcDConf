#include "DeletePathRequest.h"

namespace {
} // namespace

namespace Dpc::Sybus
{
	DeletePathRequest::DeletePathRequest(const QString& path, QObject* parent) : IFileSystemCtrlRequest(parent),
		m_request(nullptr),
		m_path(path)
	{
	}

	QString DeletePathRequest::name() const
	{
		return QString("Удаление '%1'").arg(path());
	}

	void DeletePathRequest::run()
	{
		//addInfoMsg(QString("Удаление '%1'").arg(path()));
		
		m_request = new FileSystemCommandRequest(QString("DELF %1").arg(path()), this),
		connect(m_request, &IRequest::finished, this, &DeletePathRequest::onRequestFinished);
		exec(m_request);
	}

	void DeletePathRequest::onRequestFinished()
	{
		m_request->deleteLater();

		if (m_request->error()) {
			setError(m_request->error(), m_request->errorString());
			finish();
			return;
		}

		addInfoMsg(QString("%1: %2").arg(name()).arg(successString()));
		finish();
	}
} // namespace