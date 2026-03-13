#pragma once

#include <qspinbox.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	template<class DataType, class BaseSpinBox>
	class DPC_EXPORT SpinBox : public BaseSpinBox
	{
	public:
		using ValueType = DataType;
		using ValuesTextContainerType = QHash<DataType, QString>;

		SpinBox(QWidget* parent = nullptr);
		virtual ~SpinBox();

		DataType value() const;
		void setValue(DataType value);

		DataType maximum() const;
		void setMaximum(DataType max);

		DataType minimum() const;
		void setMinimum(DataType min);

		bool inRange() const;
		void setRange(DataType min, DataType max);

		DataType singleStep() const;
		void setSingleStep(DataType val);

		void setReadOnly(bool r);
        void setWheelEventEnabled(bool enabled);

		virtual void stepBy(int steps) override;
		virtual QString textFromValue(DataType value) const override;

		void addValueText(DataType value, const QString& text);
		void setValuesText(const ValuesTextContainerType& con);

    protected:
        void wheelEvent(QWheelEvent *event) override;

	private:
		void checkValueInRange(DataType value);
		void fixRange();

	private:
		DataType m_min;
		DataType m_max;
		bool m_inRange;
		ValuesTextContainerType m_valuesText;
        bool m_wheelEventEnabled = false;
	};

	class DPC_EXPORT QxUIntSpinBox : public QDoubleSpinBox
	{
		Q_OBJECT
	public:
		QxUIntSpinBox(QWidget* parent = nullptr) : QDoubleSpinBox(parent) {
			QDoubleSpinBox::setDecimals(0);
			connect(this, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double val) { emit valueChanged(val); });
		}

		virtual QString textFromValue(double value) const override { return textFromValue(static_cast<uint>(value)); }
		virtual QString textFromValue(uint value) const { return QDoubleSpinBox::textFromValue(value); }

	signals:
		void valueChanged(uint value);
	};

	// сделано так, потому что using обЪявления нельзя сделать forward declaration
	class DPC_EXPORT IntSpinBox : public SpinBox<int, QSpinBox> { using SpinBox::SpinBox; };
	class DPC_EXPORT UIntSpinBox : public SpinBox<uint, QxUIntSpinBox> { using SpinBox::SpinBox; };
	class DPC_EXPORT DoubleSpinBox : public SpinBox<double, QDoubleSpinBox> { using SpinBox::SpinBox; };
} // namespace
