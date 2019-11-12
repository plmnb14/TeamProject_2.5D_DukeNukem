#include "Collider.h"

USING(ENGINE)

CCollider::CCollider(D3DXVECTOR3 _Pos, D3DXVECTOR3 _MinPos, D3DXVECTOR3 _MaxPos)
	: m_pTarget(nullptr)
{
	ZeroMemory(&m_tBoxCollider, sizeof(BOXCOL));
}

CCollider::~CCollider()
{
}

void CCollider::LateUpdate()
{
}

CCollider* CCollider::Create(D3DXVECTOR3 _Pos, D3DXVECTOR3 _MinPos, D3DXVECTOR3 _MaxPos)
{
	return new CCollider(_Pos , _MinPos, _MaxPos);
}
