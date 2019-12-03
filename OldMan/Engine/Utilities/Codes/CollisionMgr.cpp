#include <iostream>
#include "CollisionMgr.h"
#include "GameObject.h"
#include "Component.h"
#include "Collider.h"
#include "Trasform.h"
#include "RigidBody.h"
#include "Condition.h"
USING(ENGINE)

CCollisionMgr::CCollisionMgr()
{
}


CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::CollisionRect(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	RECT rc = {};
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{

		}
	}
}

void CCollisionMgr::CollisionRectEX(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	float fMoveX = 0.f, fMoveY = 0.f;
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
		}
	}
}

void CCollisionMgr::CollisionSphere(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			if(Check_AABB_to_PLANE(rDst, rSrc , rDstCol , rSrcCol))
			{
				rSrcCol->Set_IsCollision(true);
 				//rDstTrans->SetPos(rDstTrans->GetPos() + rDstCol->Get_Length());
				//rSrcTrans->SetPos(rSrcTrans->GetPos() + rSrcCol->Get_Length());
			}
		}
	}
}

void CCollisionMgr::CollisionPlayer_To_Trigger(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			if (rDst == rSrc)
				continue;

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 20)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Ledge_Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				rSrcCol->Set_IsCollision(true);
			}

			else
			{
				if(rSrc->Get_Tag() != SPAWN_TRIGGER)
					rSrcCol->Set_IsCollision(false);
			}
		}
	}
}

void CCollisionMgr::CollisionPlayer_Fist_To_Target(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Melee_Collider"));

		if (rDstCol->Get_Enabled() == false)
			return;

		for (auto& rSrc : rSrcList)
		{
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));


			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 5)
				continue;

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				if (rSrc->Get_Tag() == MONSTER || rSrc->Get_Tag() == ENGINE::WOOD_BOX)
				{
					ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
					ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));

					if (rSrcCon != nullptr)
					{
						rSrcCon->Add_Hp(-rDstCon->Get_Damage());
						rDstCol->Set_IsCollision(true);
					}
				}

				rDstCol->Set_Enabled(false);
			}
		}

		if (rDstCol->Get_Enabled() == true)
		{
			rDstCol->Set_IsCollision(false);
			rDstCol->Set_Enabled(false);
		}
	}
}

void CCollisionMgr::CollisionPlayer_To_Other(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			if (rDst == rSrc)
				continue;

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 20)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{

			//	cout << rSrc->Get_Tag() << endl;


				rDstTrans->SetPos(rDstTrans->GetPos() + rDstCol->Get_Length());
				rSrcTrans->SetPos(rSrcTrans->GetPos() + rSrcCol->Get_Length());


				rDstCol->LateUpdate(rDstTrans->GetPos());

				if (rDst->Get_Tag() != GRENADE)
				{
					ENGINE::CCollider* rDstGCol = static_cast<CCollider*>(rDst->Get_Component(L"GCheck_Collider"));

					rDstGCol->LateUpdate({ rDstTrans->GetPos().x ,
						rDstTrans->GetPos().y - rDstCol->Get_Radius().y,
						rDstTrans->GetPos().z });

					if (rSrc->Get_Tag() == MONSTER || rSrc->Get_Tag() == PLAYER)
					{
						ENGINE::CCollider* rSrcGCol = static_cast<CCollider*>(rSrc->Get_Component(L"GCheck_Collider"));

						rSrcCol->LateUpdate(rSrcTrans->GetPos());
						rSrcGCol->LateUpdate({ rSrcTrans->GetPos().x ,
							rSrcTrans->GetPos().y - rSrcCol->Get_Radius().y,
							rSrcTrans->GetPos().z });
					}
				}
			}
		}
	}
}

void CCollisionMgr::CollisionPlayer_To_Item(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 20)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				rSrc->SetDead();
			}
		}
	}
}

void CCollisionMgr::CollisionTarget_To_Ground(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	int iCount = 0; bool bSkip = false;
	for (auto& rDst : rDstList)
	{
		for (list<CGameObject*>::iterator rSrc = rSrcList.begin(); rSrc != rSrcList.end(); ++rSrc)
		{
			bSkip = false;
			++iCount;

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>((*rSrc)->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));
			
			if (a > 20)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"GCheck_Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>((*rSrc)->Get_Component(L"Collider"));

			ENGINE::CRigidBody*	rDstRigid = static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"));

			if (Check_AABB(rDst, (*rSrc), rDstCol, rSrcCol))
			{

				//cout << (*rSrc)->Get_Tag() << endl;


				if (rDstRigid->Get_IsJump())
					return;

				rDstRigid->Set_Accel({ 0,0,0 });
				rDstRigid->Set_IsJump(false);
				rDstRigid->Set_IsFall(false);
				rDstRigid->Set_IsAir(false);
				rDstRigid->Set_IsGround(true);

				if (rDst->Get_Tag() == ENGINE::PLAYER)
					return;

				else if (rDst->Get_Tag() == ENGINE::MONSTER)
				{
					bSkip = true;
					break;
				}
			}
		}

		if (rDst->Get_Tag() == PLAYER)
		{
			static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsGround(false);
			static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsFall(true);
		}

		if (rDst->Get_Tag() == MONSTER)
		{
			if (bSkip == false)
			{
				static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsGround(false);
				static_cast<CRigidBody*>(rDst->Get_Component(L"RigidBody"))->Set_IsFall(true);
			}
		}
	}
}

