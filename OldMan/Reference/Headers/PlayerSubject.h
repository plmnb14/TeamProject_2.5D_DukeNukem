#pragma once

#ifndef __PLAYERSUBJECT_H__

#include "Subject.h"

BEGIN(ENGINE)

typedef map<int, void*>	MAP_DATA;

class ENGINE_DLL CPlayerSubject : public CSubject
{
	DECLARE_SINGLETON(CPlayerSubject)

private:
	explicit CPlayerSubject();
	virtual ~CPlayerSubject();

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

#define __PLAYERSUBJECT_H__
#endif