#pragma once

#ifndef __BOSSSUBJECT_H__

#include "Subject.h"

BEGIN(ENGINE)

typedef map<int, void*>	MAP_DATA;

class ENGINE_DLL CBossSubject : public CSubject
{
	DECLARE_SINGLETON(CBossSubject)

public:
	enum PLAYER_SUBJECT { BOSS_INFO };

private:
	explicit CBossSubject();
	virtual ~CBossSubject();

public:
	void* GetData(int iMessage);

public:
	void AddData(int iMessage, void* pData);
	void RemoveData(int iMessage);

private:
	void Release();

private:
	MAP_DATA	m_mapData;
};

END

#define __BOSSSUBJECT_H__
#endif