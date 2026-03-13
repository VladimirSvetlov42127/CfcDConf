#ifndef QXDHEADERVIEWMENU_H
#define QXDHEADERVIEWMENU_H

#include <dpc/gui/widgets/qxd_tableview/QxdFrame.h>

class QHeaderView;

class QxdHeaderViewMenu : public QxdFrame
{
    Q_OBJECT

public:
    QxdHeaderViewMenu(QHeaderView *source_view, QWidget *parent = nullptr);
    ~QxdHeaderViewMenu();

signals:

private:

};

#endif // QXDHEADERVIEWMENU_H
