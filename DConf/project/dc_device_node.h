#ifndef DCDEVICENODE_H
#define DCDEVICENODE_H

#include "project/dc_node.h"

class DcController;

// DcDeviceNode - Узел дерева в иерархической структуре дерева проектов соответствующий "Конфигурации устройства" в проекте.

class DcDeviceNode : public DcNode
{
public:
    DcDeviceNode(int32_t id, const QString &name, DcNode* parent = nullptr);
    virtual ~DcDeviceNode();

    DcNode::Type type() const override { return DcNode::DeviceType; }

    QIcon icon() const override;

    // Абсолютный путь к папке узла
    QString path() const override;

    // Абсолютный путь к файлу конфигурации устройства.
    QString configFilePath() const;

    // Конфигурация устройства.
    DcController *config() const;
    void setConfig(std::unique_ptr<DcController> config);

private:
    std::unique_ptr<DcController> m_config;
};

#endif // DCDEVICENODE_H
