#pragma once

#ifndef __RESOURCES_H__

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CResources : public CComponent
{
protected:
	explicit CResources(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CResources();

public:
	virtual void Render() PURE;

	// 프로토타입 패턴
	virtual CResources* CloneResource() PURE;


protected:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	WORD*				m_pRefCnt;
};

END

#define __RESOURCES_H__
#endif
