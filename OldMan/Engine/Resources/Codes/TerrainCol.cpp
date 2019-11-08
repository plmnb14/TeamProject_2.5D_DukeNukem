#include "TerrainCol.h"

USING(ENGINE)

CTerrainCol::CTerrainCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTerrainCol::~CTerrainCol()
{
	Release();
}

HRESULT CTerrainCol::CreateBuffer(
	const WORD& wCntX, 
	const WORD& wCntZ, 
	const WORD& wItv)
{
	// 버텍스 전체 개수
	m_dwVtxCount = wCntX * wCntZ;

	// 삼각형 개수
	m_dwTriCount = (wCntX - 1) * (wCntZ - 1) * 2;

	m_dwVtxSize = sizeof(VTX_COL);
	m_dwVtxFVF = VTXFVF_COL;
	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::CreateBuffer(), E_FAIL);

	VTX_COL* pVtxCol = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVtxCol, 0);

	int iIndex = 0;

	for (int i = 0; i < wCntZ; ++i)
	{
		for (int j = 0; j < wCntX; ++j)
		{
			iIndex = i * wCntX + j;

			pVtxCol[iIndex].vPos = { float(j * wItv) , 0.f, float(i * wItv) };
			pVtxCol[iIndex].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	int iTriIndex = 0;

	for (int i = 0; i < wCntZ - 1; ++i)
	{
		for (int j = 0; j < wCntX - 1; ++j)
		{
			iIndex = i * wCntX + j;

			pIndex[iTriIndex]._1 = iIndex + wCntX;
			pIndex[iTriIndex]._2 = iIndex + wCntX + 1;
			pIndex[iTriIndex]._3 = iIndex + 1;
			iTriIndex++;

			pIndex[iTriIndex]._1 = iIndex + wCntX;
			pIndex[iTriIndex]._2 = iIndex + 1;
			pIndex[iTriIndex]._3 = iIndex;
			iTriIndex++;
		}
	}

	m_pIB->Unlock();


	return S_OK;
}

void CTerrainCol::Render()
{
	CVIBuffer::Render();
}

void CTerrainCol::Release()
{
}

CTerrainCol * CTerrainCol::Create(
	LPDIRECT3DDEVICE9 pGraphicDev, 
	const WORD& wCntX, 
	const WORD& wCntZ, 
	const WORD& wItv)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTerrainCol* pInstance = new CTerrainCol(pGraphicDev);

	if (FAILED(pInstance->CreateBuffer(wCntX, wCntZ, wItv)))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
