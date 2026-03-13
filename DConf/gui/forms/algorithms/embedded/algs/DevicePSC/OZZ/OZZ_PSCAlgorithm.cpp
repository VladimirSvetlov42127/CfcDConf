#include "OZZ_PSCAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_OZZ_WORDIN_PARAM);
}

OZZ_PSCAlgorithm::OZZ_PSCAlgorithm() :
	BasePSCAlgorithm(alg_addrs().floats, 0, false)
{
}

QWidget* OZZ_PSCAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager em(controller(), layout);
	
	em.addLineEditor(alg_addrs().floats, 0, "Порог срабатывания по 3Uo (В)", 1, 60);
	em.addLineEditor(alg_addrs().floats, 1, "Порог несрабатывания (%) (несимметрия K2U=100*U2/U1)", 0, 100);
	em.addCheckEditor(alg_addrs().floats, 2, "Включить блокировку по K2U");
	em.addLineEditor(alg_addrs().floats, 3, "Угол наклона зоны направления мощности Р0 (гр)", 0, 30);
	em.addListEditor(alg_addrs().floats, 4, "Чередование фаз в сети", { "ABC", "BAC" });

	em.addStretch();
	return widget;
}