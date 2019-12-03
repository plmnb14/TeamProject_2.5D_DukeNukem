#include "stdafx.h"
#include "BossObserver.h"

CBossObserver::CBossObserver()
	: m_pSubject(ENGINE::GetBossSubject())
{
}

CBossObserver::~CBossObserver()
{
}

const ENGINE::CONDITION CBossObserver::GetBossInfo() const
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return m_tInfo;
}

void CBossObserver::Update(int iMessage)
{
	// Pull
	void* pData = m_pSubject->GetData(iMessage);
	NULL_CHECK(pData);

	switch (iMessage)
	{
	case ENGINE::CBossSubject::BOSS_INFO:
		m_tInfo = *reinterpret_cast<ENGINE::CONDITION*>(pData);
		break;
	}
}

CBossObserver* CBossObserver::Create()
{
	return new CBossObserver;
}
