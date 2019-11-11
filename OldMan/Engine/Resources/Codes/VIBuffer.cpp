#include "VIBuffer.h"

USING(ENGINE)

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CResources(pGraphicDev),
	m_pVB(nullptr),
	m_pIB(nullptr),
	m_dwVtxFVF(0),
	m_dwVtxSize(0),
	m_dwVtxCount(0),
	m_dwTriCount(0)
{
}

CVIBuffer::~CVIBuffer()
{
	Release();
}

HRESULT CVIBuffer::CreateBuffer()
{
	// 버텍스 버퍼 생성
	HRESULT hr = m_pGraphicDev->CreateVertexBuffer(
		m_dwVtxSize * m_dwVtxCount, 0, m_dwVtxFVF, D3DPOOL_MANAGED, &m_pVB, nullptr);
	FAILED_CHECK_MSG_RETURN(hr, L"CreateVertexBuffer Failed", E_FAIL);

	// 인덱스 버퍼 생성
	hr = m_pGraphicDev->CreateIndexBuffer(
		m_dwIdxSize * m_dwTriCount, 0, m_IdxFmt, D3DPOOL_MANAGED, &m_pIB, nullptr);
	FAILED_CHECK_MSG_RETURN(hr, L"CreateIndexBuffer Failed", E_FAIL);

	return S_OK;
}

void CVIBuffer::Render()
{
	// 장치에 현재 내가 그릴 정점 버퍼를 연결하는 함수.
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(m_dwVtxFVF);

	// 장치에 인덱스버퍼 전달.
	m_pGraphicDev->SetIndices(m_pIB);

	// 장치에 전달한 정점을 이용해서 도형을 그리는 함수.
	m_pGraphicDev->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, /* 그릴 모양 */
		0, /* 전역 버텍스버퍼를 사용할 때의 인덱스 시작점 */
		0, /* 버텍스 버퍼의 시작점 */
		m_dwVtxCount, /* 정점 개수 */
		0, /* 인덱스 버퍼 시작점 */
		m_dwTriCount /* 삼각형 개수 */);
}

CResources* CVIBuffer::CloneResource()
{
	++(*m_pRefCnt);

	return new CVIBuffer(*this); // 얕은 복사
}

void CVIBuffer::Release()
{
	if (0 == (*m_pRefCnt))
	{
		Safe_Release(m_pVB);
		Safe_Release(m_pIB);
	}	
}

VTX_TEX* CVIBuffer::GetVtx(DWORD& _dwVtxCountOut)
{
	_dwVtxCountOut = m_dwVtxCount;
	return m_pVtx;
}
