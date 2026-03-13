#ifndef FILTEREDITOR_H
#define FILTEREDITOR_H

#include <dpc/gui/widgets/qxd_tableview/QxdFrame.h>

class QLabel;
class QPushButton;

class FilterEditor : public QxdFrame
{
	Q_OBJECT

public:
	FilterEditor(const QString &title = QString(), QWidget *parent = nullptr);
	virtual ~FilterEditor();

signals:
    void accepted();

public slots:
    void accept();
    void setActive(bool active);

protected:
	virtual void apply() = 0;

	QWidget* centralWidget() const;
    void setTitle(const QString &title);

private:
	QWidget *m_centralWidget;
    QLabel *m_titleLabel;
    QPushButton *m_okButton;
};

#endif // FILTEREDITOR_H
