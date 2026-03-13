#include "byte_code.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
ByteCode::ByteCode() : QByteArray()
{
}

ByteCode::ByteCode(const char* data, qsizetype size) : QByteArray(data, size)
{
}

ByteCode::ByteCode(qsizetype size, char ch) : QByteArray(size, ch)
{
}

ByteCode::ByteCode(const QByteArray& other) : QByteArray(other)
{
}

ByteCode::ByteCode(QByteArray&& other) : QByteArray(other)
{
}

ByteCode::~ByteCode()
{
}	


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void ByteCode::appendData(const uint16_t& value)
{
    this->append(static_cast<char>(value & 0xFF));
    this->append(static_cast<char>((value >> 8) & 0xFF));

    return;
}

void ByteCode::appendData(const uint32_t& value)
{
    this->append(static_cast<char>(value & 0xFF));
    this->append(static_cast<char>((value >> 8) & 0xFF));
    this->append(static_cast<char>((value >> 16) & 0xFF));
    this->append(static_cast<char>((value >> 24) & 0xFF));

    return;
}

void ByteCode::setData(int position, const uint16_t& value)
{
    this->data()[position++] = static_cast<char>(value & 0xFF);
    this->data()[position++] = static_cast<char>((value >> 8) & 0xFF);

    return;
}

void ByteCode::setData(int position, const uint32_t& value)
{
    this->data()[position++] = static_cast<char>(value & 0xFF);
    this->data()[position++] = static_cast<char>((value >> 8) & 0xFF);
    this->data()[position++] = static_cast<char>((value >> 16) & 0xFF);
    this->data()[position++] = static_cast<char>((value >> 24) & 0xFF);

    return;
}


//===================================================================================================================================================
//	Специализированные методы класса
//===================================================================================================================================================
void ByteCode::appendComplexLength(const uint16_t& length)
{
    this->append((char) 0x82);
    appendData(length);

    return;
}

void ByteCode::appendComplexLength(const uint32_t& length)
{
    this->append((char)0x82);
    this->append(length & 0xFF);
    this->append((length >> 8) & 0xFF);
    this->append((length >> 16) & 0xFF);

    return;
}

void ByteCode::setComplexLength(int position, const uint16_t& length)
{
    this->data()[position++] = (char)0x82;
    this->data()[position++] = static_cast<char>(length & 0xFF);
    this->data()[position++] = static_cast<char>((length >> 8) & 0xFF);

    return;
}

void ByteCode::setComplexLength(int position, const uint32_t& length)
{
    this->data()[position++] = (char)0x82;
    this->data()[position++] = static_cast<char>(length & 0xFF);
    this->data()[position++] = static_cast<char>((length >> 8) & 0xFF);
    this->data()[position++] = static_cast<char>((length >> 16) & 0xFF);
    //this->data()[position++] = static_cast<char>((length >> 24) & 0xFF);

    return;
}