void CCollisionMgr::CollisionBullet_To_Other(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			if (rDst->GetDead())
				continue;

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 20)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB_Bullet(rDst, rSrc, rDstCol, rSrcCol))
			{
				if (rSrc->Get_Tag() != ENGINE::TERRAIN && rSrc->Get_Tag() != ENGINE::STAIR && rSrc->Get_Tag() != ENGINE::METAL_BOX && rSrc->Get_Tag() != ENGINE::WOOD_BOX)
				{
					ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
					ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));

					if (rSrcCon != nullptr)
					{
						if (rSrcCon->Get_Armor() > 0)
							rSrcCon->Add_Armor(-rDstCon->Get_Damage());

						if (rSrcCon->Get_Armor() <= 0)
						{
							float fDamage = rDstCon->Get_Damage() + rSrcCon->Get_Armor();
							rSrcCon->Add_Hp(-fDamage);
						}
					}
				}

				else if (rSrc->Get_Tag() == ENGINE::WOOD_BOX)
				{
					ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
					ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));

					if (rDstCon != nullptr && rSrcCon != nullptr)
					{
						rSrcCon->Add_Hp(-rDstCon->Get_Damage());
						rSrcCol->Set_IsCollision(true);
					}
				}

				if (rDst->Get_Tag() == ENGINE::BULLET_MONSTER &&
					rSrc->Get_Tag() == ENGINE::PLAYER)
				{
					ENGINE::CRigidBody* rSrcRigid = static_cast<CRigidBody*>(rSrc->Get_Component(L"RigidBody"));

					if (rSrc->Get_Tag() == ENGINE::MONSTER)
					{
						ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
						ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));

						if (rSrcCon != nullptr)
						{
							rSrcCon->Add_Hp(-rDstCon->Get_Damage());
						}
					}

					if (rSrcRigid != nullptr)
					{
						D3DXVECTOR3 vDirection = { 0, 0, 0 };
						D3DXVec3Normalize(&vDirection, &rDstTrans->GetDir());

						rSrcRigid->Set_IsPushForUI(true);
						rSrcRigid->Set_PushDirForUI(vDirection);
					}
				}

				rDstCol->Set_IsCollision(true);
				rDst->Set_Tag(rSrc->Get_Tag());
				rDst->SetDead();
				continue;
			}
		}

	}
}

void CCollisionMgr::CollisionBomb_To_Other(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			if (rDst->GetDead())
				continue;

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 20)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"BombCollider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				if (rSrc->Get_Tag() == PLAYER || rSrc->Get_Tag() == MONSTER)
				{
					ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
					ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));

					if (rSrcCon != nullptr)
					{
						rSrcCon->Add_Hp(-rDstCon->Get_Damage());
					}

					D3DXVECTOR3 vDstPos = rDstTrans->GetPos();
					D3DXVECTOR3 vSrcPos = rSrcTrans->GetPos();
					D3DXVECTOR3 vTmpDir = vSrcPos - vDstPos;
					D3DXVec3Normalize(&vTmpDir, &vTmpDir);

					ENGINE::CRigidBody*	rSrcRigid = static_cast<CRigidBody*>(rSrc->Get_Component(L"RigidBody"));

					//if (rSrcRigid != nullptr)
					//{
						rSrcRigid->Set_Distance(D3DXVec3Length(&(vSrcPos - vDstPos)));
						rSrcRigid->Set_IsPush(true);
						rSrcRigid->Set_PushDir(vTmpDir);
					//}

					if (rSrc->Get_Tag() == ENGINE::PLAYER)
					{
						if (rSrcRigid != nullptr)
						{
							rSrcRigid->Set_IsPushForUI(true);
							rSrcRigid->Set_PushDirForUI(vTmpDir);
						}
					}
				}

				else if (rSrc->Get_Tag() == ENGINE::WOOD_BOX)
				{
					ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
					ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));

					if (rDstCon != nullptr && rSrcCon != nullptr)
					{
						rSrcCon->Add_Hp(-rDstCon->Get_Damage());
						rSrcCol->Set_IsCollision(true);
					}
				}
			}
		}

	}
}

void CCollisionMgr::CollisionVFX_To_Other(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			if (rDst->GetDead())
				continue;

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));

			if (a > 20)
				continue;

			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			if (Check_AABB_Bullet(rDst, rSrc, rDstCol, rSrcCol))
			{
			}
		}

	}
}

