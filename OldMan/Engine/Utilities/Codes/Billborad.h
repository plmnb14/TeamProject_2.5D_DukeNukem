#pragma once

#ifndef __BILLBORAD_H__

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CBillborad : public CComponent
{
private:
	explicit CBillborad();

public:
	virtual ~CBillborad();

public:
	const D3DXMATRIX& GetWorldMatrix_Billborad() const;
	
public:
	void Billborad_Yagnle(const D3DXMATRIX& _matWorld, const D3DXMATRIX& _matCamera, D3DXVECTOR3 _vSize);
	void Billborad_Front(const D3DXMATRIX& _matWorld, const D3DXMATRIX& _matCamera, D3DXVECTOR3 _vSize);

public:
	

public:
	virtual void LateUpdate();

public:
	static CBillborad* Create();

private:      // �ʿ��Ѱ� 1.���� ���. 2.���� ���
	          // �״��� �ʿ��Ѱ� ȸ�� �̵� ũ�� 
		D3DXMATRIX matBillView;
	
	
};

END

#define __BILLBORAD_H__
#endif