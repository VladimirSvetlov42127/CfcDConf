#include "ListFilterEditor.h"

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QCheckBox>
#include <QLineEdit>
#include <QScrollBar>
#include <QGroupBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QRadioButton>
#include <QLabel>

#include <dpc/gui/widgets/qxd_tableview/filters/IValueEditor.h>

namespace {

    const QString titleString = "Фильтр";
    const QString allString = "Все";
    const QString searchString = "Поиск";
    const QString emptyString = "(Пустые)";
    const QString exactMatchString = "Точное соответствие";
    const QString caseSensitivityString = "Учитывать регистр";

    const QString compareGroupHeaderString = "Показать строки, значения которых:";
    const QString andString = "И";
    const QString orString = "Или";

    const QStringList compareOpStirngs = { "", "равно", "не равно", "меньше", "меньше или равно", "больше", "больше или равно" };



    class DoubleEditor : public IValueEditor
    {
    public:
        DoubleEditor(const QVariant &val, QWidget *parent = nullptr) :
            IValueEditor(parent)
        {
            QHBoxLayout *l = new QHBoxLayout(this);
            l->setContentsMargins(0, 0, 0, 0);
            l->setSpacing(0);
            QLineEdit *le = new QLineEdit(this);
            l->addWidget(le);
            QDoubleValidator *validator = new QDoubleValidator(le);
            le->setValidator(validator);
            if (val.isValid())
                le->setText(val.toString());

            QObject::connect(le, &QLineEdit::textChanged, [=](const QString &text) {
                QVariant newVal;
                if (!text.isEmpty()) {
                    bool ok = false;
                    newVal = validator->locale().toInt(text, &ok);
                    if (!ok)
                        newVal = validator->locale().toLongLong(text, &ok);
                    if (!ok)
                        newVal = validator->locale().toDouble(text, &ok);
                    if (!ok)
                        newVal = QVariant();
                }
                emit valueChanged(newVal);
            });
        }
    };

    class StringEditor : public IValueEditor
    {
    public:
        StringEditor(const QVariant &val, QWidget *parent = nullptr) :
            IValueEditor(parent)
        {
            QHBoxLayout *l = new QHBoxLayout(this);
            l->setContentsMargins(0, 0, 0, 0);
            l->setSpacing(0);
            QLineEdit *le = new QLineEdit(this);
            l->addWidget(le);
            le->setText(val.toString());
            QObject::connect(le, &QLineEdit::textChanged, this, &IValueEditor::valueChanged);
        }
    };

    class DateTimeEditor : public IValueEditor
    {
    public:
        DateTimeEditor(const QVariant &val, QVariant::Type type, QWidget *parent = nullptr) :
            IValueEditor(parent)
        {
            QHBoxLayout *l = new QHBoxLayout(this);
            l->setContentsMargins(0, 0, 0, 0);
            l->setSpacing(0);
            QDateTimeEdit *dte = new QDateTimeEdit(this);
            l->addWidget(dte);
            dte->setCalendarPopup(true);
            if (type == QVariant::Time) {
                dte->setDisplayFormat("hh:mm:ss.zzz");
                dte->setTime(val.toTime());
            }
            if (type == QVariant::Date) {
                dte->setDisplayFormat("dd.MM.yyyy");
                dte->setDate(val.toDate());
            }
            if (type == QVariant::DateTime) {
                dte->setDisplayFormat("dd.MM.yyyy hh:mm:ss.zzz");
                dte->setDateTime(val.toDateTime());
            }

            connect(dte, &QDateTimeEdit::dateTimeChanged, [=](const QDateTime &dt) {
                QVariant newVal;
                if (dt.isValid()) {
                    if (type == QVariant::Time)
                        newVal = dt.time();
                    else if (type == QVariant::Date)
                        newVal = dt.date();
                    else if (type == QVariant::DateTime)
                        newVal = dt;
                }

                emit valueChanged(newVal);
            });
        }
    };

