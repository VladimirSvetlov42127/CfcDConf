#pragma once

#include <dpc/sybus/channel/async/manager/IRequest.h>

namespace Dpc::Sybus
{
	// Базовый класс для команд к файловой системе(создание директории, удаление пути, список файов, аттрибуты файла)
	class DPC_EXPORT IFileSystemCtrlRequest : public IRequest
	{
		Q_OBJECT

	public:
		IFileSystemCtrlRequest(QObject* parent = nullptr) : IRequest(parent) {}
	};

	// Базовый класс для чтения и записи содержимого файла
	class DPC_EXPORT IFileSystemDataRequest : public IRequest
	{
		Q_OBJECT

	public:
		enum OpenMode {
			ReadOnlyMode,
			WriteOnlyMode,
			ReadWriteMode,
		};

		IFileSystemDataRequest(QObject* parent = nullptr);	

	protected:
		void open(const QString& fileName, OpenMode mode, std::function<void()> callback);
		void close();

		void setDataOffset(uint32_t offset, std::function<void()> callback);

	private:
		QString fileName() const { return m_fileName; }

	private:
		IRequest* m_request;
		bool m_isFileOpened;
		QString m_fileName;
	};
} // namespace