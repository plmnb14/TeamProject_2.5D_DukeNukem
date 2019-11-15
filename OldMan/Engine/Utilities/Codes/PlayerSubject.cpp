#include "PlayerSubject.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CPlayerSubject)

CPlayerSubject::CPlayerSubject()
{

}

CPlayerSubject::~CPlayerSubject()
{
	Release();
}

void* CPlayerSubject::GetData(int iMessage)
{
	auto iter_find = m_mapData.find(iMessage);

	if (m_mapData.end() == iter_find)
		return nullptr;

	return iter_find->second;
}

void CPlayerSubject::AddData(int iMessage, void * pData)
{
	NULL_CHECK(pData);

	auto iter_find = m_mapData.find(iMessage);

	/*if (m_mapData.end() != iter_find)
	return;*/

	m_mapData[iMessage] = pData;
	CSubject::Notify(iMessage);
}

void CPlayerSubject::RemoveData(int iMessage)
{
	auto iter_find = m_mapData.find(iMessage);

	if (m_mapData.end() == iter_find)
		return;

	m_mapData.erase(iMessage);
}

void CPlayerSubject::Release()
{
	m_mapData.clear();
}
