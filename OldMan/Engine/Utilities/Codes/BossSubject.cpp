#include "BossSubject.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CBossSubject)

CBossSubject::CBossSubject()
{

}

CBossSubject::~CBossSubject()
{
	Release();
}

void* CBossSubject::GetData(int iMessage)
{
	auto iter_find = m_mapData.find(iMessage);

	if (m_mapData.end() == iter_find)
		return nullptr;

	return iter_find->second;
}

void CBossSubject::AddData(int iMessage, void * pData)
{
	NULL_CHECK(pData);

	auto iter_find = m_mapData.find(iMessage);

	/*if (m_mapData.end() != iter_find)
	return;*/

	m_mapData[iMessage] = pData;
	CSubject::Notify(iMessage);
}

void CBossSubject::RemoveData(int iMessage)
{
	auto iter_find = m_mapData.find(iMessage);

	if (m_mapData.end() == iter_find)
		return;

	m_mapData.erase(iMessage);
}

void CBossSubject::Release()
{
	m_mapData.clear();
}
