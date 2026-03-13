#include "dc_project.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QHash>
#include <QDir>
#include <QDebug>

#include <dpc/json_helper.h>

#include "project/dc_dir_node.h"
#include "project/dc_device_node.h"

namespace {

const char* g_InfoFile = "info.cfg";
const char* g_TopologyFile = "topology.cfg";

const char* KEY_NAME   = "name";
const char* KEY_OBJECT = "object";
const char* KEY_AUTHOR = "author";
const char* KEY_DESC   = "desc";
const char* KEY_CTIME  = "create_time";
const char* KEY_UTIME  = "update_time";
const char* KEY_VER    = "version";

const char* KEY_TOPOLOGY_TREE = "tree";
const char* KEY_TOPOLOGY_ID   = "id";
const char* KEY_TOPOLOGY_NAME = "name";
const char* KEY_TOPOLOGY_PID  = "pid";
const char* KEY_TOPOLOGY_TYPE = "type";

const char* VALUE_TOPOLOGY_PROJECT    = "project";
const char* VALUE_TOPOLOGY_FOLDER     = "folder";
const char* VALUE_TOPOLOGY_CONTROLLER = "controller";

QString toText(DcNode::Type type)
{
    switch (type) {
    case DcNode::ProjectType:
        return VALUE_TOPOLOGY_PROJECT;
        break;
    case DcNode::DirType:
        return VALUE_TOPOLOGY_FOLDER;
        break;
    case DcNode::DeviceType:
        return VALUE_TOPOLOGY_CONTROLLER;
        break;
    default:
        break;
    }

    return QString();
}

DcNode::Type toType(const QString &text)
{
    if (text == VALUE_TOPOLOGY_PROJECT)
        return DcNode::ProjectType;
    else if (text == VALUE_TOPOLOGY_FOLDER)
        return DcNode::DirType;
    else if (text == VALUE_TOPOLOGY_CONTROLLER)
        return DcNode::DeviceType;

    return DcNode::NoType;
}

QList<const DcNode*> getAllNodes(const DcNode* node)
{
    QList<const DcNode*> result;
    result.append(node);
    for(size_t i = 0; i < node->childsSize(); ++i)
        result.append(getAllNodes(node->child(i)));

    return result;
}

DcNode::UPtr makeNode(int32_t id, const QString &name, DcNode::Type type, DcNode* parent)
{
    switch (type) {
    case DcNode::DirType: return std::make_unique<DcDirNode>(id, name, parent);
    case DcNode::DeviceType: return std::make_unique<DcDeviceNode>(id, name, parent);
    default: return DcNode::UPtr();
    }
}

} // namespace

DcProject::DcProject(const QString &path, const QString &name)
    : DcNode(1, name)
    , m_path{path}
    , m_createdTime{QDateTime::currentDateTime()}
    , m_updatedTime{QDateTime::currentDateTime()}
    , m_version{0}
{
}

DcProject::~DcProject()
{
//    qDebug() << "~Deleting DcProject" << name();
}

QString DcProject::infoFilePath(const QString &prefix)
{
    if (prefix.isEmpty())
        return g_InfoFile;

    return QString("%1/%2").arg(prefix, g_InfoFile);
}

QString DcProject::topologyFilePath(const QString &prefix)
{
    if (prefix.isEmpty())
        return g_TopologyFile;

    return QString("%1/%2").arg(prefix, g_TopologyFile);
}

QIcon DcProject::icon() const
{
    return QIcon(":/icons/proj.svg");
}

