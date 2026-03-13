#include "DcFormFactory.h"

DcFormFactory & DcFormFactory::instance()
{
	static DcFormFactory factory;
	return factory;
}

void DcFormFactory::registerForm(const IdType & id, CreateFuncType creator)
{
	auto it = m_creators.find(id);
	if (it != m_creators.end())
		return;

	m_creators[id] = creator;
}

DcFormPtr DcFormFactory::build(const IdType & id, DcController * controller) const
{
	auto it = m_creators.find(id);
    if (it != m_creators.end())
        return it->second(controller);

	return DcFormPtr();
}

DcFormFactory::DcFormFactory()
{
}
