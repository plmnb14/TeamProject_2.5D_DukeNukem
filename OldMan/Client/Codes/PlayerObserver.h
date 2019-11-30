#pragma once

#ifndef __PLAYEROBSERVER_H__

#include "Observer.h"

namespace ENGINE
{
	class CPlayerSubject;
	class CCondition;
}

class CPlayerObserver : public ENGINE::CObserver
{
private:
	explicit CPlayerObserver();

public:
	virtual ~CPlayerObserver();

public:
	const ENGINE::CONDITION GetPlayerInfo() const;
	const ENGINE::W_INFO  GetWeaponInfo() const;
	const int GetGrenadeCount() const;

public:
	virtual void Update(int iMessage) override;

public:
	static CPlayerObserver* Create();

private:
	ENGINE::CPlayerSubject*	m_pSubject;
	ENGINE::CONDITION		m_tInfo;
	ENGINE::W_INFO			m_tWeaponInfo;
	int						m_iGrenadeCount;
};

#define __PLAYEROBSERVER_H__
#endif