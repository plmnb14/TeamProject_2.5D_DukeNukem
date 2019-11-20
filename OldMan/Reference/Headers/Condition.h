#pragma once

#ifndef __CONDITON_H__

#include "Component.h"

BEGIN(ENGINE)

class ENGINE_DLL CCondition : public CComponent
{
private:
	explicit CCondition();

public:
	virtual ~CCondition();

public:
	virtual void LateUpdate();

public:
	inline void Add_Hp(float _Hp) { m_tCondition.fHp += _Hp; };
	inline void Add_Armor(float _Armor) { m_tCondition.fShield += _Armor; };
	inline void Add_Shield(float _Shield) { m_tCondition.fArmor += _Shield; };
	inline void Add_Fuel(float _Fuel) { m_tCondition.fFuel += _Fuel; };
	inline void Add_Stamina(float _Stamina) { m_tCondition.fStamina += _Stamina; };

public:
	inline void Set_Hp(float _Hp) { m_tCondition.fHp = _Hp; };
	inline void Set_Armor(float _Armor) { m_tCondition.fShield = _Armor; };
	inline void Set_Shield(float _Shield) { m_tCondition.fArmor = _Shield; };
	inline void Set_Fuel(float _Fuel) { m_tCondition.fFuel = _Fuel; };
	inline void Set_Stamina(float _Stamina) { m_tCondition.fStamina = _Stamina; };

public:
	inline void Set_Cinematic(bool _Cinematic) { m_tCondition.bIsCinematic = _Cinematic; };
	inline void Set_Dodge(bool _Dodge) { m_tCondition.bIsDodge = _Dodge; };
	inline void Set_Hit(bool _Hit) { m_tCondition.bIsHit = _Hit; };
	inline void Set_Attack(bool _Attack) { m_tCondition.bIsAttck = _Attack; };
	inline void Set_JetPack(bool _Jetpack) { m_tCondition.bIsJetPak = _Jetpack; };
	inline void Set_Invincible(bool _Invincible) { m_tCondition.bInvincible = _Invincible; };
	inline void Set_MeleeAttack(bool _MeleeAttack) { m_tCondition.bMeleeAttack = _MeleeAttack; };
	inline void Set_RangeAttack(bool _RangeAttack) { m_tCondition.bRangeAttack = _RangeAttack; };
	inline void Set_SpecialAttack(bool _SpecialAttack) { m_tCondition.bSpecialAttack = _SpecialAttack; };

public:
	inline float Get_Hp(float _Hp) { return m_tCondition.fHp; };
	inline float Get_Armor(float _Armor) { return m_tCondition.fShield; };
	inline float Get_Shield(float _Shield) { return m_tCondition.fArmor; };
	inline float Get_Fuel(float _Fuel) { return m_tCondition.fFuel; };
	inline float Get_Stamina(float _Stamina) { return m_tCondition.fStamina; };
		 
public:	 
	inline bool Get_Cinematic(bool _Cinematic) { return m_tCondition.bIsCinematic; };
	inline bool Get_Dodge(bool _Dodge) { return m_tCondition.bIsDodge; };
	inline bool Get_Hit() { return m_tCondition.bIsHit; };
	inline bool Get_Attack(bool _Attack) { return m_tCondition.bIsAttck; };
	inline bool Get_JetPack(bool _Jetpack) { return m_tCondition.bIsJetPak; };
	inline bool Get_Invincible(bool _Invincible) { return m_tCondition.bInvincible; };
	inline bool Get_MeleeAttack(bool _MeleeAttack) { return m_tCondition.bMeleeAttack; };
	inline bool Get_RangeAttack(bool _RangeAttack) { return m_tCondition.bRangeAttack; };
	inline bool Get_SpecialAttack(bool _SpecialAttack) { return m_tCondition.bSpecialAttack; };


public:
	static CCondition* Create();

private:
	ENGINE::CONDITION m_tCondition;
};

END

#define __CONDITON_H__
#endif