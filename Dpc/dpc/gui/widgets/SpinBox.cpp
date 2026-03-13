#include "SpinBox.h"

#include <qdebug.h>
#include <qproxystyle.h>
#include <QWheelEvent>

#include <dpc/gui/gui.h>

namespace {
	namespace StyleSheet {
		const QString Invalid = QString("QAbstractSpinBox { background-color: %1; }").arg(Dpc::Gui::color(Dpc::Gui::ErrorColor).name());
		const QString Valid = "";
	}

	namespace ToolTip {
		const QString Invalid = "Текущее значение выходит за пределы допустимых значений[%L1 ... %L2]";
		const QString Valid = "Диапазон допустимых значений = [%L1 ... %L2]";
	}

	class CustomThresholdStyle : public QProxyStyle
	{
	public:
		CustomThresholdStyle(QObject *parent = nullptr) { setParent(parent); }
		int styleHint(StyleHint stylehint, const QStyleOption *opt, const QWidget *widget, QStyleHintReturn *returnData) const {
			if (stylehint == QStyle::SH_SpinBox_ClickAutoRepeatThreshold) {
				return 1000; //1 seconds threshold
			}
			return QProxyStyle::styleHint(stylehint, opt, widget, returnData);
		}
	};
}

namespace Dpc::Gui
{
	template class SpinBox<int, QSpinBox>;
	template class SpinBox<uint, QxUIntSpinBox>;
	template class SpinBox<double, QDoubleSpinBox>;


	template<class DataType, class BaseSpinBox>
	SpinBox<DataType, BaseSpinBox>::SpinBox(QWidget* parent) :
		BaseSpinBox(parent),
		m_inRange(true)
	{
		m_min = BaseSpinBox::minimum();
		m_max = BaseSpinBox::maximum();
		BaseSpinBox::setStyle(new CustomThresholdStyle(this));
	}

	template<class DataType, class BaseSpinBox>
	SpinBox<DataType, BaseSpinBox>::~SpinBox()
	{
	}

	template<class DataType, class BaseSpinBox>
	DataType SpinBox<DataType, BaseSpinBox>::value() const
	{
		return BaseSpinBox::value();
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::setValue(DataType value)
	{
		checkValueInRange(value);
		BaseSpinBox::setValue(value);
	}

	template<class DataType, class BaseSpinBox>
	DataType SpinBox<DataType, BaseSpinBox>::maximum() const
	{
		return m_max;
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::setMaximum(DataType max)
	{
		m_max = max;
		checkValueInRange(value());
	}

	template<class DataType, class BaseSpinBox>
	DataType SpinBox<DataType, BaseSpinBox>::minimum() const
	{
		return m_min;
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::setMinimum(DataType min)
	{
		m_min = min;
		checkValueInRange(value());
	}

	template<class DataType, class BaseSpinBox>
	bool SpinBox<DataType, BaseSpinBox>::inRange() const
	{
		return m_inRange;
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::setRange(DataType min, DataType max)
	{
		setMinimum(min);
		setMaximum(max);
	}

	template<class DataType, class BaseSpinBox>
	DataType SpinBox<DataType, BaseSpinBox>::singleStep() const
	{
		return BaseSpinBox::singleStep();
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::setSingleStep(DataType val)
	{
		BaseSpinBox::setSingleStep(val);
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::setReadOnly(bool r)
	{
		BaseSpinBox::setReadOnly(r);
        BaseSpinBox::setButtonSymbols(r ? QAbstractSpinBox::NoButtons : QAbstractSpinBox::UpDownArrows);
    }

    template<class DataType, class BaseSpinBox>
    void SpinBox<DataType, BaseSpinBox>::setWheelEventEnabled(bool enabled)
    {
        m_wheelEventEnabled = enabled;
    }

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::stepBy(int steps)
	{
		if (!inRange()) {
			fixRange();
			return;
		}
		BaseSpinBox::stepBy(steps);
	}

	template<class DataType, class BaseSpinBox>
	QString SpinBox<DataType, BaseSpinBox>::textFromValue(DataType value) const
	{
		auto it = m_valuesText.find(value);
		if (it != m_valuesText.end())
			return it.value();

		return BaseSpinBox::textFromValue(value);
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::addValueText(DataType value, const QString& text)
	{
		m_valuesText[value] = text;
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::setValuesText(const SpinBox::ValuesTextContainerType& con)
	{
        m_valuesText = con;
    }

    template<class DataType, class BaseSpinBox>
    void SpinBox<DataType, BaseSpinBox>::wheelEvent(QWheelEvent *event)
    {
        if (!m_wheelEventEnabled) {
            event->accept();
            return;
        }

        BaseSpinBox::wheelEvent(event);
    }

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::checkValueInRange(DataType value)
	{
		if (value < m_min || value > m_max) {
			BaseSpinBox::setToolTip(ToolTip::Invalid.arg(m_min).arg(m_max));
			if (!inRange())
				return;

			m_inRange = false;
			BaseSpinBox::setStyleSheet(StyleSheet::Invalid);
			BaseSpinBox::setRange(std::numeric_limits<DataType>::lowest(), std::numeric_limits<DataType>::max());
			return;
		}

		fixRange();
	}

	template<class DataType, class BaseSpinBox>
	void SpinBox<DataType, BaseSpinBox>::fixRange()
	{
		BaseSpinBox::setRange(m_min, m_max);
		BaseSpinBox::setToolTip(ToolTip::Valid.arg(m_min).arg(m_max));
		if (inRange())
			return;

		m_inRange = true;
		BaseSpinBox::setStyleSheet(StyleSheet::Valid);
	}
} // namespace
