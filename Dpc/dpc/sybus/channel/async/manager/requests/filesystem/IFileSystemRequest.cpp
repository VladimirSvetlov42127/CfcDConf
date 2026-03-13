#include "IFileSystemRequest.h"

#include <qfile.h>

#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.h>
#include <dpc/sybus/channel/async/manager/requests/WriteParamRequest.h>

namespace {
	using namespace Dpc::Sybus;
	QString modeToString(IFileSystemDataRequest::OpenMode mode) 
	{
		switch (mode)
		{
		case Dpc::Sybus::IFileSystemDataRequest::ReadOnlyMode: return "RO";
		case Dpc::Sybus::IFileSystemDataRequest::WriteOnlyMode: return "WO";
		case Dpc::Sybus::IFileSystemDataRequest::ReadWriteMode: return "RW";
		default: return QString();
		}
	}

	QString actionString(IFileSystemDataRequest::OpenMode mode)
	{
		switch (mode)
		{
		case Dpc::Sybus::IFileSystemDataRequest::ReadOnlyMode: return "чтение";
		default: return "запись";
		}
	}
} // namespace

namespace Dpc::Sybus
{
	IFileSystemDataRequest::IFileSystemDataRequest(QObject* parent) :
		IRequest(parent),
		m_request(nullptr),
		m_isFileOpened(false)
	{
	}

	void IFileSystemDataRequest::open(const QString& fileName, OpenMode mode, std::function<void()> callback)
	{
		m_isFileOpened = false;
		m_fileName = fileName;
		auto prefix = QString("Открытие файла '%1' на %2").arg(fileName).arg(actionString(mode));

		m_request = new FileSystemCommandRequest(QString("OPFL %1 %2").arg(fileName).arg(modeToString(mode)), this);
		connect(m_request, &IRequest::finished, this, [=] {
			m_request->deleteLater();
			if (m_request->error()) {
				setError(m_request->error(), m_request->errorString(), prefix);
				close();
				return;
			}

			m_isFileOpened = true;
			addInfoMsg(QString("%1: %2").arg(prefix).arg(successString()));

			callback();
		});

		manager()->exec(m_request, false);
	}

	void IFileSystemDataRequest::close()
	{
		if (!m_isFileOpened) {
			finish();
			return;
		}

		auto prefix = QString("Закрытие файла '%1'").arg(fileName());
		m_request = new FileSystemCommandRequest(QString("CLFL %1").arg(fileName()), this);
		connect(m_request, &IRequest::finished, this, [=] {
			m_request->deleteLater();
			if (m_request->error()) {
				addErrorMsg(QString("%1: %2").arg(prefix).arg(errorFormated()));
			}
			else {
				addInfoMsg(QString("%1: %2").arg(prefix).arg(successString()));
			}

			finish();
		});

		manager()->exec(m_request, false);
	}

	void IFileSystemDataRequest::setDataOffset(uint32_t offset, std::function<void()> callback)
	{
		auto prefix = QString("Установка указателя данных файла '%1' на позицию %2").arg(fileName()).arg(offset);

		auto p = ParamPack::create(T_DWORD);
		p->appendValue(offset);

		m_request = new WriteParamRequest(p, SP_FILE_DATAOFFS, 0, this);
		connect(m_request, &IRequest::finished, this, [=] {
			m_request->deleteLater();
			if (m_request->error()) {
				setError(m_request->error(), m_request->errorString(), prefix);
				close();
				return;
			}

			addInfoMsg(QString("%1: %2").arg(prefix).arg(successString()));
			callback();
		});

		manager()->exec(m_request, false);
	}

} // namespace