#include "VNR_GSMAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
}

VNR_GSMAlgorithm::VNR_GSMAlgorithm() :
	IAlgorithm(SP_RZA_ALGMNG)
{
}

bool VNR_GSMAlgorithm::checkForAvailable() const
{
	return DcController::GSM == controller()->type();
}

QWidget* VNR_GSMAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager em(controller(), layout);

	em.addCheckEditor(SP_RZA_ALGMNG, 0, "Алгоритм ВНР включен" );		//	Вопрос адреса

	em.addLineEditor(SP_RZA_ALG, 0, "Число попыток включения привода СВН");
	em.addLineEditor(SP_RZA_ALG, 1, "Число попыток включения привода ШВН-А");
	em.addLineEditor(SP_RZA_ALG, 2, "Число попыток включения привода ШВН-Б");

	em.addLineEditor(SP_RZA_TIME, 0, "Максимальное время хода привода СВН, с", 0);
	em.addLineEditor(SP_RZA_TIME, 1, "Максимальное время хода привода ШВН-А, с", 0);
	em.addLineEditor(SP_RZA_TIME, 2, "Максимальное время хода привода ШВН-Б, с", 0);
	em.addLineEditor(SP_RZA_TIME, 3, "Время задержки до включения цепей 'ОЦ RM6'", 0);

	em.addStretch();
	return widget;
}