void CCollisionMgr::CollisionTarget_To_Monstr_Mele(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			if (static_cast<CCollider*>(rDst->Get_Component(L"Monster_Mell"))->Get_Enabled()==false)
				continue;
		
			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Monster_Mell"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));
		
			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));
					

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
			//	cout << "충돌체크 됩니다" << endl;

				rDstCol->Set_IsCollision(true);
				ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
				ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));
			
				if (rSrcCon->Get_Armor() > 0)
					rSrcCon->Add_Armor(-rDstCon->Get_Damage());

				if (rSrcCon->Get_Armor() <= 0)
				{
					float fDamage = rDstCon->Get_Damage() + rSrcCon->Get_Armor();
					rSrcCon->Add_Hp(-fDamage);
				}


				return;
			}
			else
			{
			//	cout << "충돌 안함" << endl;
				rDstCol->Set_IsCollision(false);

			}
		}


	}
	

}

void CCollisionMgr::CollisionTarget_To_OcatabrainBullet(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			
			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			float a = D3DXVec3Length(&(rDstTrans->GetPos() - rSrcTrans->GetPos()));


			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				//	cout << "충돌체크 됩니다" << endl;

				rDstCol->Set_IsCollision(true);
				ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));
				ENGINE::CCondition* rSrcCon = static_cast<CCondition*>(rSrc->Get_Component(L"Condition"));

				if (rSrcCon->Get_Armor() > 0)
					rSrcCon->Add_Armor(-rDstCon->Get_Damage());

				if (rSrcCon->Get_Armor() <= 0)
				{
					float fDamage = rDstCon->Get_Damage() + rSrcCon->Get_Armor();
					rSrcCon->Add_Hp(-fDamage);
				}
				rSrcTrans->MovePos(-1.f);
				
				return;
			}
			else
			{
				//	cout << "충돌 안함" << endl;
				rDstCol->Set_IsCollision(false);

			}
		}


	}







}




void CCollisionMgr::CollisionTarget_To_Monstr(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
			ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

			ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
			ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

			if (Check_AABB(rDst, rSrc, rDstCol, rSrcCol))
			{
				ENGINE::CCondition* rDstCon = static_cast<CCondition*>(rDst->Get_Component(L"Condition"));


				static_cast<CCondition*>(rDst->Get_Component(L"Condition"))->Set_Hit(true);
				rSrcCol->Set_IsCollision(true);
				return;
			}
			else
			{
			//cout << "충돌 안함" << endl;
				static_cast<CCondition*>(rDst->Get_Component(L"Condition"))->Set_Hit(false);

			}
		}


	}

}


bool CCollisionMgr::Check_AABB(ENGINE::CGameObject* rDst , ENGINE::CGameObject* rSrc, CCollider* _rDstCol , CCollider* _rSrcCol)
{
	if (_rSrcCol->Get_CollisionType() != ENGINE::COLLISION_AABB)
		return false;

	ENGINE::BOXCOL* rDstBox = _rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = _rSrcCol->Get_BoxCollider();

	if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
		rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
		rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
	{
		// 콜리전 하는 물체끼리 체크 
		if (!rDstBox->bIsTrigger && !rSrtBox->bIsTrigger)
		{
			_rDstCol->Set_IsCollision(true);
			_rSrcCol->Set_IsCollision(true);

			// A 가 dynamic 이고 , B 도 dynamic 일 때
			if (rDstBox->bIsDynamic && rSrtBox->bIsDynamic)
			{
				D3DXVECTOR3 tmpLength_Dst = {};
				D3DXVECTOR3 tmpLength_Srt = {};


				if(rSrc->Get_Tag() == ENGINE::STAIR)
				{ 
					tmpLength_Dst = Get_Stair_Length(rDstBox, rSrtBox, true);
					tmpLength_Srt = Get_Stair_Length(rDstBox, rSrtBox, true);
				}
				else
				{
					tmpLength_Dst = Get_Length(rDstBox, rSrtBox, true);
					tmpLength_Srt = Get_Length(rDstBox, rSrtBox, true);
				}

				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength_Dst.x *= -1.f : tmpLength_Dst.x *= 1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength_Dst.y *= -1.f : tmpLength_Dst.y *= 1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength_Dst.z *= -1.f : tmpLength_Dst.z *= 1.f);

				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength_Srt.x *= 1.f : tmpLength_Srt.x *= -1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength_Srt.y *= 1.f : tmpLength_Srt.y *= -1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength_Srt.z *= 1.f : tmpLength_Srt.z *= -1.f);

				_rDstCol->Set_Length(tmpLength_Dst);
				_rSrcCol->Set_Length(tmpLength_Srt);
			}

			// A 가 dynamic 이고 , B 는 Static 일 때
			else if (rDstBox->bIsDynamic && !rSrtBox->bIsDynamic)
			{
				D3DXVECTOR3 tmpLength_Dst = {};

				if (rSrc->Get_Tag() == ENGINE::STAIR)
				{
					tmpLength_Dst = { 0, rSrtBox->vRadius.y * 2, 0 };

					//tmpLength_Dst = Get_Stair_Length(rDstBox, rSrtBox, true);
					//tmpLength_Dst.y = fabs(tmpLength_Dst.y);
				}
				else
				{
					tmpLength_Dst = Get_Length(rDstBox, rSrtBox, true);

					(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength_Dst.x *= -1.f : tmpLength_Dst.x *= 1.f);
					(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength_Dst.y *= -1.f : tmpLength_Dst.y *= 1.f);
					(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength_Dst.z *= -1.f : tmpLength_Dst.z *= 1.f);
				}

				_rDstCol->Set_Length(tmpLength_Dst);
			}

			// A 가 Static 이고 , B 는 dynamic 일 때
			else if (!rDstBox->bIsDynamic && rSrtBox->bIsDynamic)
			{
				_rSrcCol->Set_Length(Get_Length(rDstBox, rSrtBox));
			}
		}

		// Dst 는 트리거, Src 는 트리거 아님
		else if (rDstBox->bIsTrigger && !rSrtBox->bIsTrigger)
		{
			return true;
		}

		return true;
	}

	else
	{


		return false;
	}

	return false;
}

