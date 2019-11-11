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
	// ���ؽ� ���� ����
	HRESULT hr = m_pGraphicDev->CreateVertexBuffer(
		m_dwVtxSize * m_dwVtxCount, 0, m_dwVtxFVF, D3DPOOL_MANAGED, &m_pVB, nullptr);
	FAILED_CHECK_MSG_RETURN(hr, L"CreateVertexBuffer Failed", E_FAIL);

	// �ε��� ���� ����
	hr = m_pGraphicDev->CreateIndexBuffer(
		m_dwIdxSize * m_dwTriCount, 0, m_IdxFmt, D3DPOOL_MANAGED, &m_pIB, nullptr);
	FAILED_CHECK_MSG_RETURN(hr, L"CreateIndexBuffer Failed", E_FAIL);

	return S_OK;
}

void CVIBuffer::Render()
{
	// ��ġ�� ���� ���� �׸� ���� ���۸� �����ϴ� �Լ�.
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(m_dwVtxFVF);

	// ��ġ�� �ε������� ����.
	m_pGraphicDev->SetIndices(m_pIB);

	// ��ġ�� ������ ������ �̿��ؼ� ������ �׸��� �Լ�.
	m_pGraphicDev->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, /* �׸� ��� */
		0, /* ���� ���ؽ����۸� ����� ���� �ε��� ������ */
		0, /* ���ؽ� ������ ������ */
		m_dwVtxCount, /* ���� ���� */
		0, /* �ε��� ���� ������ */
		m_dwTriCount /* �ﰢ�� ���� */);
}

CResources* CVIBuffer::CloneResource()
{
	++(*m_pRefCnt);

	return new CVIBuffer(*this); // ���� ����
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
