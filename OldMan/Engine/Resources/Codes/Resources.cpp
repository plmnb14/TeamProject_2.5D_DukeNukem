#include "Resources.h"

USING(ENGINE)

CResources::CResources(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_pRefCnt(new WORD(0))
{
	m_pGraphicDev->AddRef();
}

CResources::~CResources()
{
	if (0 == (*m_pRefCnt))
	{
		Safe_Release(m_pGraphicDev);
		Safe_Delete(m_pRefCnt);
	}
	else
		--(*m_pRefCnt);
	
}
