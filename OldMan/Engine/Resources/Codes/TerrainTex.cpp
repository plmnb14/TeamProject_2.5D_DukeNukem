#include "TerrainTex.h"

USING(ENGINE)

CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTerrainTex::~CTerrainTex()
{
	Release();
}

HRESULT CTerrainTex::CreateBuffer(
	const WORD& wCntX, 
	const WORD& wCntZ, 
	const WORD& wItv)
{
	// 버텍스 전체 개수
	m_dwVtxCount = wCntX * wCntZ;

	// 삼각형 개수
	m_dwTriCount = (wCntX - 1) * (wCntZ - 1) * 2;

	m_dwVtxSize = sizeof(VTX_TEX);
	m_dwVtxFVF = VTXFVF_TEX;
	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::CreateBuffer(), E_FAIL);

	DWORD* pPixel = LoadHeightMap();
	NULL_CHECK_RETURN(pPixel, E_FAIL);

	VTX_TEX* pVtxTex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVtxTex, 0);

	int iIndex = 0;

	for (int i = 0; i < wCntZ; ++i)
	{
		for (int j = 0; j < wCntX; ++j)
		{
			iIndex = i * wCntX + j;

			pVtxTex[iIndex].vPos = 
			{ 
				float(j * wItv), 
				(pPixel[iIndex] & 0x000000ff) / 100.f,
				float(i * wItv) 
			};


			pVtxTex[iIndex].vTex =
			{
				pVtxTex[iIndex].vPos.x / (wCntX - 1),
				1.f - pVtxTex[iIndex].vPos.z / (wCntZ - 1)
			};
		}
	}

	m_pVB->Unlock();

	Safe_Delete_Array(pPixel);

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

void CTerrainTex::Render()
{
	CVIBuffer::Render();
}

DWORD* CTerrainTex::LoadHeightMap()
{
	HANDLE hFile = CreateFile(L"../Texture/Terrain/Height.bmp", GENERIC_READ,
		0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return nullptr;

	DWORD dwBytes = 0;

	BITMAPFILEHEADER	tBmpFileHeader;	// 비트맵 파일에 대한 정보
	BITMAPINFOHEADER	tBmpInfoHeader;	// 비트맵 이미지에 대한 정보

	ReadFile(hFile, &tBmpFileHeader, sizeof(BITMAPFILEHEADER), &dwBytes, nullptr);
	ReadFile(hFile, &tBmpInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytes, nullptr);

	// 비트맵이 가진 픽셀들을 불러옴.
	DWORD* pPixel = new DWORD[tBmpInfoHeader.biWidth * tBmpInfoHeader.biHeight];

	ReadFile(hFile, pPixel, sizeof(DWORD) * tBmpInfoHeader.biWidth * tBmpInfoHeader.biHeight, &dwBytes, nullptr);

	CloseHandle(hFile);

	return pPixel;
}

void CTerrainTex::Release()
{
}

CTerrainTex* CTerrainTex::Create(
	LPDIRECT3DDEVICE9 pGraphicDev, 
	const WORD& wCntX, 
	const WORD& wCntZ, 
	const WORD& wItv)
{
	NULL_CHECK_RETURN(pGraphicDev, nullptr);

	CTerrainTex* pInstance = new CTerrainTex(pGraphicDev);

	if (FAILED(pInstance->CreateBuffer(wCntX, wCntZ, wItv)))
	{
		Safe_Delete(pInstance);
		return nullptr;
	}

	return pInstance;
}
