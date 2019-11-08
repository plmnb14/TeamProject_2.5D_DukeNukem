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

	// 프로토타입 패턴
	virtual CResources* CloneResource();

private:
	void Release();

protected:
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	DWORD		m_dwVtxFVF;
	// FVF (Flexible Vertex Format, 유연한 정점 포맷)

	DWORD		m_dwVtxSize;	// 버텍스 메모리 크기
	DWORD		m_dwVtxCount;	// 버텍스 개수
	DWORD		m_dwTriCount;	// 삼각형 개수 (폴리곤 개수)

	DWORD		m_dwIdxSize;	// 인덱스 메모리 크기.
	D3DFORMAT	m_IdxFmt;	// 인덱스 포맷 (16비트 or 32비트)
};

END

#define __VIBUFFER_H__
#endif