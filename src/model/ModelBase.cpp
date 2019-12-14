#include "ModelBase.h"
#include <typeinfo>
ModelBase::ModelBase()
{
	m_Name = typeid(this).name();
}
