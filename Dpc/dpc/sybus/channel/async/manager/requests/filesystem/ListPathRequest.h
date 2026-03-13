#pragma once

#include <dpc/sybus/channel/async/manager/requests/filesystem/IFileSystemRequest.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT ListPathRequest : public IFileSystemCtrlRequest
	{
		Q_OBJECT

	public:
		ListPathRequest(const QString& path, QObject* parent = nullptr);

		QString path() const { return m_path; }
		QStringList list() const { return m_list; }

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:
		void onRequestFinished();

	private:
		void startResponseRequest();

	private:
		IRequest* m_request;
		QString m_path;
		QStringList m_list;
	};
} // namespace