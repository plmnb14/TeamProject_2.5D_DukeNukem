#pragma once

#ifndef __VIBUFFER_H__

#include "Resources.h"

BEGIN(ENGINE)

class ENGINE_DLL CVIBuffer : public CResources
{
public:
	enum BUFFER_TYPE
	{
		BUFFER_TRICOL,
		BUFFER_RCCOL,
		BUFFER_RCTEX,
		BUFFER_TERRAIN,
		BUFFER_TERRAINTEX,
	};

protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CVIBuffer();

public:
	virtual HRESULT CreateBuffer();
	virtual void Render();

	// ������Ÿ�� ����
	virtual CResources* CloneResource();

private:
	void Release();

protected:
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	DWORD		m_dwVtxFVF;
	// FVF (Flexible Vertex Format, ������ ���� ����)

	DWORD		m_dwVtxSize;	// ���ؽ� �޸� ũ��
	DWORD		m_dwVtxCount;	// ���ؽ� ����
	DWORD		m_dwTriCount;	// �ﰢ�� ���� (������ ����)

	DWORD		m_dwIdxSize;	// �ε��� �޸� ũ��.
	D3DFORMAT	m_IdxFmt;	// �ε��� ���� (16��Ʈ or 32��Ʈ)
};

END

#define __VIBUFFER_H__
#endif