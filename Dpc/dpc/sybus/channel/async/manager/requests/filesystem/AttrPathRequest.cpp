#include "AttrPathRequest.h"

#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.h>
#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemResponseRequest.h>

namespace {
} // namespace

namespace Dpc::Sybus
{
	AttrPathRequest::AttrPathRequest(const QString& path, QObject* parent) :
		IFileSystemCtrlRequest(parent),
		m_request(nullptr),
		m_path(path),
		m_size(0)
	{
	}

	QString AttrPathRequest::name() const
	{
		return QString("Атрибуты '%1'").arg(path());
	}

	void AttrPathRequest::run()
	{
		//addInfoMsg(QString("Получение атрибутов '%1'").arg(path()));		
		m_attr.clear();
		m_size = 0;

		m_request = new FileSystemCommandRequest(QString("FATR %1").arg(path()), this);
		connect(m_request, &IRequest::finished, this, &AttrPathRequest::onRequestFinished);
		exec(m_request);
	}
	
	void AttrPathRequest::onRequestFinished()
	{		
		m_request->deleteLater();

		if (m_request->error()) {
			setError(m_request->error(), m_request->errorString());
			finish();
			return;
		}

		if (auto req = dynamic_cast<FileSystemCommandRequest*>(m_request); req) {
			m_request = new FileSystemResponseRequest(this);
			connect(m_request, &IRequest::finished, this, &AttrPathRequest::onRequestFinished);
			exec(m_request);
			return;
		}

		auto r = static_cast<FileSystemResponseRequest*>(m_request);
		m_attr = r->response();

		const QString beginStr = "SIZE=";
		const QString endStr = " kB";
		int beginIdx = attr().indexOf(beginStr) + beginStr.size();
		int endIdx = attr().indexOf(endStr);
		auto valList = attr().mid(beginIdx, endIdx - beginIdx).split('.');
		m_size = valList.value(0).toUInt() * 1000 + valList.value(1).toUInt();

		addInfoMsg(QString("%1: size(%2): %3").arg(name()).arg(size()).arg(attr()));
		finish();
	}

} // namespace