#include "MakeDirRequest.h"

namespace {
} // namespace

namespace Dpc::Sybus
{
	MakeDirRequest::MakeDirRequest(const QString& path, QObject* parent) :
		IFileSystemCtrlRequest(parent),
		m_request(nullptr),
		m_path(path)
	{
	}

	QString MakeDirRequest::name() const
	{
		return QString("Создание директории '%1'").arg(path());
	}

	void MakeDirRequest::run()
	{
		//addInfoMsg(QString("Создание директории '%1'").arg(path()));
		
		m_request = new FileSystemCommandRequest(QString("MKDR %1").arg(path()), this);
		connect(m_request, &IRequest::finished, this, &MakeDirRequest::onRequestFinished);
		exec(m_request);
	}

	void MakeDirRequest::onRequestFinished()
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