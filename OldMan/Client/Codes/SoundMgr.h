#pragma once

#ifndef __SOUNDMGR_H__

#include "Engine_Include.h"

class CSoundMgr
{
	DECLARE_SINGLETON(CSoundMgr)

public:
	enum CHANNELID{
		BGM,
		PLAYER_WALK, PLAYER, PLAYER_VOICE,
		MONSTER, MONSTER_VOICE, MONSTER_EFF,
		WEAPON, WEAPON_AFTER , BULLET_SHOOT, BULLET_DEAD,
		ENVIRONMENT, WOOD_BOX,
		VEHICLE,
		EFFECT,
		UI,
		MAXCHANNEL};
private:
	CSoundMgr();
	~CSoundMgr();

public:
	void Initialize(); 
	void Release(); 

public:
	void MyPlaySound(TCHAR* pSoundKey, CHANNELID eID, bool bIsCanMulti = false);
	void MyPlayBGM(TCHAR* pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetVolume(CHANNELID eID, float _fVolume);

public:
	void Set_MasterVolume(float _Value);


private:
	void LoadSouneFile();

private:
	// FMOD_SOUND ���ҽ� ������ ���� ��ü
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	//����, ä�� ��ü �� ��ġ�� ������ ��ü. 
	FMOD_SYSTEM* m_pSystem; 

	float m_fVolumeAmp;
	float m_fVolumeValue;


};

#define __SOUNDMGR_H__
#endif