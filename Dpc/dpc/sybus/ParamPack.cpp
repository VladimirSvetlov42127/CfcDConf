#include "ParamPack.h"

#include <qvariant.h>
#include <qtextcodec.h>

namespace {

const QTextCodec* g_Win1251Codec() {
    static QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
    return codec;
}

} // namespace

namespace Dpc::Sybus
{
	//================================================================================
	IpAddress::IpAddress(const QString & ip)
	{
		auto list = ip.split('.');
		for (size_t i = 0; i < sizeof(byte); i++)
			byte[i] = list.value(i).toUInt();
	}

	QString IpAddress::toString() const
	{
		return QString("%1.%2.%3.%4").arg(byte[0]).arg(byte[1]).arg(byte[2]).arg(byte[3]);
	}

	//================================================================================
	MacAddress::MacAddress(const QString & mac)
	{
		auto list = mac.split(':');
		for (size_t i = 0; i < sizeof(byte); i++)
			byte[i] = list.value(i).toUInt(nullptr, 16);
	}

	QString MacAddress::toString() const
	{
		return QString(QByteArray((char*)&byte, sizeof(byte)).toHex(':').toUpper());
	}

	//================================================================================
	ParamPack::ParamPack(uint8_t typeId) :
		m_typeId(typeId), m_addr(0), m_index(0)
	{
	}

	ParamPack::ParamPack(uint8_t typeId, uint16_t addr, uint16_t index) :
		m_typeId(typeId), m_addr(addr), m_index(index)
	{
	}

	void ParamPack::appendData(const QByteArray& data)
	{
		m_data.append(data);
		if (T_STRING == typeId() && m_data.back() != char(0))
			m_data.append(char(0));
	}

	size_t ParamPack::count() const
	{
		if (typeId() == T_STRING)
			return m_data.count(char(0));

		if (!typeSize())
			return 0;

		return m_data.size() / typeSize();
	}

	QByteArray ParamPack::data(int idx) const
	{
		if (idx < 0)
			return m_data;

		if (idx >= count())
			return QByteArray();

		if (typeId() == T_STRING)
			return m_data.split(char(0)).value(idx).append(char(0));

		return QByteArray(m_data.begin() + idx * typeSize(), typeSize());
	}

	bool ParamPack::appendValue(const IpAddress& value)
	{
		if (typeId() != T_IP4ADR)
			return false;

		return append(value);
	}

	bool ParamPack::appendValue(const MacAddress& value)
	{
		if (typeId() != T_OUI)
			return false;

		return append(value);
	}

	bool ParamPack::appendValue(const QString& value)
	{
		switch (typeId())
		{
		case T_BYTE:
		case T_8BIT:
			return append(QVariant(value).value<uint8_t>());
		case T_BYTEBOOL:
		case T_BOOL:
			return append(QVariant(value).value<bool>());
		case T_CHAR:
		case T_SBYTE:
			return append(QVariant(value).value<int8_t>());
		case T_SHORT:
			return append(QVariant(value).value<int16_t>());
		case T_WORD:
		case T_16BIT:
			return append(QVariant(value).value<uint16_t>());
		case T_INTEGER:
			return append(QVariant(value).value<int32_t>());
		case T_DWORD:
		case T_32BIT:
		case T_DATETIME:
			return append(QVariant(value).value<uint32_t>());
		case T_FLOAT:
			return append(QVariant(value).value<float>());
		case T_INT64:
			return append(QVariant(value).value<int64_t>());
		case T_UTCTIME:
		case T_UINT64:
		case T_TIME61850:
			return append(QVariant(value).value<uint64_t>());
		case T_IP4ADR:
			return append(IpAddress(value));
		case T_OUI:
			return append(MacAddress(value));
		case T_STRING:
			return append(value);
		default:
			return false;
		}
	}

	QString ParamPack::toString() const
	{
		QStringList list;
		for (size_t i = 0; i < count(); i++)
			list.append(value<QString>(i));

        return list.join(", ");
    }

    QString ParamPack::toUnicode(const QByteArray &win1251_data)
    {
        return g_Win1251Codec()->toUnicode(win1251_data);
    }

    QByteArray ParamPack::toWin1251(const QString &unicode_string)
    {
        return g_Win1251Codec()->fromUnicode(unicode_string);
    }

	uint8_t ParamPack::typeSize() const
	{
		switch (typeId())
		{
		case T_BYTE:
		case T_8BIT:
		case T_CHAR:
		case T_BYTEBOOL:
		case T_BOOL:
		case T_SBYTE:
			return 1;
		case T_SHORT:
		case T_WORD:
		case T_16BIT:
			return 2;
		case T_INTEGER:
		case T_DWORD:
		case T_32BIT:
		case T_FLOAT:
		case T_DATETIME:
		case T_IP4ADR:
			return 4;
		case T_OUI:
		case T_STRICT:
			return 6;
		case T_UTCTIME:
		case T_UINT64:
		case T_INT64:
		case T_TIME61850:
			return 8;
		default:
			return 0;
		}
	}

	bool ParamPack::append(const QString& value)
	{
        m_data.append(toWin1251(value));
		m_data.append(char(0));
		return true;
	}
} // namespace
