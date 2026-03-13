#include "FileSystemResponseRequest.h"

#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
} // namespace

namespace Dpc::Sybus
{
	FileSystemResponseRequest::FileSystemResponseRequest(QObject* parent) :
		IFileSystemCtrlRequest(parent),
		m_request(nullptr)
	{
	}

	QString FileSystemResponseRequest::name() const
	{
		return QString("Чтение овтета на команду к файловой системе");
	}

	void FileSystemResponseRequest::run()
	{
		//addInfoMsg(QString("Получение овтета на команду к файловой системе"));
		m_response = QString();
		
		m_request = new ReadParamRequest(SP_FILE_CTRLR, this);
		connect(m_request, &IRequest::finished, this, &FileSystemResponseRequest::onRequestFinished);
		exec(m_request);
	}

	void FileSystemResponseRequest::onRequestFinished()
	{	
		
		m_request->deleteLater();

		if (m_request->error()) {
			setError(m_request->error(), m_request->errorString());
			finish();
			return;
		}

		m_response = m_request->pack()->value<QString>();
		addInfoMsg(QString("%1: %2").arg(name()).arg(response()));
		finish();
	}
} // namespace