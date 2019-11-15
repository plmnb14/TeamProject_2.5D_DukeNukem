#pragma once

#ifndef __PLAYEROBSERVER_H__

#include "Observer.h"

namespace ENGINE
{
	class CPlayerSubject;
}

class CPlayerObserver : public ENGINE::CObserver
{
private:
	explicit CPlayerObserver();

public:
	virtual ~CPlayerObserver();

public:
	const ENGINE::INFO GetPlayerInfo() const;

public:
	virtual void Update(int iMessage) override;

public:
	static CPlayerObserver* Create();

private:
	ENGINE::CPlayerSubject*	m_pSubject;
	ENGINE::INFO			m_tInfo;
};

#define __PLAYEROBSERVER_H__
#endif