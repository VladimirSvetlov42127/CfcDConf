#ifndef MAIN_FORMS_H
#define MAIN_FORMS_H

#include <gui/forms/DcFormFactory.h>

#include "device_operations/abstract_operation.h"

class QProgressBar;
class QTabWidget;

class MainForm : public DcForm
{
    Q_OBJECT

public:
    MainForm(DcController *controller);

    // Список адресов параметров, состояния которых нужны форме, для отображения данных
    QList<uint16_t> paramAddrList() const;

signals:
    void operationRequest(AbstractOperation::SPtr operation);

public slots:
    // Активация/деактивация всех дополнительных вкладок формы.
    void setTabsActive(bool active);

    // Вызывает у всех вкладок соответсвующую операцию, чтобы они обновили свой интерфейс.
    void operationFinished(AbstractOperation *operation);

private:
    QTabWidget *m_tabWidget;
};

#endif // MAIN_FORMS_H


