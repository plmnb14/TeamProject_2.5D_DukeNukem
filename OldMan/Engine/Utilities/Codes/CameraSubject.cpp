#include "CameraSubject.h"

USING(ENGINE)

IMPLEMENT_SINGLETON(CCameraSubject)

CCameraSubject::CCameraSubject()
{

}

CCameraSubject::~CCameraSubject()
{
	Release();
}

void* CCameraSubject::GetData(int iMessage)
{
	auto iter_find = m_mapData.find(iMessage);

	if (m_mapData.end() == iter_find)
		return nullptr;

	return iter_find->second;
}

void CCameraSubject::AddData(int iMessage, void * pData)
{
	NULL_CHECK(pData);

	auto iter_find = m_mapData.find(iMessage);

	if (m_mapData.end() != iter_find)
		return;

	m_mapData[iMessage] = pData;
	CSubject::Notify(iMessage);
}

void CCameraSubject::RemoveData(int iMessage)
{
	auto iter_find = m_mapData.find(iMessage);

	if (m_mapData.end() == iter_find)
		return;

	m_mapData.erase(iMessage);
}

void CCameraSubject::Release()
{
	m_mapData.clear();
}
