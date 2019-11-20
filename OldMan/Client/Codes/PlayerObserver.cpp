#include "stdafx.h"
#include "PlayerObserver.h"

CPlayerObserver::CPlayerObserver()
	: m_pSubject(ENGINE::GetPlayerSubject())
{
}

CPlayerObserver::~CPlayerObserver()
{
}

const ENGINE::ABILITY CPlayerObserver::GetPlayerInfo() const
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return m_tInfo;
}

const ENGINE::W_INFO CPlayerObserver::GetWeaponInfo() const
{
	return m_tWeaponInfo;
}

void CPlayerObserver::Update(int iMessage)
{
	// Pull
	void* pData = m_pSubject->GetData(iMessage);
	NULL_CHECK(pData);

	switch (iMessage)
	{
	case ENGINE::CPlayerSubject::PLAYER_INFO:
		m_tInfo = *reinterpret_cast<ENGINE::ABILITY*>(pData);
		break;
	case ENGINE::CPlayerSubject::WEAPON_INFO:
		m_tWeaponInfo = *reinterpret_cast<ENGINE::W_INFO*>(pData);
		break;
	}
}

CPlayerObserver* CPlayerObserver::Create()
{
	return new CPlayerObserver;
}
