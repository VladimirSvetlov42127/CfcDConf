#include "DownloadFileRequest.h"

#include <qfile.h>

#include <dpc/sybus/channel/async/manager/requests/filesystem/AttrPathRequest.h>
#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>

namespace {
} // namespace

namespace Dpc::Sybus
{
	DownloadFileRequest::DownloadFileRequest(const QString& sourceFileName, const QString& destinationFileName, QObject* parent) :
		IFileSystemDataRequest(parent),
		m_request(nullptr),
		m_sourceFileName(sourceFileName),
		m_destinationFileName(destinationFileName),
		m_file(new QFile(destinationFileName, this)),
		m_fileSize(0),
		m_bufferSize(0x40),
		m_readedSize(0)
	{
	}

	QString DownloadFileRequest::name() const
	{
		return QString("Чтение файла '%1' в '%2'").arg(sourceFileName()).arg(destinationFileName());
	}

	void DownloadFileRequest::run()
	{
		m_fileSize = 0;
		m_readedSize = 0;
		m_bufferSize = std::min(manager()->readBufferSize(), manager()->fileDataBufferSize());

		open(sourceFileName(), ReadOnlyMode, [=] { getFileSize(); });
	}

	void DownloadFileRequest::getFileSize()
	{
		auto prefix = QString("Размер файла '%1'").arg(sourceFileName());

		auto req = new AttrPathRequest(sourceFileName(), this);		
		connect(req, &IRequest::finished, this, [=] {
			req->deleteLater();
			if (req->error()) {
				setError(req->error(), req->errorString(), prefix);
				close();
				return;
			}

			m_fileSize = req->size();
			addInfoMsg(QString("%1: %2 байт").arg(prefix).arg(fileSize()));
			setDataOffset(0, [=] { getNextData(); });
		});

		m_request = req;
		manager()->exec(m_request, false);
	}

	void DownloadFileRequest::getNextData()
	{
		if (!file()->isOpen() && !file()->open(QIODevice::WriteOnly)) {
			setError(file()->error(), file()->errorString(), QString("Не удалось открыть файл '%1' на запись").arg(destinationFileName()));
			close();
			return;
		}		

		size_t nextSize = fileSize() - readedSize() > bufferSize() ? bufferSize() : fileSize() - readedSize();
		auto req = new ReadParamRequest(SP_FILE_DATA, 0, nextSize, this);
		connect(req, &IRequest::finished, this, [=] {
			req->deleteLater();			
			if (req->error()) {

				// обработать ситуации, когда идет ошибка busy
				/*if (ChannelReply::Busy == req->error()) {

				}*/

				file()->remove();
				setError(req->error(), req->errorString());
				close();
				return;
			}

			auto pack = req->pack();
			auto wr = file()->write(pack->data());
			if (wr < 0 || wr != pack->data().count()) {
				file()->remove();
				setError(file()->error(), file()->errorString(), QString("Не удалось записать данные в файл '%1'").arg(destinationFileName()));
				close();
				return;
			}

			m_readedSize += wr;
			addProgressValue(static_cast<double>(readedSize()) / fileSize() * 100);
			if (readedSize() < fileSize()) {
				getNextData();
				return;
			}

			file()->close();
			addInfoMsg(QString("%1: %2").arg(name()).arg(successString()));			
			close();
		});

		m_request = req;
		manager()->exec(m_request, false);
	}
} // namespace