#include "stdafx.h"
#include "PlayerObserver.h"

CPlayerObserver::CPlayerObserver()
	: m_pSubject(ENGINE::GetPlayerSubject()),
	m_iGrenadeCount(0)
{
}

CPlayerObserver::~CPlayerObserver()
{
}

const ENGINE::CONDITION CPlayerObserver::GetPlayerInfo() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_tInfo;
}

const ENGINE::W_INFO CPlayerObserver::GetWeaponInfo() const
{
	return m_tWeaponInfo;
}

const int CPlayerObserver::GetGrenadeCount() const
{
	return m_iGrenadeCount;
}

void CPlayerObserver::Update(int iMessage)
{
	// Pull
	void* pData = m_pSubject->GetData(iMessage);
	NULL_CHECK(pData);

	switch (iMessage)
	{
	case ENGINE::CPlayerSubject::PLAYER_INFO:
		m_tInfo = *reinterpret_cast<ENGINE::CONDITION*>(pData);
		break;
	case ENGINE::CPlayerSubject::WEAPON_INFO:
		m_tWeaponInfo = *reinterpret_cast<ENGINE::W_INFO*>(pData);
		break;
	case ENGINE::CPlayerSubject::GRENADE_COUNT:
		m_iGrenadeCount = *reinterpret_cast<int*>(pData);
	}
}

CPlayerObserver* CPlayerObserver::Create()
{
	return new CPlayerObserver;
}
