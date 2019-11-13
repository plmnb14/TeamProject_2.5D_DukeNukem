#pragma once

#ifndef __CAMERASUBJECT_H__

#include "Subject.h"

BEGIN(ENGINE)

typedef map<int, void*>	MAP_DATA;

class ENGINE_DLL CCameraSubject : public CSubject
{
	DECLARE_SINGLETON(CCameraSubject)

private:
	explicit CCameraSubject();
	virtual ~CCameraSubject();

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

#define __CAMERASUBJECT_H__
#endif