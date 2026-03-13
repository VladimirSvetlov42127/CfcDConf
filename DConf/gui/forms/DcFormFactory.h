#pragma once

#include <gui/forms/DcForm.h>
#include <gui/forms/DcMenu.h>

class DcFormFactory {
public:	
	using IdType = QString;
    using CreateFuncType = std::function<DcFormPtr(DcController*)>;

	DcFormFactory(const DcFormFactory&) = delete;
	DcFormFactory(DcFormFactory&&) = delete;
	DcFormFactory& operator=(const DcFormFactory&) = delete;
	DcFormFactory& operator=(DcFormFactory&&) = delete;

	static DcFormFactory& instance();
	void registerForm(const IdType &id, CreateFuncType creator);	
    DcFormPtr build(const IdType &id, DcController *controller) const;

private:
	DcFormFactory();

private:
	std::map<IdType, CreateFuncType> m_creators;
};

template<typename T>
class DcFormAutoReg
{
public:
	DcFormAutoReg(const typename DcFormFactory::IdType &id, DcMenu &menu)	{
		if (m_registered++ == 0)
			DcFormFactory::instance().registerForm(id, &createFunc);

		menu.setFormIdx(id);
		menu.setCheckFunc(&T::isAvailableFor);
		menu.setFillReportFunc(&T::fillReport);
	}

private:
    static DcFormPtr createFunc(DcController *controller) {
        return std::make_unique<T>(controller);
	}

	static int m_registered;
};

template<typename T>
int DcFormAutoReg<T>::m_registered = 0;

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)

#define REGISTER_FORM(TYPE, MENU) \
namespace { \
	DcFormAutoReg<TYPE> CONCAT(TYPE, __COUNTER__)(#TYPE, MENU()); \
}
