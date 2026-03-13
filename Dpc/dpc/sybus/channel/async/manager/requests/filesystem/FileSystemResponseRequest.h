#pragma once

#include <dpc/sybus/channel/async/manager/requests/filesystem/IFileSystemRequest.h>

namespace Dpc::Sybus
{
	class ReadParamRequest;

	class DPC_EXPORT FileSystemResponseRequest : public IFileSystemCtrlRequest
	{
		Q_OBJECT

	public:
		FileSystemResponseRequest(QObject* parent = nullptr);

		QString response() const { return m_response; }

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:
		void onRequestFinished();

	private:
		ReadParamRequest* m_request;
		QString m_response;
	};
} // namespace