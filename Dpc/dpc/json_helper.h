#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <dpc/dpc_global.h>

namespace Dpc {

// JsonHelper - Вспомагательный класс для загрузки/сохранения данных в файлы в формате json.
// Инкапсулирует процесс открытия/записи/чтения файлов и парсинга данных.
// Основная кодировка utf-8, но при не удачном парсинге, пробуется парсинг из windows-1251.

class DPC_EXPORT JsonHelper
{
public:
    JsonHelper();

    // Возвращает json документ после парсинга данных из файла fileName.
    // Предполагается что данные в файле имееют кодировку utf-8 или windows-1251.
    // Если возникает ошибка на любом этапе возвращается пустой документ и устанавливается сообщение об ошибке.
    QJsonDocument load(const QString &fileName);

    // Сохраняет данные json документа в файл fileName в кодировке utf-8.
    // Возвращает true при успехе, и false при возникновении ошибок на любом этапе. Устанавливает сообщение об ошибке.
    bool save(const QJsonDocument &doc, const QString &fileName);

    // Сообщения об ошибке
    QString errorString() const { return m_errorString; }

private:
    void setError(const QString &text = QString()) { m_errorString = text; }

private:
    QString m_errorString;
};

} // Dpc

#endif // JSON_HELPER_H
