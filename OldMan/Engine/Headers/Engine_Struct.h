#pragma once

#ifndef __ENGINE_STRUCT_H__

namespace ENGINE
{
	typedef struct tagVertexColor
	{
		D3DXVECTOR3 vPos;		// ���ؽ� ��ġ
		DWORD		dwColor;	// ���ؽ� ����
	}VTX_COL;

	const DWORD VTXFVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		D3DXVECTOR3	vPos;
		D3DXVECTOR2 vTex;	// UV��ǥ.
	}VTX_TEX;

	const DWORD VTXFVF_TEX = D3DFVF_XYZ | D3DFVF_TEX1 ;

	typedef struct tagIndex16
	{
		WORD _1, _2, _3;
	}INDEX16;

	typedef struct tagIndex32
	{
		DWORD _1, _2, _3;
	}INDEX32;

	typedef struct tagInfo
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vDir;
		D3DXVECTOR3 vLook;
		D3DXVECTOR3 vSize;
		D3DXMATRIX matWorld;
	}INFO;

	typedef struct tagAbility	// �ɷ�ġ, [ ���� / �Ұ��� ] ����
	{
		float fHitPoint;		// HP ( HitPoint)	 ü�� ��ġ
		float fShieldPoint;		// SP ( ShieldPoint) ���� ��ġ

		bool bCanAttack;		// ���� ���� ����
		bool bCanHit;			// �ǰ� ���� ����
		bool bCanMove;			// �̵� ���� ����

		bool bIsDead;
		bool bIsAttack;
		bool bIsHit;

	}ABILITY;

	typedef struct tagPhysic
	{
		float fDistance;		// �Ÿ�
		float fLength;			// ����

		float fCombine_Angle;	// ���� ����
		float fX_Angle;			// X �� ����
		float fY_Angle;			// Y �� ����
		float fZ_Angle;			// Z �� ����

		float fRadian;			// ����

		float fAccel;			// ���ӵ�
		float fSpeed;			// �ӵ�
		float fPower;			// ��
		float fJumpPower;		// ���� ��

	}PHYSIC;

	typedef struct tagCamera
	{
		D3DXVECTOR3 vRight;
		D3DXVECTOR3 vUp;
		D3DXVECTOR3 vLook;

		D3DXVECTOR3 vEyePos;
		D3DXVECTOR3 vLookAt;

		int			iIndex;
		float		fDistance;

		bool		bMainCamera;

	}CAM_INFO;


	typedef struct tagBoxCollider
	{
		D3DXVECTOR3 vCenterPos;
		D3DXVECTOR3 vUnderPos;

		// AABB �浹 �˻� �� ���
		D3DXVECTOR3 vMaxPos;
		D3DXVECTOR3 vMinPos;

		D3DXVECTOR3 vLength;
		D3DXVECTOR3 vRadius;

		bool bIsDynamic;
		bool bIsCollision;
		bool bIsTrigger;
		bool bIsEnabled;

	}BOXCOL;

	typedef struct tagTexture
	{
		// �ҷ��� �ؽ�ó�� �����ϱ� ���� Com��ü.
		LPDIRECT3DTEXTURE9	pTexture;

		// �ҷ��� �ؽ�ó�� ����
		D3DXIMAGE_INFO		tImgInfo;
	}TEX_INFO;

	typedef struct tagPathInfo
	{
		wstring wstrObjectKey = L"";
		wstring wstrStateKey = L"";
		wstring wstrImgPath = L"";
		wstring wstrFileName = L"";
		int iImgCount = 0;
	}PATH_INFO;
}

#define __ENGINE_STRUCT_H__
#endif