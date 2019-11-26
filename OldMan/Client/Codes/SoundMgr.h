#pragma once

#ifndef __SOUNDMGR_H__

#include "Engine_Include.h"

class CSoundMgr
{
	DECLARE_SINGLETON(CSoundMgr)
public:
	enum CHANNELID{BGM, PLAYER, MONSTER, WEAPON, EFFECT, UI, MAXCHANNEL};
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

private:
	void LoadSouneFile();

private:
	// FMOD_SOUND 리소스 정보를 갖는 객체
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	//사운드, 채널 객체 및 장치를 관리할 객체. 
	FMOD_SYSTEM* m_pSystem; 


};

#define __SOUNDMGR_H__
#endif