#include "ListPathRequest.h"

#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.h>
#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemResponseRequest.h>

namespace {
	const QString NEXT_LIST_ITEM_TAG = "LIST>>";
} // namespace

namespace Dpc::Sybus
{
	ListPathRequest::ListPathRequest(const QString& path, QObject* parent) :
		IFileSystemCtrlRequest(parent),
		m_request(nullptr),
		m_path(path)
	{
	}

	QString ListPathRequest::name() const
	{
		return QString("Список файлов и папок '%1'").arg(path());
	}

	void ListPathRequest::run()
	{
		//addInfoMsg(QString("Получение списка файлов и папок '%1'").arg(path()));		
		m_list.clear();

		m_request = new FileSystemCommandRequest(QString("LIST %1").arg(path()), this);
		connect(m_request, &IRequest::finished, this, &ListPathRequest::onRequestFinished);
		exec(m_request);
	}
	
	void ListPathRequest::onRequestFinished()
	{
		
		m_request->deleteLater();

		if (m_request->error()) {
			setError(m_request->error(), m_request->errorString());
			finish();
			return;
		}

		if (auto req = dynamic_cast<FileSystemCommandRequest*>(m_request); req) {
			startResponseRequest();
			return;
		}

		auto req = static_cast<FileSystemResponseRequest*>(m_request);
		auto nextLine = req->response();
		if (nextLine.contains(NEXT_LIST_ITEM_TAG)) {
			auto item = nextLine.split(NEXT_LIST_ITEM_TAG).value(1).trimmed();
			if (!item.isEmpty()) {
				if (item != "./" && item != "../")
					m_list << item;

				startResponseRequest();
				return;
			}			
		}

		addInfoMsg(QString("%1: %2").arg(name()).arg(list().join(", ")));
		finish();
	}

	void ListPathRequest::startResponseRequest()
	{
		m_request = new FileSystemResponseRequest(this);
		connect(m_request, &IRequest::finished, this, &ListPathRequest::onRequestFinished);
		exec(m_request);
	}
} // namespace