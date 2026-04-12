#ifndef REPORT_OPERATION_H
#define REPORT_OPERATION_H

#include "data_model/parameters/parameter.h"
#include "device_operations/abstract_operation.h"

class ReportOperation : public AbstractOperation
{
public:
    ReportOperation(const QString &dirPath, QObject *parent = nullptr);
    Flags flags() const override { return Flag::Read; }

protected slots:
    void onChannelProgress(int state) override;

protected:
    virtual bool before() override;
    virtual bool exec() override;
    virtual bool after() override;

private:
    bool readParams();
    bool dowloadFiles();
    bool clearDirectory();

    uint16_t m_filesCount;
    uint16_t m_currentFile;
    QString m_dirPath;
    std::vector<Parameter::UPtr> m_parameterContainer;
};

#endif // REPORT_OPERATION_H
