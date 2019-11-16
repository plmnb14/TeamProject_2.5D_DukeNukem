#pragma once

#ifndef __CAMERAOBSERVER_H__

#include "Observer.h"

namespace ENGINE
{
	class CCameraSubject;
}

class CCameraObserver : public ENGINE::CObserver
{
private:
	explicit CCameraObserver();

public:
	virtual ~CCameraObserver();

public:
	const D3DXMATRIX& GetViewMatrix() const;
	const D3DXMATRIX& GetProjMatrix() const;

public:
	virtual void Update(int iMessage) override;

public:
	static CCameraObserver* Create();

private:
	ENGINE::CCameraSubject*	m_pSubject;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProj;

};

#define __CAMERAOBSERVER_H__
#endif