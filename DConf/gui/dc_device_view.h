#ifndef DCDEVICEVIEW_H
#define DCDEVICEVIEW_H

#include <QTreeView>

class QStandardItem;
class QStandardItemModel;

class DcMenu;
class DcController;

class DcDeviceView : public QTreeView
{
    Q_OBJECT
public:
    DcDeviceView(DcController *config, QWidget* parent = nullptr);
    virtual ~DcDeviceView();

    DcController* config() const;

signals:
    void itemSelected(const QString& itemIdx, bool isConfigItem);

private slots:
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void addSubmenu(DcMenu *menu, QStandardItem *parent_view_item);

private:
    QStandardItemModel* m_model;
    DcController *m_config;
};

#endif // DCDEVICEVIEW_H
