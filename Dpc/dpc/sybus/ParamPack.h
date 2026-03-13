#pragma once

#include <memory>
#include <qvariant.h>
#include <qdebug.h>

#include <dpc/dpc_global.h>
#include <dpc/sybus/smparlist.h>

namespace Dpc::Sybus
{
	//================================================================================
	struct DPC_EXPORT IpAddress
	{
		uint8_t byte[4] = {};

		IpAddress() = default;
		IpAddress(const QString &ip);
		QString toString() const;
	};

	//================================================================================
	struct DPC_EXPORT MacAddress
	{
		uint8_t byte[6] = {};

		MacAddress() = default;
		MacAddress(const QString &mac);
		QString toString() const;
	};

	//================================================================================
	class ParamPack;
	using ParamPackPtr = std::shared_ptr<ParamPack>;

	class DPC_EXPORT ParamPack
	{
	public:
		template<typename ...Args>
		static ParamPackPtr create(Args ...args) { return std::make_shared<ParamPack>(args...); }

		ParamPack(uint8_t typeId);
		ParamPack(uint8_t typeId, uint16_t addr, uint16_t index = 0);

		uint8_t typeId() const { return m_typeId; }
		uint16_t addr() const { return m_addr; }
		uint16_t index() const { return m_index; }

		// добавить массив данных в пакет
		void appendData(const QByteArray& data);

		// количество элементов в пакете. считается в зависимости от массива данных и типа элементов
		size_t count() const;

		// массив данных элемента idx, при idx = -1, возвращается массив данных всего пакета
		QByteArray data(int idx = -1) const;

		// Добавить занчение в пакет
		template<typename T>
		bool appendValue(const T &value)
		{
			switch (typeId())
			{
			case T_BYTE:
			case T_8BIT:
			case T_BYTEBOOL:
			case T_BOOL:
				return append(uint8_t(value));
			case T_CHAR:
			case T_SBYTE:
				return append(int8_t(value));
			case T_SHORT:
				return append(int16_t(value));
			case T_WORD:
			case T_16BIT:
				return append(uint16_t(value));
			case T_INTEGER:
				return append(int32_t(value));
			case T_DWORD:
			case T_32BIT:
			case T_DATETIME:
				return append(uint32_t(value));
			case T_FLOAT:
				return append(float(value));
			case T_INT64:
				return append(int64_t(value));
			case T_UTCTIME:
			case T_UINT64:
			case T_TIME61850:
				return append(uint64_t(value));
			default:
				return false;
			}
		}

		// Перегрузка для IpAddress
		bool appendValue(const IpAddress& value);

		// Перегрузка для MacAddress
		bool appendValue(const MacAddress& value);

		// Перегрузка для QString. Если тип данных не строка, то происходит попытка перевода в нужный тип через QVariant
		bool appendValue(const QString& value);

		// Получить значение типа T под индексом idx из пакета, при ошибке или неверном индексе возвращается defaultValue
		template<typename T>
		T value(size_t idx = 0, const T &defaultValue = T()) const
		{
			size_t offset = idx * sizeof(T);
			if (!typeId() || offset + sizeof(T) > m_data.size())
				return defaultValue;

			T val = T();
			memcpy(&val, m_data.data() + offset, sizeof(T));
			return val;
		}

		// вывести все элементы массива в строке
        QString toString() const;

	private:
        // Переводит массив байт закодированых Windows-1251, в строку с кодировкой Unicode
        static QString toUnicode(const QByteArray &win1251_data);

        // Переводит строку с кодировкой Unicode в массив байт кодировки Windows-1251
        static QByteArray toWin1251(const QString &unicode_string);

        // Размер типа данных в байтах
		uint8_t typeSize() const;

		template<typename T>
		bool append(const T &value) 
		{
			m_data.append((const char*)&value, sizeof(T)); 
			return true; 
		}

		bool append(const QString& value);

	private:
		uint8_t m_typeId;
		uint16_t m_addr;
		uint16_t m_index;
		QByteArray m_data;
	};


	// Перегрузка для QString. Если тип данных не строка, возвращается строковое представление данных
	// (Данная функция вынесена из тела класса, для избежания ошибки 'explicit specialization in non-namespace scope' на компиляторах GCC/Mingw)
	template<> inline
	QString ParamPack::value<QString>(size_t idx, const QString& defaultValue) const
	{
		if (idx >= count())
			return defaultValue;

		switch (typeId())
		{
		case T_BYTE:
		case T_8BIT:
			return QString::number(value<uint8_t>(idx));
		case T_BYTEBOOL:
		case T_BOOL:
			return QString::number(value<bool>(idx));
		case T_CHAR:
		case T_SBYTE:
			return QString::number(value<int8_t>(idx));
		case T_SHORT:
			return QString::number(value<int16_t>(idx));
		case T_WORD:
		case T_16BIT:
			return QString::number(value<uint16_t>(idx));
		case T_INTEGER:
			return QString::number(value<int32_t>(idx));
		case T_DWORD:
		case T_32BIT:
		case T_DATETIME:
			return QString::number(value<uint32_t>(idx));
		case T_FLOAT:
			return QString::number(value<float>(idx));
		case T_INT64:
			return QString::number(value<int64_t>(idx));
		case T_UTCTIME:
		case T_UINT64:
		case T_TIME61850:
			return QString::number(value<uint64_t>(idx));
		case T_IP4ADR:
			return value<IpAddress>(idx).toString();
		case T_OUI:
			return value<MacAddress>(idx).toString();
        case T_STRING:
            return toUnicode(m_data.split(char(0)).value(idx).trimmed());

		default:
			return defaultValue;
		}
	}
} // namespace