bool CCollisionMgr::Check_AABB_Bullet(ENGINE::CGameObject * rDst, ENGINE::CGameObject * rSrc, CCollider * _rDstCol, CCollider * _rSrcCol)
{
	if (_rSrcCol->Get_CollisionType() != ENGINE::COLLISION_AABB)
		return false;

	ENGINE::BOXCOL* rDstBox = _rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = _rSrcCol->Get_BoxCollider();

	ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
	ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

	if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
		rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
		rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
	{
		D3DXVECTOR3 tmpLength = Get_Length_Bullet(rDst, rSrc);
		
		if (tmpLength.x != 0)
		{
			// x가 음의 방향이라면
			if (rDstTrans->GetDir().x < 0)
			{
				// rSrc 의 Max X 와 비교
				if (rSrtBox->vMaxPos.x < rDstBox->vCenterPos.x)
				{
					float fixed_x = rSrtBox->vMaxPos.x - rDstBox->vCenterPos.x;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x * 0.95f , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (rSrtBox->vMaxPos.x >= rDstBox->vCenterPos.x)
				{
					float fixed_x = rSrtBox->vMaxPos.x - rDstBox->vCenterPos.x;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x* 1.05f , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}
			}

			// x가 양의 방향이라면
			else if (rDstTrans->GetDir().x >= 0)
			{
				// rSrc 의 Max X 와 비교
				if (rSrtBox->vMinPos.x > rDstBox->vCenterPos.x)
				{
					float fixed_x = rSrtBox->vMinPos.x - rDstBox->vCenterPos.x;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x* 0.95f , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (rSrtBox->vMinPos.x <= rDstBox->vCenterPos.x)
				{
					float fixed_x = rSrtBox->vMinPos.x - rDstBox->vCenterPos.x;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x* 1.05f , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}
			}
		}

		else if (tmpLength.y != 0)
		{
			// x가 음의 방향이라면
			if (rDstTrans->GetDir().y < 0)
			{
				// rSrc 의 Max X 와 비교
				if (rSrtBox->vMaxPos.y < rDstBox->vCenterPos.y)
				{
					float fixed_x = rSrtBox->vMaxPos.y - rDstBox->vCenterPos.y;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fixed_x * 0.95f , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (rSrtBox->vMaxPos.y >= rDstBox->vCenterPos.y)
				{
					float fixed_x = rSrtBox->vMaxPos.y - rDstBox->vCenterPos.y;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fixed_x * 1.05f  , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}
			}

			// x가 양의 방향이라면
			else if (rDstTrans->GetDir().y >= 0)
			{
				// rSrc 의 Max X 와 비교
				if (rSrtBox->vMinPos.y > rDstBox->vCenterPos.y)
				{
					float fixed_x = rSrtBox->vMinPos.y - rDstBox->vCenterPos.y;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fixed_x * 0.95f , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (rSrtBox->vMinPos.y <= rDstBox->vCenterPos.y)
				{
					float fixed_x = rSrtBox->vMinPos.y - rDstBox->vCenterPos.y;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x, rDstBox->vCenterPos.y + fixed_x* 1.05f  , rDstBox->vCenterPos.z };

					_rDstCol->Set_CenterPos(vTmpPos);
				}
			}
		}

		else if (tmpLength.z != 0)
		{
			// x가 음의 방향이라면
			if (rDstTrans->GetDir().z < 0)
			{
				// rSrc 의 Max X 와 비교
				if (rSrtBox->vMaxPos.z < rDstBox->vCenterPos.z)
				{
					float fixed_x = rSrtBox->vMaxPos.z - rDstBox->vCenterPos.z;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y, rDstBox->vCenterPos.z + fixed_x * 0.95f };

					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (rSrtBox->vMaxPos.z >= rDstBox->vCenterPos.z)
				{
					float fixed_x = rSrtBox->vMaxPos.z - rDstBox->vCenterPos.z;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z + fixed_x  * 1.05f };

					_rDstCol->Set_CenterPos(vTmpPos);
				}
			}

			// x가 양의 방향이라면
			else if (rDstTrans->GetDir().z >= 0)
			{
				// rSrc 의 Max X 와 비교
				if (rSrtBox->vMinPos.z > rDstBox->vCenterPos.z)
				{
					float fixed_x = rSrtBox->vMinPos.z - rDstBox->vCenterPos.z;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y  , rDstBox->vCenterPos.z + fixed_x * 0.95f };

					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (rSrtBox->vMinPos.z <= rDstBox->vCenterPos.z)
				{
					float fixed_x = rSrtBox->vMinPos.z - rDstBox->vCenterPos.z;
					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x, rDstBox->vCenterPos.y , rDstBox->vCenterPos.z + fixed_x * 1.05f };

					_rDstCol->Set_CenterPos(vTmpPos);
				}
			}
		}

		return true;
	}

	else 
	{
		// 현재 거리와 과거 거리의 차이를 구한다. 그리고, 차이의 일정 비율만큼 빼준다.

		D3DXVECTOR3 vTempGap = _rDstCol->Get_OldPos() - rDstBox->vCenterPos;
		D3DXVECTOR3 vOriginPos = rDstBox->vCenterPos;
		D3DXVECTOR3 vTemPos[19] = {};
		vTemPos[18] = rDstBox->vCenterPos + vTempGap * 0.95f;
		vTemPos[17] = rDstBox->vCenterPos + vTempGap * 0.9f;
		vTemPos[16] = rDstBox->vCenterPos + vTempGap * 0.85f;
		vTemPos[15] = rDstBox->vCenterPos + vTempGap * 0.8f;
		vTemPos[14] = rDstBox->vCenterPos + vTempGap * 0.75f;
		vTemPos[13] = rDstBox->vCenterPos + vTempGap * 0.7f;
		vTemPos[12] = rDstBox->vCenterPos + vTempGap * 0.65f;
		vTemPos[11] = rDstBox->vCenterPos + vTempGap * 0.6f;
		vTemPos[10] = rDstBox->vCenterPos + vTempGap * 0.55f;
		vTemPos[9] = rDstBox->vCenterPos + vTempGap * 0.5f;
		vTemPos[8] = rDstBox->vCenterPos+ vTempGap  * 0.45f;
		vTemPos[7] = rDstBox->vCenterPos+ vTempGap  * 0.4f;
		vTemPos[6] = rDstBox->vCenterPos + vTempGap  * 0.35f;
		vTemPos[5] = rDstBox->vCenterPos+ vTempGap  * 0.3f;
		vTemPos[4] = rDstBox->vCenterPos + vTempGap  * 0.25f;
		vTemPos[3] = rDstBox->vCenterPos+ vTempGap  * 0.2f;
		vTemPos[2] = rDstBox->vCenterPos + vTempGap  * 0.15f;
		vTemPos[1] = rDstBox->vCenterPos+ vTempGap  * 0.1f;
		vTemPos[0] = rDstBox->vCenterPos+ vTempGap  * 0.05f; 

		for (int i = 0; i < 19; ++i)
		{
			_rDstCol->Set_CenterPos(vTemPos[i]);
			_rDstCol->SetUp_Box();
			
			if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
				rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
				rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
			{
				D3DXVECTOR3 tmpLength = Get_Length_Bullet(rDst, rSrc);

				if (tmpLength.x != 0)
				{
					// x가 음의 방향이라면
					if (rDstTrans->GetDir().x < 0)
					{
						// rSrc 의 Max X 와 비교
						if (rSrtBox->vMaxPos.x < rDstBox->vCenterPos.x)
						{
							float fixed_x = rSrtBox->vMaxPos.x - rDstBox->vCenterPos.x;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x * 1.05f , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}

						if (rSrtBox->vMaxPos.x > rDstBox->vCenterPos.x)
						{
							float fixed_x = rSrtBox->vMaxPos.x - rDstBox->vCenterPos.x;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x* 1.05f  , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}
					}

					// x가 양의 방향이라면
					else if (rDstTrans->GetDir().x >= 0)
					{
						// rSrc 의 Max X 와 비교
						if (rSrtBox->vMinPos.x > rDstBox->vCenterPos.x)
						{
							float fixed_x = rSrtBox->vMinPos.x - rDstBox->vCenterPos.x;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x* 1.05f  , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}

						else if (rSrtBox->vMinPos.x <= rDstBox->vCenterPos.x)
						{
							float fixed_x = rSrtBox->vMinPos.x - rDstBox->vCenterPos.x;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fixed_x* 1.05f  , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}
					}
				}

				else if (tmpLength.y != 0)
				{
					// x가 음의 방향이라면
					if (rDstTrans->GetDir().y < 0)
					{
						// rSrc 의 Max X 와 비교
						if (rSrtBox->vMaxPos.y < rDstBox->vCenterPos.y)
						{
							float fixed_x = rSrtBox->vMaxPos.y - rDstBox->vCenterPos.y;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fixed_x * 1.05f , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}

						else if (rSrtBox->vMaxPos.y > rDstBox->vCenterPos.y)
						{
							float fixed_x = rSrtBox->vMaxPos.y - rDstBox->vCenterPos.y;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fixed_x * 1.05f  , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}
					}

					// x가 양의 방향이라면
					else if (rDstTrans->GetDir().y >= 0)
					{
						// rSrc 의 Max X 와 비교
						if (rSrtBox->vMinPos.y > rDstBox->vCenterPos.y)
						{
							float fixed_x = rSrtBox->vMinPos.y - rDstBox->vCenterPos.y;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fixed_x * 1.05f , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}

						else if (rSrtBox->vMinPos.y <= rDstBox->vCenterPos.y)
						{
							float fixed_x = rSrtBox->vMinPos.y - rDstBox->vCenterPos.y;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x, rDstBox->vCenterPos.y + fixed_x* 1.05f  , rDstBox->vCenterPos.z };

							_rDstCol->Set_CenterPos(vTmpPos);
						}
					}
				}

				else if (tmpLength.z != 0)
				{
					// x가 음의 방향이라면
					if (rDstTrans->GetDir().z < 0)
					{
						// rSrc 의 Max X 와 비교
						if (rSrtBox->vMaxPos.z < rDstBox->vCenterPos.z)
						{
							float fixed_x = rSrtBox->vMaxPos.z - rDstBox->vCenterPos.z;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y, rDstBox->vCenterPos.z + fixed_x * 1.05f };

							_rDstCol->Set_CenterPos(vTmpPos);
						}

						else if (rSrtBox->vMaxPos.z > rDstBox->vCenterPos.z)
						{
							float fixed_x = rSrtBox->vMaxPos.z - rDstBox->vCenterPos.z;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z + fixed_x  * 1.05f };

							_rDstCol->Set_CenterPos(vTmpPos);
						}
					}

					// x가 양의 방향이라면
					else if (rDstTrans->GetDir().z >= 0)
					{
						// rSrc 의 Max X 와 비교
						if (rSrtBox->vMinPos.z > rDstBox->vCenterPos.z)
						{
							float fixed_x = rSrtBox->vMinPos.z - rDstBox->vCenterPos.z;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y  , rDstBox->vCenterPos.z + fixed_x * 1.05f };

							_rDstCol->Set_CenterPos(vTmpPos);
						}

						else if (rSrtBox->vMinPos.z <= rDstBox->vCenterPos.z)
						{
							float fixed_x = rSrtBox->vMinPos.z - rDstBox->vCenterPos.z;
							D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x, rDstBox->vCenterPos.y , rDstBox->vCenterPos.z + fixed_x * 1.05f };

							_rDstCol->Set_CenterPos(vTmpPos);
						}
					}
				}
			
				return true;
			}
		}

		_rDstCol->Set_CenterPos(vOriginPos);
		_rDstCol->SetUp_Box();

		return false;
	}

	return false;
}

