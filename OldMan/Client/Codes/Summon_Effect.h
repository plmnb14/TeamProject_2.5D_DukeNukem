#pragma once

#ifndef __SUMMON_EFFECT_H__

#include "Vfx.h"

namespace ENGINE
{
	class CResourceMgr;
	class CTimeMgr;
	class CVIBuffer;
	class CTexture;
	class CTransform;
	class CCollider;
}

class CSummon_Effect : public CVfx
{
protected:
	explicit CSummon_Effect(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual ~CSummon_Effect();

public:
	enum MONSUM
	{
		PIGMAN, OCTABRAIN, TROPPER, ALIEN , MONSTER
	};

public:
	virtual int Update() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

protected:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;
	virtual void Release() override;

public:
	virtual void Set_Pos(D3DXVECTOR3 _Pos);
	virtual void Set_Summon(MONSUM _MonsterType) { m_eMonster = _MonsterType; }

protected:
	HRESULT AddComponent();

public:
	static CSummon_Effect* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3 _Pos , MONSUM _MonsterType);

private:
	WORD	m_wFrame;
	MONSUM	m_eMonster;
	bool	m_bSummon;
};

#define __SUMMON_EFFECT_H__
#endif