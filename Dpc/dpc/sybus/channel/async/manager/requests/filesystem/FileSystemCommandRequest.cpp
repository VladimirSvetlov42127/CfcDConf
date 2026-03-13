#include "FileSystemCommandRequest.h"

#include <dpc/sybus/channel/async/manager/requests/WriteParamRequest.h>
#include <dpc/sybus/smparlist.h>

namespace {
} // namespace

namespace Dpc::Sybus
{
	FileSystemCommandRequest::FileSystemCommandRequest(const QString& command, QObject* parent) :
		IFileSystemCtrlRequest(parent),
		m_request(nullptr),
		m_command(command)
	{
	}

	QString FileSystemCommandRequest::name() const
	{
		return QString("Команда к файловой системе '%1'").arg(command());
	}

	void FileSystemCommandRequest::run()
	{
		//addInfoMsg(QString("Команда к файловой системе '%1'").arg(command()));
		
		auto p = ParamPack::create(T_STRING);
		p->appendValue(command());

		m_request = new WriteParamRequest(p, SP_FILE_CTRLW, 0, this);
		connect(m_request, &IRequest::finished, this, &FileSystemCommandRequest::onRequestFinished);
		exec(m_request);
	}

	void FileSystemCommandRequest::onRequestFinished()
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