#include "CompareFilterEditor.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QCalendarWidget>

static const QString AndString = "И";
static const QString OrString = "Или";
static const QString titleNumericString = "Число";
static const QString titleTimeString = "Время";
static const QString titleDateString = "Дата";
static const QString titleDateTimeFString = "Дата и время";

static QComboBox* createOperatorComboBox(Qxd::CompareOperator &opVar)
{
    QComboBox *comboBox = new QComboBox;
    QStringList items = { "", "равно", "не равно", "меньше", "меньше или равно", "больше", "больше или равно" };
    comboBox->addItems(items);
    comboBox->setCurrentIndex(opVar);
//    comboBox->setFixedWidth(40);
    QObject::connect(comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&opVar](int idx) {
        opVar = (Qxd::CompareOperator) idx;
    });

    return comboBox;
}

static QRadioButton* createCombinationRButton(Qxd::Combination &combVar, Qxd::Combination value,
                                              const QString &text)
{
    QRadioButton *rbutton = new QRadioButton(text);
    if (combVar == value)
        rbutton->setChecked(true);
    QObject::connect(rbutton, &QRadioButton::toggled, [&combVar, value](bool checked) {
        if (checked)
            combVar = value;
    });

    return rbutton;
}

static QWidget* createDoubleEditor(QVariant &valueVar)
{
    QLineEdit *le = new QLineEdit;
    QDoubleValidator *validator = new QDoubleValidator(le);
    le->setValidator(validator);
    if (valueVar.isValid())
        le->setText(valueVar.toString());
    QObject::connect(le, &QLineEdit::textChanged, [&valueVar, validator](const QString &text) {
        if (!text.isEmpty()) {
            bool ok = false;
            valueVar = validator->locale().toInt(text, &ok);
            if (ok)
                return;

            valueVar = validator->locale().toLongLong(text, &ok);
            if (ok)
                return;

            valueVar = validator->locale().toDouble(text, &ok);
            if (ok)
                return;
        }

        valueVar = QVariant();
    });

    return le;
}

static QWidget* createDateTimeEditor(QVariant &valueVar, QVariant::Type valueType, bool hasNull = true)
{
    auto updateVarValue = [&valueVar, valueType](const QDateTime &dt) {
        if (!dt.isValid())
            valueVar = QVariant();
        else if (valueType == QVariant::DateTime)
            valueVar = dt;
        else if (valueType == QVariant::Date)
            valueVar = dt.date();
        else if (valueType == QVariant::Time)
            valueVar = dt.time();
    };

    QWidget *w = new QWidget;
    QHBoxLayout *wl = new QHBoxLayout(w);
    wl->setContentsMargins(0, 0, 0, 0);

    QDateTimeEdit *dte = new QDateTimeEdit(w);
    dte->setCalendarPopup(true);    
    if (valueType == QVariant::Time) {
        dte->setDisplayFormat("hh:mm:ss.zzz");
        dte->setTime(valueVar.toTime());
    }
    if (valueType == QVariant::Date) {
        dte->setDisplayFormat("dd.MM.yyyy");
        dte->setDate(valueVar.toDate());
    }
    if (valueType == QVariant::DateTime) {
        dte->setDisplayFormat("dd.MM.yyyy hh:mm:ss.zzz");
        dte->setDateTime(valueVar.toDateTime());
    }
    QObject::connect(dte, &QDateTimeEdit::dateTimeChanged, updateVarValue );
    wl->addWidget(dte);
    if (!hasNull)
        return w;

    dte->setEnabled(!valueVar.isNull());
    QCheckBox *cb = new QCheckBox(w);
    cb->setChecked(!valueVar.isNull());
    cb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QObject::connect(cb, &QCheckBox::toggled, [dte, updateVarValue](bool checked) {
        if (checked) {
            dte->setEnabled(true);
            updateVarValue(dte->dateTime());
        } else {
            dte->setEnabled(false);
            updateVarValue(QDateTime());
        }
    });
    wl->addWidget(cb);

    return w;
}

CompareFilterEditor::CompareFilterEditor(CompareFilter *filter, const QVariant &min, const QVariant &max, QWidget * parent) :
    FilterEditor(QString(), parent),
    m_filter(filter)
{
    if (!m_filter) {
        setActive(false);
        return;
    }

    m_op1 = filter->operator1();
    m_op2 = filter->operator2();
    m_comb = filter->combination();
    m_value1 = filter->value1();
    m_value2 = filter->value2();
    if (m_op1 == Qxd::Unknown)
        m_value1 = min;
    if (m_op2 == Qxd::Unknown)
        m_value2 = max;

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());

    QHBoxLayout *v1Layout = new QHBoxLayout;
    v1Layout->addWidget(createOperatorComboBox(m_op1));
    mainLayout->addLayout(v1Layout);

    QHBoxLayout *combLayout = new QHBoxLayout;
    combLayout->addStretch();
    combLayout->addWidget(createCombinationRButton(m_comb, Qxd::AND, AndString));
    combLayout->addWidget(createCombinationRButton(m_comb, Qxd::OR, OrString));
    combLayout->addStretch();
    mainLayout->addLayout(combLayout);

    QHBoxLayout *v2Layout = new QHBoxLayout;
    v2Layout->addWidget(createOperatorComboBox(m_op2));
    mainLayout->addLayout(v2Layout);
    mainLayout->addStretch();

    QWidget *value1Widget = nullptr;
    QWidget *value2Widget = nullptr;
    if (filter->type() == Qxd::Numeric) {
        setTitle(titleNumericString);
        value1Widget = createDoubleEditor(m_value1);
        value2Widget = createDoubleEditor(m_value2);
    }
    if (filter->type() == Qxd::Time) {
        setTitle(titleTimeString);
        value1Widget = createDateTimeEditor(m_value1, QVariant::Time, min.isNull());
        value2Widget = createDateTimeEditor(m_value2, QVariant::Time, min.isNull());
    }
    if (filter->type() == Qxd::Date) {
        setTitle(titleDateString);
        value1Widget = createDateTimeEditor(m_value1, QVariant::Date, min.isNull());
        value2Widget = createDateTimeEditor(m_value2, QVariant::Date, min.isNull());
    }
    if (filter->type() == Qxd::DateTime) {
        setTitle(titleDateTimeFString);
        value1Widget = createDateTimeEditor(m_value1, QVariant::DateTime, min.isNull());
        value2Widget = createDateTimeEditor(m_value2, QVariant::DateTime, min.isNull());
    }

    v1Layout->addWidget(value1Widget);
    v2Layout->addWidget(value2Widget);
    setGeometry(0, 0, sizeHint().width(), sizeHint().height());
}

CompareFilterEditor::~CompareFilterEditor()
{
}

void CompareFilterEditor::apply()
{
    if (!m_filter)
        return;

    m_filter->setCombination(m_comb);
    m_filter->setOperator1(m_op1);
    m_filter->setOperator2(m_op2);
    m_filter->setValue1(m_value1);
    m_filter->setValue2(m_value2);

    m_filter->setEnabled( (m_op1 != Qxd::Unknown) || (m_op2 != Qxd::Unknown) );
}
