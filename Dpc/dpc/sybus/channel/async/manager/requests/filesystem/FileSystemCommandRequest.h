#pragma once

#include <dpc/sybus/channel/async/manager/requests/filesystem/IFileSystemRequest.h>

namespace Dpc::Sybus
{
	class WriteParamRequest;

	class DPC_EXPORT FileSystemCommandRequest : public IFileSystemCtrlRequest
	{
		Q_OBJECT

	public:
		FileSystemCommandRequest(const QString& command, QObject* parent = nullptr);

		QString command() const { return m_command; }

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:
		void onRequestFinished();

	private:
		WriteParamRequest* m_request;
		QString m_command;
	};
} // namespace