bool CCollisionMgr::Check_AABB_to_PLANE(ENGINE::CGameObject* rDst, ENGINE::CGameObject* rSrc, CCollider* _rDstCol, CCollider* _rSrcCol)
{
	ENGINE::BOXCOL* rDstBox = _rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = _rSrcCol->Get_BoxCollider();

	ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
	ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));


	D3DXVECTOR3* tmpVtx =  _rSrcCol->Get_PlaneVtx();
	D3DXVECTOR3 tmpMax , tmpMin;
	
	
	D3DXPLANE tmpPlane;
	D3DXPlaneFromPoints(&tmpPlane, &tmpVtx[0] , &tmpVtx[1] , &tmpVtx[2]);

	if (rSrcTrans->GetDir().x >= 0)
	{
		tmpMax.x = rDstBox->vMaxPos.x;
		tmpMin.x = rDstBox->vMinPos.x;
	}
	
	else
	{
		tmpMax.x = rDstBox->vMinPos.x;
		tmpMin.x = rDstBox->vMaxPos.x;
	}


	if (rSrcTrans->GetDir().y >= 0)
	{
		tmpMax.y = rDstBox->vMaxPos.y;
		tmpMin.y = rDstBox->vMinPos.y;
	}

	else
	{
		tmpMax.y = rDstBox->vMinPos.y;
		tmpMin.y = rDstBox->vMaxPos.y;
	}


	if (rSrcTrans->GetDir().z >= 0)
	{
		tmpMax.z = rDstBox->vMaxPos.z;
		tmpMin.z = rDstBox->vMinPos.z;
	}

	else
	{
		tmpMax.z = rDstBox->vMinPos.z;
		tmpMin.z = rDstBox->vMaxPos.z;
	}
	
	if (D3DXPlaneDotCoord(&tmpPlane, &tmpMin) < 0)
	{
		//cout << " 뒤에 있어" << endl;
		return false;
	}
	
	else if (D3DXPlaneDotCoord(&tmpPlane, &tmpMax) > 0)
	{
		//cout << " 앞에 있어" << endl;
		return false;
	}

	else
	{
		//cout << "겹 친다" << endl;
		return true;
	}

	return false;
}

