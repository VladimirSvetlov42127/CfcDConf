#pragma once

#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT MakeDirRequest : public IFileSystemCtrlRequest
	{
		Q_OBJECT

	public:
		MakeDirRequest(const QString& path, QObject* parent = nullptr);

		QString path() const { return m_path; }

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:
		void onRequestFinished();

	private:
		FileSystemCommandRequest* m_request;
		QString m_path;
	};
} // namespace