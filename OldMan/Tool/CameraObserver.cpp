#include "stdafx.h"
#include "CameraObserver.h"

CCameraObserver::CCameraObserver()
	: m_pSubject(ENGINE::GetCameraSubject())
{
}

CCameraObserver::~CCameraObserver()
{
}

const D3DXMATRIX& CCameraObserver::GetViewMatrix() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_matView;
}

const D3DXMATRIX& CCameraObserver::GetProjMatrix() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_matProj;
}

void CCameraObserver::Update(int iMessage)
{
	// Pull
	void* pData = m_pSubject->GetData(iMessage);
	NULL_CHECK(pData);

	switch (iMessage)
	{
	case D3DTS_VIEW:
		m_matView = *reinterpret_cast<D3DXMATRIX*>(pData);
		break;
	case D3DTS_PROJECTION:
		m_matProj = *reinterpret_cast<D3DXMATRIX*>(pData);
		break;
	}
}

CCameraObserver* CCameraObserver::Create()
{
	return new CCameraObserver;
}
