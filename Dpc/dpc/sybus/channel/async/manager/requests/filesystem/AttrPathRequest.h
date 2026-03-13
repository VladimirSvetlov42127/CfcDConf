#pragma once

#include <dpc/sybus/channel/async/manager/requests/filesystem/IFileSystemRequest.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT AttrPathRequest : public IFileSystemCtrlRequest
	{
		Q_OBJECT

	public:
		AttrPathRequest(const QString& path, QObject* parent = nullptr);

		QString path() const { return m_path; }

		QString attr() const { return m_attr; }
		size_t size() const { return m_size; }

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:
		void onRequestFinished();

	private:
		IRequest* m_request;
		QString m_path;
		QString m_attr;
		size_t m_size;
	};
} // namespace