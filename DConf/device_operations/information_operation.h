#ifndef INFORMATION_OPERATION_H
#define INFORMATION_OPERATION_H

#include <qdatetime.h>

#include <device_operations/abstract_operation.h>
#include <dpc/sybus/ParamPack.h>

class InformationOperation : public AbstractOperation
{
    Q_OBJECT

public:

    struct Element
    {
        QString name;
        QString value;
    };

    // key - индекс элемента параметра
    // value - элемент параметра
    using ParamElements = std::map<uint16_t, Element>;

    using ParamProfiles = std::vector<ParamElements>;
    // key - адрес параметра
    // value - элементы параметра
    using ParamsContainer = std::unordered_map<uint16_t, ParamProfiles>;

    InformationOperation(const QList<uint16_t> &requiredParams, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Read; }

    const ParamsContainer &params() const;
    QDateTime dateTime() const { return m_dateTime; };
    QVariant offset() const { return m_offset; };

protected slots:
    void onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg) override;

protected:
    virtual bool exec() override;

private:
    bool readParams();

private:
    ParamsContainer m_params;
    QDateTime m_dateTime;
    QVariant m_offset;
    QList<uint16_t> m_requiredParams;
};

#endif // INFORMATION_OPERATION_H