    IValueEditor *createEditor(QVariant::Type type, const QVariant &val = QVariant())
    {
        switch (type) {
        case QVariant::Int :
        case QVariant::UInt :
        case QVariant::LongLong :
        case QVariant::ULongLong :
        case QVariant::Double :
            return new DoubleEditor(val);
        case QVariant::Time :
        case QVariant::Date :
        case QVariant::DateTime :
            return new DateTimeEditor(val, type);
        default:
            return new StringEditor(val);
        }
    }

} // namespace

ListFilterEditor::ListFilterEditor(ListFilter *filter, QAbstractItemModel *source_model, int column, int filterRole, OtherFiltersAcceptFunc accept, QWidget *parent) :
    FilterEditor(titleString, parent)
{
    setGeometry(0, 0, 200, 250);

    QVBoxLayout *l = new QVBoxLayout(centralWidget());
    l->setContentsMargins(0, 0, 0, 0);

    if (!filter || !source_model || !source_model->rowCount()) {
        setActive(false);
        return;
    }

    QVariant::Type dataType = filter->dataType() != QVariant::Invalid ?
                filter->dataType() : source_model->index(0, column).data(filterRole).type();
    bool getVal1 = filter->value1().isNull();
    bool getVal2 = filter->value2().isNull();
    if (dataType != QVariant::String && (getVal1 || getVal2))
        for(int i = 0; i < source_model->rowCount(); i++) {
            QVariant v = source_model->index(i, column).data(filterRole);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
            if (getVal1 && !v.isNull() && (filter->value1().isNull() || (QVariant::compare(v, filter->value1()) < 0)))
                filter->setValue1(v);
            if (getVal2 && !v.isNull() && (filter->value2().isNull() || (QVariant::compare(v, filter->value2()) > 0)))
                filter->setValue2(v);
#else
            if ( getVal1 && !v.isNull() && (filter->value1().isNull() || v < filter->value1()) )
                filter->setValue1(v);
            if ( getVal2 && !v.isNull() && (filter->value2().isNull() || v > filter->value2()) )
                filter->setValue2(v);
#endif
        }

    QGroupBox *compareGroup = new QGroupBox(this);
    QVBoxLayout *compareGroupLayout = new QVBoxLayout(compareGroup);
    compareGroupLayout->setContentsMargins(4, 4, 4, 4);

    QHBoxLayout *compareGroupHeaderLayout = new QHBoxLayout;
    QLabel *compareGroupHeaderLabel = new QLabel(compareGroupHeaderString, this);
    QFont f = compareGroupHeaderLabel->font();
    f.setItalic(true);
    compareGroupHeaderLabel->setFont(f);
    compareGroupHeaderLayout->addWidget(compareGroupHeaderLabel);
    compareGroupLayout->addLayout(compareGroupHeaderLayout);

    QHBoxLayout *v1Layout = new QHBoxLayout;
    QComboBox *op1Box = new QComboBox(this);
    op1Box->addItems(compareOpStirngs);
    op1Box->setCurrentIndex(filter->operator1());
    v1Layout->addWidget(op1Box);
    IValueEditor *editor1 = createEditor(dataType, filter->value1());
    editor1->setEnabled(filter->operator1() != Qxd::Unknown);
    v1Layout->addWidget(editor1);
    compareGroupLayout->addLayout(v1Layout);

    QHBoxLayout *combLayout = new QHBoxLayout;
    combLayout->addStretch();
    QRadioButton *andRButton = new QRadioButton(andString, this);
    andRButton->setChecked(filter->combination() == Qxd::AND);
    combLayout->addWidget(andRButton);
    QRadioButton *orRButton = new QRadioButton(orString, this);
    orRButton->setChecked(filter->combination() == Qxd::OR);
    combLayout->addWidget(orRButton);
    combLayout->addStretch();
    compareGroupLayout->addLayout(combLayout);

    QHBoxLayout *v2Layout = new QHBoxLayout;
    QComboBox *op2Box = new QComboBox(this);
    op2Box->addItems(compareOpStirngs);
    op2Box->setCurrentIndex(filter->operator2());
    v2Layout->addWidget(op2Box);
    IValueEditor *editor2 = createEditor(dataType, filter->value2());
    editor2->setEnabled(filter->operator2() != Qxd::Unknown);
    v2Layout->addWidget(editor2);
    compareGroupLayout->addLayout(v2Layout);
    compareGroup->setEnabled(dataType != QVariant::String);
    l->addWidget(compareGroup);

    QGroupBox *searchGroup = new QGroupBox(this);
    QVBoxLayout *searchGroupLayout = new QVBoxLayout(searchGroup);
    searchGroupLayout->setSpacing(0);
    searchGroupLayout->setContentsMargins(4, 4, 4, 4);

    QLineEdit *maskEdit = new QLineEdit(this);
    maskEdit->setClearButtonEnabled(true);
    maskEdit->setPlaceholderText(searchString);
    maskEdit->setText(filter->mask());
    searchGroupLayout->addWidget(maskEdit);

    QCheckBox *exactMatchBox = new QCheckBox(this);
    exactMatchBox->setText(exactMatchString);
    exactMatchBox->setChecked(filter->exactMatch());
    searchGroupLayout->addWidget(exactMatchBox);

    QCheckBox *caseSensitivityBox = new QCheckBox(this);
    caseSensitivityBox->setText(caseSensitivityString);
    caseSensitivityBox->setChecked(filter->sensitivity());
    searchGroupLayout->addWidget(caseSensitivityBox);
    l->addWidget(searchGroup);

    ListFilterModel *fm = new ListFilterModel(filter, accept, this);
    fm->setFilterKeyColumn(column);
    fm->setFilterRole(filterRole);
    fm->setSourceModel(source_model);

    QCheckBox *allCheckBox = new QCheckBox(allString, this);
    l->addWidget(allCheckBox);

	QTableView *view = new QTableView(this);
	l->addWidget(view);
	view->setShowGrid(false);
    view->setWordWrap(false);
	view->horizontalHeader()->setStretchLastSection(true);
	view->horizontalHeader()->hide();
    view->verticalHeader()->setMinimumSectionSize(0);
    view->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    view->verticalHeader()->hide();
    view->verticalHeader()->setDefaultSectionSize( QFontMetrics(fm->index(0,0).data(Qt::FontRole).value<QFont>()).height() + 2 );
    view->setModel(fm);

    allCheckBox->setCheckState(fm->allCheckState());
    view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    int width = view->horizontalHeader()->sectionSize(0) + view->verticalScrollBar()->sizeHint().width() + 20;
    if (width < sizeHint().width())
        width = sizeHint().width();
    setGeometry(0, 0, width, sizeHint().height());

    connect(this, &ListFilterEditor::applyPressed, fm, &ListFilterModel::saveToFilter);
    connect(maskEdit, &QLineEdit::textEdited, fm, &ListFilterModel::setMask);
    connect(exactMatchBox, &QCheckBox::clicked, fm, &ListFilterModel::setExactMatch);
    connect(caseSensitivityBox, &QCheckBox::clicked, [=](bool checked) {
        fm->setCaseSensitivity(checked ? Qt::CaseSensitive : Qt::CaseInsensitive);
    });
    connect(fm, &ListFilterModel::allCheckStateChanged, this, [=](Qt::CheckState s) {
        allCheckBox->setCheckState(s);
    });
    connect(allCheckBox, &QCheckBox::clicked, this, [=](bool checked) {
        allCheckBox->setTristate(false);
        fm->setAllCheckState(checked);
    });

    connect(editor1, &IValueEditor::valueChanged, fm, &ListFilterModel::setValue1);
    connect(editor2, &IValueEditor::valueChanged, fm, &ListFilterModel::setValue2);
    connect(op1Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int idx) {
        fm->setOperator1(Qxd::CompareOperator(idx));
        editor1->setEnabled(idx != Qxd::Unknown);
    });
    connect(op2Box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int idx) {
        fm->setOperator2(Qxd::CompareOperator(idx));
        editor2->setEnabled(idx != Qxd::Unknown);
    });
    connect(andRButton, &QRadioButton::toggled, [fm](bool checked) {
        if (checked)
            fm->setCombination(Qxd::AND);
    });
    connect(orRButton, &QRadioButton::toggled, [fm](bool checked) {
        if (checked)
            fm->setCombination(Qxd::OR);
    });
}

void ListFilterEditor::apply()
{
    emit applyPressed();
}
