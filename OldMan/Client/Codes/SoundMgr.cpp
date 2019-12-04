#include "stdafx.h"
#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
	: m_fVolumeAmp(1) , m_fVolumeValue(0)
{
	m_pSystem = nullptr;
}


CSoundMgr::~CSoundMgr()
{
	Release();
}

void CSoundMgr::Initialize()
{
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSouneFile();
}

void CSoundMgr::Release()
{
	for (auto& MyPair : m_mapSound)
	{
		delete[] MyPair.first; 
		FMOD_Sound_Release(MyPair.second); 
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

void CSoundMgr::MyPlaySound(TCHAR * pSoundKey, CHANNELID eID, bool bIsCanMulti)
{
	if (m_mapSound.empty())
		return; 

	auto& iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(iter.first, pSoundKey);
	});

	if (m_mapSound.end() == iter)
		return; 

	if (bIsCanMulti)
	{
		FMOD_BOOL IsPlay;
		if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &IsPlay))
		{
			FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);
		}
	}
	else
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);

	FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_OFF);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::MyPlayBGM(TCHAR * pSoundKey)
{
	if (m_mapSound.empty())
		return; 

	auto& iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(iter.first, pSoundKey);
	});

	if (m_mapSound.end() == iter)
		return;


	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[BGM]);

	FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);

}

void CSoundMgr::StopAll()
{
	for(int i = 0 ; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetVolume(CHANNELID eID, float _fVolume)
{
	if (_fVolume <= 0)_fVolume = 0;
	FMOD_Channel_SetVolume(m_pChannelArr[eID], _fVolume);
}

void CSoundMgr::Set_MasterVolume(float _Value)
{
	m_fVolumeAmp = _Value;

	for (int i = 0; i < MAXCHANNEL; ++i)
	{
		FMOD_Channel_GetVolume(m_pChannelArr[i], &m_fVolumeValue);
		FMOD_Channel_SetVolume(m_pChannelArr[i], m_fVolumeAmp);
	}

}

void CSoundMgr::SetVolume_Test(CHANNELID eID, float _fVolume)
{
	if (_fVolume <= 0)_fVolume = 0;
	FMOD_Channel_SetVolume(m_pChannelArr[eID], _fVolume);
}

void CSoundMgr::LoadSouneFile()
{
	_finddata_t fd; 

	long Handle = _findfirst("../Sound/*.*", &fd);
	if (Handle == 0)
		return; 

	int iResult = 0; 

	char szCurPath[128] = "../Sound/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr; 

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1; 
			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR)*iLength);
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _findnext(Handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
}
