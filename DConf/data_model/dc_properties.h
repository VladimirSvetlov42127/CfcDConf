#pragma once

#include <map>

#include <QString>

class DcProperties {
public:
    DcProperties();

    auto begin() const { return m_container.begin(); }
    auto end() const { return m_container.end(); }
    auto cbegin() const noexcept { return m_container.cbegin(); }
    auto cend() const noexcept { return m_container.cend(); }

    bool operator!=(const DcProperties &other) const;
    bool operator==(const DcProperties &other) const;

    // Из Json строки, ключ/значение
    bool fromJson(const QString &jsondoc);

    // В Json строку, ключ/значение
    QString toJson() const;

    void set(const QString &name, const QString &value);
    QString get(const QString &name, const QString &defaultValue = QString()) const;

private:
    std::map<QString, QString> m_container;
};
