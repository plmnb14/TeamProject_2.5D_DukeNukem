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
	// FMOD_SOUND ���ҽ� ������ ���� ��ü
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	//����, ä�� ��ü �� ��ġ�� ������ ��ü. 
	FMOD_SYSTEM* m_pSystem; 


};

#define __SOUNDMGR_H__
#endif