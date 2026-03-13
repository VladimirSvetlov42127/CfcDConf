#ifndef SERVICE_H
#define SERVICE_H

#include <memory>
#include <vector>

#include <QString>

#include "service_manager/services/service_input.h"
#include "service_manager/services/service_output.h"

using ServiceInputPtr = std::unique_ptr<ServiceInput>;
using ServiceOutputPtr = std::unique_ptr<ServiceOutput>;
using ServiceInputList = std::vector<ServiceInputPtr>;
using ServiceOutputList = std::vector<ServiceOutputPtr>;

class Service
{
public:
    enum Type {
        NoType = 0,
        AlgType,
        CfcAlgType,
        FuncVOutType
    };

    Service(const QString &name);
    virtual ~Service() = default;

    virtual Service::Type type() const;
    QString name() const;
    void setName(const QString &name);

    const ServiceInputList& inputs() const;
    const ServiceOutputList& outputs() const;

    void print() const;

protected:
    // Добавление входа/выхода в сервис
    void addInput(ServiceInputPtr input);
    void addOutput(ServiceOutputPtr output);

    // Удаление входа/выхода из сервиса. !!!ПОСЛЕ ВЫЗОВА ЭТОЙ ФУНКЦИИ, ВСЕ УКАЗАТЕЛИ НА ВХОД/ВЫХОД СТАНУТ НЕ ВАЛИДНЫМИ!!!
    void deleteInput(ServiceInput *input);
    void deleteOutput(ServiceOutput *output);

private:
    QString m_name;
    ServiceInputList m_inputs;
    ServiceOutputList m_outputs;
};

#endif // SERVICE_H
