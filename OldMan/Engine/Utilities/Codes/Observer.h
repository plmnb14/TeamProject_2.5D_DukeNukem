#pragma once

#ifndef __OBSERVER_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class ENGINE_DLL CObserver
{
protected:
	explicit CObserver();

public:
	virtual ~CObserver();

public:
	virtual void Update(int iMessage) PURE;
};

END

#define __OBSERVER_H__
#endif