bool DcProject::load()
{
    Dpc::JsonHelper json_helper;
    auto infoFile = infoFilePath(path());
    auto infoJsonDoc = json_helper.load(infoFile);
    if (infoJsonDoc.isNull()) {
//        emit error(QString("Не удалось прочитать файл %1: %2").arg(infoFile, json_helper.errorString()));
        return false;
    }

    auto topologyFile = topologyFilePath(path());
    auto topologyJsonDoc = json_helper.load(topologyFile);
    if (infoJsonDoc.isNull()) {
//        emit error(QString("Не удалось прочитать файл %1: %2").arg(topologyFile, json_helper.errorString()));
        return false;
    }

    auto info = infoJsonDoc.object();
    setName(info.value(KEY_NAME).toString());
    m_object = info.value(KEY_OBJECT).toString();
    m_author = info.value(KEY_AUTHOR).toString();
    m_desc = info.value(KEY_DESC).toString();
    m_createdTime = QDateTime::fromString(info.value(KEY_CTIME).toString(), "(yyyy.MM.dd) hh:mm:ss");
    m_updatedTime = QDateTime::fromString(info.value(KEY_UTIME).toString(), "(yyyy.MM.dd) hh:mm:ss");
    m_version = info.value(KEY_VER).toString().toInt();

    auto topology = topologyJsonDoc.object();
    auto array = topology.value(KEY_TOPOLOGY_TREE).toArray();
    QHash<uint16_t, DcNode*> cache;
    bool hasProject = false;
    for(auto o: array) {
        auto obj = o.toObject();

        // Пропускаем элементы с некорректным id
        auto id = obj[KEY_TOPOLOGY_ID].toInt();
        if (!id)
            continue;

        // Пропускаем элементы с некорректным type
        auto type = toType(obj[KEY_TOPOLOGY_TYPE].toString());
        if (!type)
            continue;

        // Пропускаем элементы у которых указан pid, но сам родитель не найден (нет родителя или нарушен порядок в файле)
        auto pid = obj[KEY_TOPOLOGY_PID].toInt();
        auto parent = cache.value(pid, nullptr);
        if (!parent && pid)
            continue;

        // Корневой элемент может быть только 1, все остальные отбрасываем.
        if (DcNode::ProjectType == type) {
            if (!pid && !hasProject) {
                setId(id);
                cache[id] = this;
                hasProject = true;
            }

            continue;
        }

        auto name = obj[KEY_TOPOLOGY_NAME].toString();
        auto node = makeNode(id, name, type, parent);
        if (parent && node) {
            cache[id] = node.get();
            DcNode::UPtrVector list;
            list.emplace_back(std::move(node));
            parent->insertChilds(parent->childsSize(), std::move(list) );
        }
    }

    return true;
}

bool DcProject::save() const
{
    m_updatedTime = QDateTime::currentDateTime();

    QJsonObject info;
    info[KEY_NAME] = name();
    info[KEY_OBJECT] = object();
    info[KEY_AUTHOR] = author();
    info[KEY_DESC] = desc();
    info[KEY_CTIME] = createdTime().toString("(yyyy.MM.dd) hh:mm:ss");
    info[KEY_UTIME] = updatedTime().toString("(yyyy.MM.dd) hh:mm:ss");
    info[KEY_VER] = QString::number(version());

    QJsonObject topology;
    QJsonArray array;
    for(auto node: getAllNodes(this)) {
        QJsonObject obj;
        obj[KEY_TOPOLOGY_ID] = node->id();
        obj[KEY_TOPOLOGY_NAME] = node->name();
        obj[KEY_TOPOLOGY_TYPE] = toText(node->type());
        obj[KEY_TOPOLOGY_PID] = node->parent() ? node->parent()->id() : 0;
        array.append(obj);
    }
    topology[KEY_TOPOLOGY_TREE] = array;

    Dpc::JsonHelper json_helper;
    auto infoFile = infoFilePath(path());
    if (!json_helper.save(QJsonDocument(info), infoFile)) {
//        emit error(QString("Не удалось сохранить файл %1: %2").arg(infoFile, json_helper.errorString()));
        return false;
    }

    auto topologyFile = topologyFilePath(path());
    if (!json_helper.save(QJsonDocument(topology), topologyFile)) {
//        emit error(QString("Не удалось сохранить файл %1: %2").arg(topologyFile, json_helper.errorString()));
        return false;
    }

    return true;
}

DcNode *DcProject::createNode(const QString &name, Type type, DcNode *parent)
{
    if (!parent)
        return nullptr;

    auto newIndex = maxId() + 1;
    DcNode::UPtr node = makeNode(newIndex, name, type, parent);
    if (!node)
        return nullptr;

    // Для DeviceType нужно создать папку устройства.
    if (DeviceType == node->type()) {
        QDir dir(node->path());
        if (dir.exists() && !dir.removeRecursively()) {
//            emit error(QString("Ошибка удаления старой папки(%1) в проекте! Проверьте, все ли файлы закрыты").arg(node->path()));
            return nullptr;
        }

        //	Создание папки конфигурации устройства
        if (!QDir().mkdir(node->path())) {
//            emit error(QString("Не удалось создать папку(%1) конфигурации устройства").arg(node->path()));
            return nullptr;
        }
    }

    auto raw = node.get();
    DcNode::UPtrVector list;
    list.emplace_back(std::move(node));
    parent->insertChilds(parent->childsSize(), std::move(list) );

    return raw;
}

bool DcProject::removeNodeData(DcNode *node)
{
    if (!node || node == this || node->path().isEmpty())
        return false;

    return QDir(node->path()).removeRecursively();
}
