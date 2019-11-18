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

void CPlayerObserver::Update(int iMessage)
{
	// Pull
	void* pData = m_pSubject->GetData(iMessage);
	NULL_CHECK(pData);

	switch (iMessage)
	{
	case 0:
		m_tInfo = *reinterpret_cast<ENGINE::ABILITY*>(pData);
		break;
	}
}

CPlayerObserver* CPlayerObserver::Create()
{
	return new CPlayerObserver;
}
