#pragma once

#include <memory>

#include <qwidget.h>
#include <qboxlayout.h>
#include <qdebug.h>

#include <dpc/journal/Journal.h>

#include "data_model/dc_controller.h"

class DcForm;
class QLabel;

using DcFormPtr = std::unique_ptr<DcForm>;

class DcForm : public QWidget 
{
public:
    explicit DcForm(DcController *controller, const QString &title = QString(), bool hasShape = true);
	virtual ~DcForm();

    void setJournal(Dpc::Journal *journal);
    Dpc::Journal* journal() const;
	DcController* controller() const;

    QString title() const;
//	virtual void activate() {}

protected:
	QWidget* centralWidget() const;
	void setTitle(const QString &title);

	struct Param {
		int32_t addr;
		int32_t index = 0; };

	static bool hasAny(DcController *controller, const QList<Param> &params);
	static bool hasAll(DcController *controller, const QList<Param> &params);

private:
	DcController *m_controller;
    QString m_title;

	QWidget *m_centralWidget;
    Dpc::Journal *m_journal;
};
