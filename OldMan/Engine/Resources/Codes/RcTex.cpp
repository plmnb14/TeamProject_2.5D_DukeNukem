#include "RcTex.h"

USING(ENGINE)

CRcTex::CRcTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRcTex::~CRcTex()
{
	Release();
}

HRESULT CRcTex::CreateBuffer()
{
	m_dwVtxSize = sizeof(VTX_TEX);
	m_dwVtxFVF = VTXFVF_TEX;
	m_dwVtxCount = 4;
	m_dwTriCount = 2;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::CreateBuffer(), E_FAIL);

	VTX_TEX* pVtxTex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVtxTex, 0);

	/*
	0	1
	3	2
	*/
	pVtxTex[0].vPos = { -1.f, 1.f, 0.f };

	// UV좌표: 텍스처 영역에 버텍스를 얹기 위한 고유 좌표.
	// 이미지 한장은 0, 0 ~ 1, 1 범위를 갖는다.
	pVtxTex[0].vTex = { 0.f, 0.f };

	pVtxTex[1].vPos = { 1.f, 1.f, 0.f };
	pVtxTex[1].vTex = { 1.f, 0.f };

	pVtxTex[2].vPos = { 1.f, -1.f, 0.f };
	pVtxTex[2].vTex = { 1.f, 1.f };

	pVtxTex[3].vPos = { -1.f, -1.f, 0.f };
	pVtxTex[3].vTex = { 0.f, 1.f };

	m_pVB->Unlock();

	// 인덱스 셋팅
	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	/*
	0	1
	3	2
	*/

	pIndex[0]._1 = 0;
	pIndex[0]._2 = 1;
	pIndex[0]._3 = 2;

	pIndex[1]._1 = 0;
	pIndex[1]._2 = 2;
	pIndex[1]._3 = 3;

	m_pIB->Unlock();
	

	return S_OK;
}

void CRcTex::Render()
{
	CVIBuffer::Render();
}

void CRcTex::Release()
{
}

CRcTex* CRcTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CRcTex* pInstance = new CRcTex(pGraphicDev);

	if (FAILED(pInstance->CreateBuffer()))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
