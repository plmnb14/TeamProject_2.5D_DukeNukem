#pragma once

#ifndef __COMPONENT_H__

#include "Engine_Include.h"

BEGIN(ENGINE)

class ENGINE_DLL CComponent
{
protected:
	CComponent();

public:
	virtual ~CComponent() PURE;

public:
	virtual void Update() {}
	virtual void LateUpdate() {}

};

END

#define __COMPONENT_H__
#endif