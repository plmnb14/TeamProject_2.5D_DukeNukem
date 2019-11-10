#include "CubeCol.h"

USING(ENGINE)

CCubeCol::CCubeCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CCubeCol::~CCubeCol()
{
	Release();
}

HRESULT CCubeCol::CreateBuffer()
{
	m_dwVtxSize = sizeof(VTX_COL);
	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;
	m_dwVtxFVF = VTXFVF_COL;
	m_dwTriCount = 12;
	m_dwVtxCount = 8;

	HRESULT hr = CVIBuffer::CreateBuffer();
	FAILED_CHECK_MSG_RETURN(hr, L"CubeCol CreateBuffer Failed", E_FAIL);

	VTX_COL* pVtxCol = nullptr;

	// 버텍스 버퍼가 관리하고 있던 버텍스에 접근하고 제어하기 위해선
	// Lock, Unlock 사이에서 진행한다.
	m_pVB->Lock(0, 0, (void**)&pVtxCol, 0);

	pVtxCol[0].vPos = { -1.f, 1.f, -1.f };
	pVtxCol[0].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	pVtxCol[1].vPos = { 1.f, 1.f, -1.f };
	pVtxCol[1].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	pVtxCol[2].vPos = { 1.f, -1.f, -1.f };
	pVtxCol[2].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	pVtxCol[3].vPos = { -1.f, -1.f, -1.f };
	pVtxCol[3].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	pVtxCol[4].vPos = { -1.f, -1.f, 1.f };
	pVtxCol[4].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	pVtxCol[5].vPos = { -1.f, 1.f, 1.f };
	pVtxCol[5].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	pVtxCol[6].vPos = { 1.f, 1.f, 1.f };
	pVtxCol[6].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	pVtxCol[7].vPos = { 1.f, -1.f, 1.f };
	pVtxCol[7].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	m_pVB->Unlock();

	INDEX32* wIdx = nullptr;
	m_pIB->Lock(0, 0, (void**)&wIdx, 0);

	wIdx[0]._1 = 0;
	wIdx[0]._2 = 1;
	wIdx[0]._3 = 2;

	wIdx[1]._1 = 0;
	wIdx[1]._2 = 2;
	wIdx[1]._3 = 3;

	wIdx[2]._1 = 6;
	wIdx[2]._2 = 5;
	wIdx[2]._3 = 4;

	wIdx[3]._1 = 6;
	wIdx[3]._2 = 4;
	wIdx[3]._3 = 7;

	wIdx[4]._1 = 5;
	wIdx[4]._2 = 0;
	wIdx[4]._3 = 4;

	wIdx[5]._1 = 0;
	wIdx[5]._2 = 3;
	wIdx[5]._3 = 4;

	wIdx[6]._1 = 1;
	wIdx[6]._2 = 6;
	wIdx[6]._3 = 7;

	wIdx[7]._1 = 1;
	wIdx[7]._2 = 7;
	wIdx[7]._3 = 2;

	wIdx[8]._1 = 7;
	wIdx[8]._2 = 4;
	wIdx[8]._3 = 2;

	wIdx[9]._1 = 4;
	wIdx[9]._2 = 3;
	wIdx[9]._3 = 2;

	wIdx[10]._1 = 5;
	wIdx[10]._2 = 6;
	wIdx[10]._3 = 1;

	wIdx[11]._1 = 5;
	wIdx[11]._2 = 1;
	wIdx[11]._3 = 0;

	m_pIB->Unlock();

	return S_OK;
}

void CCubeCol::Render()
{
	CVIBuffer::Render();
}

void CCubeCol::Release()
{
}

CCubeCol* CCubeCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CCubeCol* pInstance = new CCubeCol(pGraphicDev);

	if (FAILED(pInstance->CreateBuffer()))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
