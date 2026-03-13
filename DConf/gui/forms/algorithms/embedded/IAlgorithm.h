#pragma once

#include <qdebug.h>

#include <gui/forms/algorithms/embedded/DcAlgDefines.h>
#include <gui/forms/algorithms/embedded/AlgDataModel.h>
#include <report/DcIConfigReport.h>

#include "data_model/dc_controller.h"

class QWidget;

class IAlgorithm
{
public:
	IAlgorithm(int32_t mainParamAddr, int32_t mainParamIdx = 0, bool useMainParamAsSwitcher = true);

	virtual QString name() const = 0;

	void setController(DcController *controller);
	bool isAvailable() const;

	bool isAnyActive() const;
	bool isActive(int group = 0) const;
	void setActive(bool active, int group = 0);
	bool hasSwitcher() const;
	int groupCount() const;

	virtual QWidget* createWidget() const = 0;

	virtual void fillReport(DcIConfigReport *report) const;

protected:
	DcController* controller() const;
    ParameterElement* mainParam(int group = 0) const;

	virtual bool checkForAvailable() const;
	virtual void fillReport(DcReportTable *table) const;

	QWidget* widgetForProfile(int profile = 0) const;
	AlgDataModel* createModel(QObject *parent) const;


	static int alg_index(int profile, int idx) { return profile * 256 + idx; }

private:
	int32_t m_mainParamAddr;
	int32_t m_mainParamIdx;
	bool m_useMainParamAsSwitcher;

	DcController *m_controller;
};

using AlgorithmPtr = std::shared_ptr<IAlgorithm>;
QList<AlgorithmPtr>& algorithmsList();

template<typename T>
class AutoReg {
public:
	template<typename... Args>
	AutoReg(Args... args) { algorithmsList().append(std::make_shared<T>(args...)); }
};

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)

#define ALGORITHMS_REGISTER(TYPE) \
namespace { AutoReg<TYPE> __var; };

#define ALGORITHMS_REGISTER_ARG(TYPE, ...) \
namespace { AutoReg<TYPE> CONCAT(TYPE, __COUNTER__)(__VA_ARGS__); };