bool CCollisionMgr::Check_Collision(ENGINE::CGameObject* rDst, ENGINE::CGameObject* rSrc, CCollider* _rDstCol, CCollider* _rSrcCol)
{
	if(_rDstCol->Get_CollisionType() == ENGINE::COLLISION_LINE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_PLANE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_AABB)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			if (Check_AABB(rDst, rSrc, _rDstCol, _rSrcCol))
			{
				return true;
			}

			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_SHPERE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	if (_rDstCol->Get_CollisionType() == ENGINE::COLLISION_CAPSHULE)
	{
		switch (_rSrcCol->Get_CollisionType())
		{
		case ENGINE::COLLISION_LINE:
		{
			break;
		}

		case ENGINE::COLLISION_PLANE:
		{
			break;
		}

		case ENGINE::COLLISION_AABB:
		{
			break;
		}

		case ENGINE::COLLISION_SHPERE:
		{
			break;
		}

		case ENGINE::COLLISION_CAPSHULE:
		{
			break;
		}
		}
	}

	return false;
}

D3DXVECTOR3 CCollisionMgr::Get_Length_Bullet(ENGINE::CGameObject * rDst, ENGINE::CGameObject * rSrc,  bool _static)
{
	ENGINE::CTransform* rDstTrans = static_cast<CTransform*>(rDst->Get_Component(L"Transform"));
	ENGINE::CTransform* rSrcTrans = static_cast<CTransform*>(rSrc->Get_Component(L"Transform"));

	ENGINE::CCollider* rDstCol = static_cast<CCollider*>(rDst->Get_Component(L"Collider"));
	ENGINE::CCollider* rSrcCol = static_cast<CCollider*>(rSrc->Get_Component(L"Collider"));

	ENGINE::BOXCOL* rDstBox = rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = rSrcCol->Get_BoxCollider();

	D3DXVECTOR3 vMin = {};
	D3DXVECTOR3 vMax = {};
	D3DXVECTOR3 vCross = {};

	vMax.x = min(rDstBox->vMaxPos.x, rSrtBox->vMaxPos.x);
	vMax.y = min(rDstBox->vMaxPos.y, rSrtBox->vMaxPos.y);
	vMax.z = min(rDstBox->vMaxPos.z, rSrtBox->vMaxPos.z);

	vMin.x = max(rDstBox->vMinPos.x, rSrtBox->vMinPos.x);
	vMin.y = max(rDstBox->vMinPos.y, rSrtBox->vMinPos.y);
	vMin.z = max(rDstBox->vMinPos.z, rSrtBox->vMinPos.z);

	vCross.x = vMax.x - vMin.x;
	vCross.y = vMax.y - vMin.y;
	vCross.z = vMax.z - vMin.z;

	if (fabs(vCross.x) == fabs(vCross.y) &&
		fabs(vCross.y) == fabs(vCross.z) &&
		fabs(vCross.z) == fabs(vCross.x) )
	{
		float fGaps[3] = {};
		//float fReverse[3] = {};
		D3DXVECTOR3 vTempDir = -rDstTrans->GetDir();
		D3DXVECTOR3 vTempPos = rDstCol->Get_CenterPos();

		// x 축 방향이 0보다 작았을 경우 ( 음수 일 경우)
		if (rDstTrans->GetDir().x < 0)
		{
			fGaps[0] = rSrtBox->vMaxPos.x - rDstBox->vCenterPos.x;
		}

		// x 축 방향이 0보다 컷을 경우 ( 양수 일 경우)
		else if (rDstTrans->GetDir().x >= 0)
		{
			fGaps[0] = rSrtBox->vMinPos.x - rDstBox->vCenterPos.x;
		}

		if (rDstTrans->GetDir().y < 0)
		{
			fGaps[1] = rSrtBox->vMaxPos.y - rDstBox->vCenterPos.y;
		}

		// y 축 방향이 0보다 컷을 경우 ( 양수 일 경우)
		else if (rDstTrans->GetDir().y >= 0)
		{
			fGaps[1] = rSrtBox->vMinPos.y - rDstBox->vCenterPos.y;
		}


		// z축
		if (rDstTrans->GetDir().z < 0)
		{
			fGaps[2] = rSrtBox->vMaxPos.z - rDstBox->vCenterPos.z;
		}

		// y 축 방향이 0보다 컷을 경우 ( 양수 일 경우)
		else if (rDstTrans->GetDir().z >= 0)
		{
			fGaps[2] = rSrtBox->vMinPos.z - rDstBox->vCenterPos.z;
		}

		if (fabs(fGaps[0]) < fabs(fGaps[1]))
		{
			if (fabs(fGaps[0]) < fabs(fGaps[2]))
			{
				fGaps[2] = 0;

				// x가 가장 작을 때,
			}

			else
			{
				fGaps[0] = 0;


			}

			fGaps[1] = 0;
		}

		else
		{
			if (fabs(fGaps[1]) < fabs(fGaps[2]))
			{
				fGaps[2] = 0;
			}

			else
			{
				fGaps[1] = 0;
			}

			fGaps[0] = 0;
		}

		vCross.x = 0; 
		vCross.y = 0;
		vCross.z = 0;

		rDstCol->Set_CenterPos({ vTempPos.x + fGaps[0] * 1.05f , vTempPos.y + fGaps[1] * 1.05f, vTempPos.z + fGaps[2] * 1.05f });

		return vCross;
	}

	if (fabs(vCross.x) < fabs(vCross.y))
	{
		if (fabs(vCross.x) < fabs(vCross.z))
		{
			vCross.z = 0;

			// x가 가장 작을 때,
		}

		else
		{
			vCross.x = 0;


		}

		vCross.y = 0;
	}

	else
	{
		if (fabs(vCross.y) < fabs(vCross.z))
		{
			vCross.z = 0;
		}

		else
		{
			vCross.y = 0;
		}

		vCross.x = 0;
	}

	if (_static)
		return vCross * 0.5f;

	else
		return vCross;
}

