#include "AMTZ_PSCAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_AMTZ_WORDIN_PARAM, SP_AMTZIo_WORDIN_PARAM)
}

AMTZ_PSCAlgorithm::AMTZ_PSCAlgorithm(bool Io) :
	BasePSCAlgorithm(alg_addrs(Io).floats, 0, false), 
	m_Io(Io)
{
}

QString AMTZ_PSCAlgorithm::name() const
{
	return m_Io ? "АМТЗ 3Io" : "АМТЗ";
}

QWidget* AMTZ_PSCAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager em(controller(), layout);

	uint16_t floatsParam = alg_addrs(m_Io).floats;
	em.addLineEditor(floatsParam, 0, "Время усреднения (с)", 0.04, 60);
	em.addLineEditor(floatsParam, 1, "Порог срабатывания (A)", m_Io ? 2 : 5, 500);
	em.addLineEditor(floatsParam, 2, "Время возврата (с)", 0.04, 60);
	em.addLineEditor(floatsParam, 3, "Абс. порог тока (А)", m_Io ? 2 : 5, 1000);

	em.addStretch();
	return widget;
}