#pragma once

#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.h>

class QFile;

namespace Dpc::Sybus
{
	class DPC_EXPORT UploadFileRequest : public IFileSystemDataRequest
	{
		Q_OBJECT

	public:
		UploadFileRequest(const QString& sourceFileName, const QString& destinationFileName, QObject* parent = nullptr);

		QString sourceFileName() const { return m_sourceFileName; }
		QString destinationFileName() const { return m_destinationFileName; }

		size_t fileSize() const { return m_fileSize; }
		size_t writtenSize() const { return m_writtenSize; }

		virtual QString name() const override;

	signals:

	protected:
		virtual void run() override;

	private:
		uint16_t bufferSize() const { return m_bufferSize; }
		QFile* file() const { return m_file; }

		void writeNextData();

	private:
		IRequest* m_request;
		QString m_sourceFileName;
		QString m_destinationFileName;

		QFile* m_file;
		size_t m_fileSize;
		uint16_t m_bufferSize;
		size_t m_writtenSize;
	};
} // namespace