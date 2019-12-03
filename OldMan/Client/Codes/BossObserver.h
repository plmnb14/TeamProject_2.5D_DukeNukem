#pragma once

#ifndef __BOSSOBSERVER_H__

#include "Observer.h"

namespace ENGINE
{
	class CBossSubject;
	class CCondition;
}

class CBossObserver : public ENGINE::CObserver
{
private:
	explicit CBossObserver();

public:
	virtual ~CBossObserver();

public:
	const ENGINE::CONDITION GetBossInfo() const;

public:
	virtual void Update(int iMessage) override;

public:
	static CBossObserver* Create();

private:
	ENGINE::CBossSubject*	m_pSubject;
	ENGINE::CONDITION		m_tInfo;
};

#define __BOSSOBSERVER_H__
#endif