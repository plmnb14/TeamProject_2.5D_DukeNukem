#include "Trasform.h"

USING(ENGINE)

CTransform::CTransform(const D3DXVECTOR3& vLook)
	: m_vLook(vLook)
{
	ZeroMemory(&m_vPos, sizeof(D3DXVECTOR3));
	ZeroMemory(&m_vDir, sizeof(D3DXVECTOR3));
	ZeroMemory(&m_vSize, sizeof(D3DXVECTOR3));
	ZeroMemory(m_fAngle, sizeof(float) * ANGLE_END);

	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matRot[ANGLE_X]);
	D3DXMatrixIdentity(&m_matRot[ANGLE_Y]);
	D3DXMatrixIdentity(&m_matRot[ANGLE_Z]);
	D3DXMatrixIdentity(&m_matTrans);
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::~CTransform()
{
}

const D3DXVECTOR3& CTransform::GetPos() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_vPos;
}

const D3DXVECTOR3& CTransform::GetDir() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_vDir;
}

const D3DXVECTOR3& CTransform::GetSize() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_vSize;
}

const D3DXMATRIX& CTransform::GetWorldMatrix() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_matWorld;
}

const float& CTransform::GetAngle(ANGLE_TYPE eAxis) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_fAngle[eAxis];
}

void CTransform::SetPos(const D3DXVECTOR3& vPos)
{
	m_vPos = vPos;
}

void CTransform::SetDir(const D3DXVECTOR3& vDir)
{
	m_vDir = vDir;
}

void CTransform::SetSize(const D3DXVECTOR3& vSize)
{
	m_vSize = vSize;
}

void CTransform::SetWorldMatrix(const D3DXMATRIX& matWorld)
{
	m_matWorld = matWorld;
}

void CTransform::SetAngle(const float& fAngle, ANGLE_TYPE eAxis)
{
	m_fAngle[eAxis] = fAngle;
}

void CTransform::MovePos(float fSpeed)
{
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	m_vPos += m_vDir * fSpeed;
}

void CTransform::Move_AdvancedPos(D3DXVECTOR3& _vDir,  float _fSpeed)
{
	m_vPos += _vDir * _fSpeed;
}

void CTransform::MoveAngle(ANGLE_TYPE eAxis, float fSpeed)
{
	m_fAngle[eAxis] += fSpeed;
}

void CTransform::LateUpdate()
{
	D3DXMatrixScaling(&m_matScale, m_vSize.x, m_vSize.y, m_vSize.z);
	D3DXMatrixRotationX(&m_matRot[ANGLE_X], D3DXToRadian(m_fAngle[ANGLE_X]));
	D3DXMatrixRotationY(&m_matRot[ANGLE_Y], D3DXToRadian(m_fAngle[ANGLE_Y]));
	D3DXMatrixRotationZ(&m_matRot[ANGLE_Z], D3DXToRadian(m_fAngle[ANGLE_Z]));
	D3DXMatrixTranslation(&m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	m_matWorld = m_matScale * m_matRot[ANGLE_X] * m_matRot[ANGLE_Y] * m_matRot[ANGLE_Z] * m_matTrans;

	D3DXVec3TransformNormal(&m_vDir, &m_vLook, &m_matWorld);	
}

CTransform* CTransform::Create(const D3DXVECTOR3& vLook)
{
	return new CTransform(vLook);
}
