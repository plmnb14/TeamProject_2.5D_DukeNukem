#include "Condition.h"

USING(ENGINE)

CCondition::CCondition()
{
	ZeroMemory(&m_tCondition, sizeof(CONDITION));
}

CCondition::~CCondition()
{
}

void CCondition::LateUpdate()
{
}

CCondition * CCondition::Create()
{
	return new CCondition();
}
