#include "dc_device_node.h"

#include <QDebug>

#include "data_model/dc_controller.h"

namespace {
const char *g_dirPrefix = "u";
const char *g_configFile = "uconf.db";
}

DcDeviceNode::DcDeviceNode(int32_t id, const QString &name, DcNode *parent)
    : DcNode{id, name, parent}
{
}

DcDeviceNode::~DcDeviceNode()
{
    //    qDebug() << "~DcDeviceNode" << name();
}

QIcon DcDeviceNode::icon() const
{
    return config() ? config()->icon() : QIcon();
}

QString DcDeviceNode::path() const
{
    if (!parent())
        return QString();

    auto rootNode = parent();
    while(rootNode->parent())
        rootNode = rootNode->parent();

    auto rootPath = rootNode->path();
    if (rootPath.isEmpty())
        return QString();

    return QString("%1/%2%3").arg(rootNode->path(), g_dirPrefix).arg(id());
}

QString DcDeviceNode::configFilePath() const
{
    auto prefix = path();
    if (prefix.isEmpty())
        return QString();

    return QString("%1/%2").arg(prefix, g_configFile);
}

DcController *DcDeviceNode::config() const
{
    return m_config.get();
}

void DcDeviceNode::setConfig(std::unique_ptr<DcController> config)
{
    m_config = std::move(config);
}
