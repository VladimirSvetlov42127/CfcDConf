#include "UploadFileRequest.h"

#include <qfileinfo.h>

#include <dpc/sybus/channel/async/manager/requests/filesystem/AttrPathRequest.h>
#include <dpc/sybus/channel/async/manager/requests/WriteParamRequest.h>

namespace {
} // namespace

namespace Dpc::Sybus
{
	UploadFileRequest::UploadFileRequest(const QString& sourceFileName, const QString& destinationFileName, QObject* parent) :
		IFileSystemDataRequest(parent),
		m_request(nullptr),
		m_sourceFileName(sourceFileName),
		m_destinationFileName(destinationFileName),
		m_file(new QFile(sourceFileName, this)),
		m_fileSize(0),
		m_bufferSize(0x40),
		m_writtenSize(0)
	{
	}

	QString UploadFileRequest::name() const
	{
		return QString("Запись файла '%1' в '%2'").arg(sourceFileName()).arg(destinationFileName());
	}

	void UploadFileRequest::run()
	{
		m_writtenSize = 0;
		m_bufferSize = std::min(manager()->writeBufferSize(), manager()->fileDataBufferSize());

		if (!file()->open(QIODevice::ReadOnly)) {
			setError(file()->error(), file()->errorString(), QString("Не удалось открыть файл '%1' на чтение").arg(sourceFileName()));
			close();
			return;
		}

		m_fileSize = QFileInfo(*file()).size();
		open(destinationFileName(), ReadWriteMode, [=] { writeNextData(); });
	}	

	void UploadFileRequest::writeNextData()
	{
		auto fileData = file()->read(bufferSize());
		if (fileData.isEmpty()) {
			if (QFile::NoError != file()->error()) {
				setError(file()->error(), file()->errorString(), QString("Не удалось прочитать данные из файл '%1'").arg(sourceFileName()));
			}
			else {
				addInfoMsg(QString("%1: %2").arg(name()).arg(successString()));
			}

			file()->close();
			close();
			return;
		}

		auto p = ParamPack::create(T_BYTE);
		p->appendData(fileData);

		auto req = new WriteParamRequest(p, SP_FILE_DATA, 0, this);
		connect(req, &IRequest::finished, this, [=] {
			req->deleteLater();			
			if (req->error()) {
				// обработать ситуации, когда идет ошибка busy
				/*if (ChannelReply::Busy == req->error()) {

				}*/

				file()->close();
				setError(req->error(), req->errorString());
				close();
				return;
			}

			m_writtenSize += fileData.size();
			addProgressValue(static_cast<double>(writtenSize()) / fileSize() * 100);
			writeNextData();
		});

		m_request = req;
		manager()->exec(m_request, false);
	}
} // namespace