#pragma once

#ifndef __SUBJECT_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class CObserver;
class ENGINE_DLL CSubject
{
protected:
	explicit CSubject();

public:
	virtual ~CSubject();

public:
	virtual void Subscribe(CObserver* pObserver);
	virtual void UnSubscribe(CObserver* pObserver);
	virtual void Notify(int iMessage);

protected:
	void Release();

protected:
	typedef list<CObserver*>	OBSERVER_LIST;
	OBSERVER_LIST	m_ObserverList;
};

END

#define __SUBJECT_H__
#endif