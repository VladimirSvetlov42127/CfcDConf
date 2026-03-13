#include "IAlgorithm.h"

#include <gui/forms/algorithms/embedded/ProfileWidget.h>

 QList<AlgorithmPtr>& algorithmsList() { static QList<AlgorithmPtr> list; return list; }

 IAlgorithm::IAlgorithm(int32_t mainParamAddr, int32_t mainParamIdx, bool useMainParamAsSwitcher) :
	 m_mainParamAddr(mainParamAddr),
	 m_mainParamIdx(mainParamIdx),
	 m_useMainParamAsSwitcher(useMainParamAsSwitcher)
 {
 }

 void IAlgorithm::setController(DcController * controller)
 {
	 m_controller = controller;
 }

 bool IAlgorithm::isAvailable() const
 {
	 if (!mainParam())
		 return false;

	 return checkForAvailable();
 }

 bool IAlgorithm::isAnyActive() const
 {
	 for (size_t i = 0; i < groupCount(); i++)
		 if (isActive(i))
			 return true;

	 return false;
 }

 bool IAlgorithm::isActive(int group) const
 {
	 return m_useMainParamAsSwitcher ? mainParam(group)->value().toInt() : true;
 }

 void IAlgorithm::setActive(bool active, int group)
 {
	 if (!m_useMainParamAsSwitcher)
		 return;

	 mainParam(group)->updateValue(QString::number(active ? 1 : 0));
 }

 bool IAlgorithm::hasSwitcher() const
 {
	 return m_useMainParamAsSwitcher;
 }

 int IAlgorithm::groupCount() const
 {
	 // Костыль, пока не избавились от старых алгоритмов
	 uint8_t firstByte = m_mainParamAddr & 0xFF;
	 QList<int> algsFirstByte = { 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67 };
	 if (!algsFirstByte.contains(firstByte))
		 return 1;

     return mainParam()->parameter()->profilesCount();
 }

 void IAlgorithm::fillReport(DcIConfigReport * report) const
 {
	 QStringList headers;
	 auto grpCount = groupCount();
	 if (grpCount > 1) {
		 headers << Text::ReportTable::Name;
         for (int i = 0; i < grpCount; i++)
			 headers << Algs::Text::SettingsGroupNo.arg(i + 1);
	 }

	 DcReportTable table(report->device(), headers, { 40 });
	 if (hasSwitcher()) {
		 QList<DcReportTable::ParamRecord> recList;
         for (int i = 0; i < grpCount; i++)
			 recList << DcReportTable::ParamRecord(mainParam(i), QString(), true);
		 table.addRow("Введен в работу", recList);
	 }

	 fillReport(&table);
	 report->insertTable(table);
 }

 DcController * IAlgorithm::controller() const
 {
	 return m_controller;
 }

 ParameterElement *IAlgorithm::mainParam(int group) const
 {
	 if (!controller())
		 return nullptr;

     return controller()->paramsRegistry().element(m_mainParamAddr, alg_index(group, m_mainParamIdx));
 }

 bool IAlgorithm::checkForAvailable() const
 {
	 return true;
 }

 void IAlgorithm::fillReport(DcReportTable * table) const
 {
 }

QWidget * IAlgorithm::widgetForProfile(int profile) const
{
    auto activeProfileParam = controller()->paramsRegistry().element(SP_ACTIVPROFILE, 0);

	QWidget *result = nullptr;	
	if (groupCount() > 1) {
		auto w = new ProfileWidget(profile, Algs::Text::SettingsGroupNo.arg(profile + 1));
		w->setEnabled(isActive(profile));
		if (activeProfileParam) {
			w->onProfileChanged(activeProfileParam->value());
			QObject::connect(activeProfileParam->helper(), &__Helper::valueChanged, w, &ProfileWidget::onProfileChanged);
		}

		result = w;
	}
	else
		result = new QWidget;

	return result;
 }

AlgDataModel * IAlgorithm::createModel(QObject *parent) const
{
    auto activeProfileParam = controller()->paramsRegistry().element(SP_ACTIVPROFILE, 0);

	auto model = new AlgDataModel(parent);
	if (groupCount() > 1) {
		model->setHighlightCurrentGroup(true);
		if (activeProfileParam) {
            auto onActiveProfilleChanged = [=](const QString &value) {
//                qDebug() << "ActiveProfie changed to" << value;
                model->setCurrentGroup(value.toInt());
            };

			QObject::connect(activeProfileParam->helper(), &__Helper::valueChanged, model, onActiveProfilleChanged);
			onActiveProfilleChanged(activeProfileParam->value());

			if (m_useMainParamAsSwitcher)
				for (size_t i = 0; i < groupCount(); i++) {
					model->setGroupEnabled(i, isActive(i));
                    QObject::connect(mainParam(i)->helper(), &__Helper::valueChanged, model, [=](const QString &value) {
//                        qDebug() << "MainParamSwitcher" << i << "changed to" << value;
                        model->setGroupEnabled(i, value.toUInt());
                    });
				}
		}
		
	}
	return model;
}
