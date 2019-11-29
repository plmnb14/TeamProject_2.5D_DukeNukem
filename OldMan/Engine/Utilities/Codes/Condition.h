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
	inline void Set_MoveSpeed(float _MoveSpeed) { m_tCondition.fMoveSpeed = _MoveSpeed; };
	inline void Set_MoveAccel(float _MoveAccel) { m_tCondition.fMoveAccel = _MoveAccel; };
	inline void Set_Damage(float _Damage) { m_tCondition.fDamage = _Damage; }

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
	inline void Set_Slide(bool _Slide) { m_tCondition.bSlide = _Slide; }
	inline void Set_Run(bool _Run) { m_tCondition.bRun = _Run; }

public:
	inline float Get_Hp() { return m_tCondition.fHp; };
	inline float Get_Armor() { return m_tCondition.fShield; };
	inline float Get_Shield() { return m_tCondition.fArmor; };
	inline float Get_Fuel() { return m_tCondition.fFuel; };
	inline float Get_Stamina() { return m_tCondition.fStamina; };
	inline float Get_MoveSpeed() { return m_tCondition.fMoveSpeed; };
	inline float Get_MoveAccel() { return m_tCondition.fMoveAccel; };
	inline float Get_Damage() { return m_tCondition.fDamage; }
		 
public:	 
	inline bool Get_Cinematic() { return m_tCondition.bIsCinematic; };
	inline bool Get_Dodge() { return m_tCondition.bIsDodge; };
	inline bool Get_Hit() { return m_tCondition.bIsHit; };
	inline bool Get_Attack() { return m_tCondition.bIsAttck; };
	inline bool Get_JetPack() { return m_tCondition.bIsJetPak; };
	inline bool Get_Invincible() { return m_tCondition.bInvincible; };
	inline bool Get_MeleeAttack() { return m_tCondition.bMeleeAttack; };
	inline bool Get_RangeAttack() { return m_tCondition.bRangeAttack; };
	inline bool Get_SpecialAttack() { return m_tCondition.bSpecialAttack; };
	inline bool Get_Slide() { return m_tCondition.bSlide; }
	inline bool Get_Run() { return m_tCondition.bRun; }


public:
	static CCondition* Create();

private:
	ENGINE::CONDITION m_tCondition;
};

END

#define __CONDITON_H__
#endif