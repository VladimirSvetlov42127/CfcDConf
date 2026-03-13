#include "json_helper.h"

#include <QFile>
#include <QTextCodec>

namespace Dpc {

JsonHelper::JsonHelper()
{
}

QJsonDocument JsonHelper::load(const QString &fileName)
{
    setError();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        setError(file.errorString());
        return QJsonDocument();
    }

    auto data = file.readAll();
    QJsonParseError parseInfo;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseInfo);
    if (QJsonParseError::NoError != parseInfo.error) {
        auto firstError = parseInfo.errorString();

        // Пробуем перевести в utf-8 и распарсить ещё раз
        static QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
        data = codec->toUnicode(data).toUtf8();
        doc = QJsonDocument::fromJson(data, &parseInfo);
        if (QJsonParseError::NoError != parseInfo.error) {
            setError(firstError);
            return QJsonDocument();
        }
    }

    return doc;
}

bool JsonHelper::save(const QJsonDocument &doc, const QString &fileName)
{
    setError();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        setError(file.errorString());
        return false;
    }

    file.write(doc.toJson());
    return true;
}

}
