#include "TriCol.h"

USING(ENGINE)

CTriCol::CTriCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTriCol::~CTriCol()
{
	Release();
}

HRESULT CTriCol::CreateBuffer()
{
	m_dwVtxSize = sizeof(VTX_COL);
	m_dwVtxFVF = VTXFVF_COL;
	m_dwTriCount = 1;
	m_dwVtxCount = 3;

	HRESULT hr = CVIBuffer::CreateBuffer();
	FAILED_CHECK_MSG_RETURN(hr, L"TriCol CreateBuffer Failed", E_FAIL);

	VTX_COL* pVtxCol = nullptr;

	// ���ؽ� ���۰� �����ϰ� �ִ� ���ؽ��� �����ϰ� �����ϱ� ���ؼ�
	// Lock, Unlock ���̿��� �����Ѵ�.
	m_pVB->Lock(0, 0, (void**)&pVtxCol, 0);

	pVtxCol[0].vPos = { 0.f, 1.f, 0.f };
	pVtxCol[0].dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVtxCol[1].vPos = { 1.f, -1.f, 0.f };
	pVtxCol[1].dwColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVtxCol[2].vPos = { -1.f, -1.f, 0.f };
	pVtxCol[2].dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);

	m_pVB->Unlock();

	return S_OK;
}

void CTriCol::Render()
{
	CVIBuffer::Render();
}

void CTriCol::Release()
{
}

CTriCol* CTriCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTriCol* pInstance = new CTriCol(pGraphicDev);

	if (FAILED(pInstance->CreateBuffer()))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
