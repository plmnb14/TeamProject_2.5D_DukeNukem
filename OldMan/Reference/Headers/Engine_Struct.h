#pragma once

#ifndef __ENGINE_STRUCT_H__

namespace ENGINE
{
	typedef struct tagVertexColor
	{
		D3DXVECTOR3 vPos;		// 버텍스 위치
		DWORD		dwColor;	// 버텍스 색상
	}VTX_COL;

	const DWORD VTXFVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		D3DXVECTOR3	vPos;
		D3DXVECTOR2 vTex;	// UV좌표.
	}VTX_TEX;

	const DWORD VTXFVF_TEX = D3DFVF_XYZ | D3DFVF_TEX1 ;

	typedef struct tagVertexCube
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vTex;
	}VTX_CUBE;

	const DWORD VTXFVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

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

	typedef struct tagAbility	// 능력치, [ 가능 / 불가능 ] 여부
	{
		float fHitPoint;		// HP ( HitPoint)	 체력 수치
		float fShieldPoint;		// SP ( ShieldPoint) 쉴드 수치

		bool bCanAttack;		// 공격 가능 여부
		bool bCanHit;			// 피격 가능 여부
		bool bCanMove;			// 이동 가능 여부

		bool bIsDead;
		bool bIsAttack;
		bool bIsHit;

	}ABILITY;

	typedef struct tagPhysic
	{
		float fDistance;		// 거리
		float fLength;			// 길이

		float fCombine_Angle;	// 통합 각도
		float fX_Angle;			// X 축 각도
		float fY_Angle;			// Y 축 각도
		float fZ_Angle;			// Z 축 각도

		float fRadian;			// 라디안

		float fAccel;			// 가속도
		float fSpeed;			// 속도
		float fPower;			// 힘
		float fJumpPower;		// 점프 힘

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

		// AABB 충돌 검사 시 사용
		D3DXVECTOR3 vMaxPos;
		D3DXVECTOR3 vMinPos;

		D3DXVECTOR3 vLength;
		D3DXVECTOR3 vRadius;

		bool bIsDynamic;
		bool bIsCollision;
		bool bIsTrigger;
		bool bIsEnabled;

	}BOXCOL;

	typedef struct tagRigidBody
	{
		D3DXVECTOR3 vAccel;
		D3DXVECTOR3 vMaxAccel;
		D3DXVECTOR3 vSpeed;
		D3DXVECTOR3 vMaxSpeed;

		bool bIsGravity; // 중력 영향력을 체크합니다.

		bool bIsGround;	 // 땅인지 체크 합니다.
		bool bIsAir;	 // 공중인지 체크합니다.
		bool bIsLanding; // 착지 시

		bool bIsHit;	 // 피격 체크
		bool bIsAttck;	 // 공격 가능체크
		bool bIsFall;    // 낙하 체크
		bool bIsJump;	 // 점프 체크

		float fPower;	// 힘
		float fMass;	// 질량
		float fForce;	// 넉백할 량

	}RIGID;

	typedef struct tagTexture
	{
		// 불러온 텍스처를 조작하기 위한 Com객체.
		LPDIRECT3DTEXTURE9	pTexture;

		// 불러올 텍스처의 정보
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

	typedef struct tagCondition
	{
		float fHp;			// 체력 수치
		float fShield;		// 보호막 수치
		float fArmor;		// 방어력 수치 ( 방어력만큼 공격력이 감소 됩니다. )
		float fStamina;		// 스태미너 ( 달리기, 슬라이드 등 행동관련 )

		float fMoveSpeed;
		float fMoveAccel;

		float fFuel;		// 연료 게이지 ( 제트팩이나, 특수 무기 장착시 활성화 )

		bool bIsCinematic;	// 연출 중 인지 ( 연출 중엔 무적 상태가되고, 조작이 불가능 ) 

		bool bIsDodge;		// 회피 상태
		bool bIsHit;		// 피격 체크
		bool bIsAttck;		// 공격 가능체크
		bool bIsJetPak;		// 제트팩을 장착했는지
		bool bMeleeAttack;	// 근접 공격 체크
		bool bRangeAttack;	// 원거리 공격 체크
		bool bSpecialAttack;// 특수 공격 체크
		bool bSlide;		// 슬라이딩
		bool bRun;

		bool bInvincible;	// 무적 ( 피격에 대한 면역 )

	}CONDITION;

	typedef struct tagWeapon
	{
		WORD wWeaponDamage;		// 무기 데미지 입니다.

		WORD wMaxBullet;		// 최대 총알 개수
		WORD wCurBullet;		// 현재 총알 개수
		WORD wUseBullet;		// 한번 사용 당 총알 개수
		WORD wMagazineSize;		// 한 탄창의 크기
		WORD wMagazineBullet;	// 한 탄창 속 총알의 개수

		float fRebound_Value;	// 반동 수치
		float fInterval;		// 발사 간격
		float fKnockBack_Value;	// 넉백 수치

		float fVertical_Rebound;	// 수평 반동
		float fHorizontal_Rebound;	// 수직 반동

		float fMaxZoom_Value;	// 최대 줌 수치
		float fZoom_Value;		// 줌 수치

		float fDelayTimer;		// 발사, 재장전 시 타이머

		float fBullet_Speed;	// 투사체일 시, 총알 속도

		float fBullet_Lifetime;	// 지속시간

		int fSpread_X;			// 수평 탄 퍼짐
		int fSpread_Y;			// 수직 탄 퍼짐

		bool fCanZoom;			// 줌 가능 한지
		bool fFovValue;			// 줌 수치

		BULLET_TYPE eBulletType;
		WEAPON_TAG	eWeaponTag;

	}W_INFO;
}

#define __ENGINE_STRUCT_H__
#endif