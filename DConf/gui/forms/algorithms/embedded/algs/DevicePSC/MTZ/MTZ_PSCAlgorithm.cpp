#include "MTZ_PSCAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_MTZ_WORDIN_PARAM, SP_MTZIo_WORDIN_PARAM);
}

MTZ_PSCAlgorithm::MTZ_PSCAlgorithm(bool Io) :
	BasePSCAlgorithm(alg_addrs(Io).floats, 0, false),
	m_Io(Io)
{
}

QString MTZ_PSCAlgorithm::name() const
{
	return m_Io ? "МТЗ 3Io" : "МТЗ";
}

QWidget* MTZ_PSCAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager em(controller(), layout);

	uint16_t floatsParam = alg_addrs(m_Io).floats;
	em.addLineEditor(floatsParam, 0, Algs::Text::ActuationAmperage, 10, 2000);
	em.addLineEditor(floatsParam, 1, Algs::Text::ActuationTime, 0.04, 60);
	em.addLineEditor(floatsParam, 2, Algs::Text::ReturnRate, 0.1, 0.99);

	em.addStretch();
	return widget;
}