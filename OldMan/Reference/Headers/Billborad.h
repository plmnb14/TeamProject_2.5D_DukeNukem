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

private:      // 필요한것 1.받을 행렬. 2.템프 행렬
	          // 그다음 필요한것 회전 이동 크기 
		D3DXMATRIX matBillView;
	
	
};

END

#define __BILLBORAD_H__
#endif