D3DXVECTOR3 CCollisionMgr::Get_Length(ENGINE::BOXCOL * _DistCollider, ENGINE::BOXCOL * _TargetCollider, bool _static)
{
	D3DXVECTOR3 vMin = {};
	D3DXVECTOR3 vMax = {};
	D3DXVECTOR3 vCross = {};

	vMax.x = min(_DistCollider->vMaxPos.x, _TargetCollider->vMaxPos.x);
	vMax.y = min(_DistCollider->vMaxPos.y, _TargetCollider->vMaxPos.y);
	vMax.z = min(_DistCollider->vMaxPos.z, _TargetCollider->vMaxPos.z);

	vMin.x = max(_DistCollider->vMinPos.x, _TargetCollider->vMinPos.x);
	vMin.y = max(_DistCollider->vMinPos.y, _TargetCollider->vMinPos.y);
	vMin.z = max(_DistCollider->vMinPos.z, _TargetCollider->vMinPos.z);

	vCross.x = vMax.x - vMin.x;
	vCross.y = vMax.y - vMin.y;
	vCross.z = vMax.z - vMin.z;

	if (vCross.x == vCross.y && vCross.y == vCross.z)
	{
		return { 0,0,0 };
	}

	if (fabs(vCross.x) < fabs(vCross.y))
	{
		if (fabs(vCross.x) < fabs(vCross.z))
		{
			vCross.z = 0;
		}

		else
		{
			vCross.x = 0;
		}

		vCross.y = 0;
	}

	else
	{
		if (fabs(vCross.y) < fabs(vCross.z))
		{
			vCross.z = 0;
		}

		else
		{
			vCross.y = 0;
		}

		vCross.x = 0;
	}


	if (_static)
		return vCross * 0.5f;

	else
		return vCross;
}

D3DXVECTOR3 CCollisionMgr::Get_Stair_Length(ENGINE::BOXCOL * _DistCollider, ENGINE::BOXCOL * _TargetCollider, bool _static)
{
	D3DXVECTOR3 vMin = {};
	D3DXVECTOR3 vMax = {};
	D3DXVECTOR3 vCross = {};

	vMax.x = min(_DistCollider->vMaxPos.x, _TargetCollider->vMaxPos.x);
	vMax.y = min(_DistCollider->vMaxPos.y, _TargetCollider->vMaxPos.y);
	vMax.z = min(_DistCollider->vMaxPos.z, _TargetCollider->vMaxPos.z);

	vMin.x = max(_DistCollider->vMinPos.x, _TargetCollider->vMinPos.x);
	vMin.y = max(_DistCollider->vMinPos.y, _TargetCollider->vMinPos.y);
	vMin.z = max(_DistCollider->vMinPos.z, _TargetCollider->vMinPos.z);

	vCross.x = 0;
	vCross.y = vMax.y - vMin.y;
	vCross.z = 0;


	if (_static)
		return vCross * 0.5f;

	else
		return vCross;
}