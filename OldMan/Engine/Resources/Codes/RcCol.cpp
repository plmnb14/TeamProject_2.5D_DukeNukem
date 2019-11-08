#include "RcCol.h"

USING(ENGINE)

CRcCol::CRcCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRcCol::~CRcCol()
{
	Release();
}

HRESULT CRcCol::CreateBuffer()
{
	m_dwVtxSize = sizeof(VTX_COL);
	m_dwVtxFVF = VTXFVF_COL;
	m_dwTriCount = 2;
	m_dwVtxCount = 4;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	HRESULT hr = CVIBuffer::CreateBuffer();
	FAILED_CHECK_MSG_RETURN(hr, L"RcCol CreateBuffer Failed", E_FAIL);

	// 버텍스 셋팅
	VTX_COL* pVtxCol = nullptr;

	// 버텍스 버퍼가 관리하고 있던 버텍스에 접근하고 제어하기 위해선
	// Lock, Unlock 사이에서 진행한다.
	m_pVB->Lock(0, 0, (void**)&pVtxCol, 0);
	
	/*
	0	1
	3	2
	*/

	pVtxCol[0].vPos = { -1.f, 1.f, 0.f };
	pVtxCol[0].dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVtxCol[1].vPos = { 1.f, 1.f, 0.f };
	pVtxCol[1].dwColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVtxCol[2].vPos = { 1.f, -1.f, 0.f };
	pVtxCol[2].dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);	

	pVtxCol[3].vPos = { -1.f, -1.f, 0.f };
	pVtxCol[3].dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

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

void CRcCol::Render()
{
	CVIBuffer::Render();
}

void CRcCol::Release()
{
}

CRcCol* CRcCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CRcCol* pInstance = new CRcCol(pGraphicDev);

	if (FAILED(pInstance->CreateBuffer()))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
