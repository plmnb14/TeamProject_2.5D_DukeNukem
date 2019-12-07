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
				if (rSrc->Get_Tag() != SPAWN_TRIGGER)
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
							rSrcCon->Set_Armor(0);
							rSrcCon->Add_Hp(-fDamage);

							if (rSrcCon->Get_Hp() < 0)
								rSrcCon->Set_Hp(0.f);
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
							if (rSrcCon->Get_Hp() > 0)
								rSrcCon->Add_Hp(-rDstCon->Get_Damage());
						}
					}

					if (rSrcRigid != nullptr)
					{
						D3DXVECTOR3 vDstPos = rDstTrans->GetPos();
						D3DXVECTOR3 vSrcPos = rSrcTrans->GetPos();
						D3DXVECTOR3 vTmpDir = vSrcPos - vDstPos;
						D3DXVec3Normalize(&vTmpDir, &vTmpDir);

						rSrcRigid->Set_IsPushForUI(true);
						rSrcRigid->Set_PushDirForUI(vTmpDir);
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
						if (rSrcCon->Get_Armor() > 0)
							rSrcCon->Add_Armor(-rDstCon->Get_Damage());

						if (rSrcCon->Get_Armor() <= 0)
						{
							float fDamage = rDstCon->Get_Damage() + rSrcCon->Get_Armor();
							rSrcCon->Set_Armor(0);
							rSrcCon->Add_Hp(-fDamage);

							if (rSrcCon->Get_Hp() < 0)
								rSrcCon->Set_Hp(0.f);
						}
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
			if (static_cast<CCollider*>(rDst->Get_Component(L"Monster_Mell"))->Get_Enabled() == false)
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
					rSrcCon->Set_Armor(0);
					rSrcCon->Add_Hp(-fDamage);

					if (rSrcCon->Get_Hp() < 0)
						rSrcCon->Set_Hp(0.f);
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
				static_cast<CCondition*>(rDst->Get_Component(L"Condition"))->Set_Hit(false);

			}
		}


	}

}


bool CCollisionMgr::Check_AABB(ENGINE::CGameObject* rDst, ENGINE::CGameObject* rSrc, CCollider* _rDstCol, CCollider* _rSrcCol)
{
	ENGINE::BOXCOL* rDstBox = _rDstCol->Get_BoxCollider();
	ENGINE::BOXCOL* rSrtBox = _rSrcCol->Get_BoxCollider();

	if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
		rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
		rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
	{
		if (!rDstBox->bIsTrigger && !rSrtBox->bIsTrigger)
		{
			D3DXVECTOR3 tmpLength = { 0,0,0 };
			bool bCheckDynamic = false;

			if (rDstBox->bIsDynamic)
			{
				if (rSrtBox->bIsDynamic)
					bCheckDynamic = true;

				tmpLength = Get_Length(rDstBox, rSrtBox, bCheckDynamic);

				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength.x *= -1.f : tmpLength.x *= 1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength.y *= -1.f : tmpLength.y *= 1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength.z *= -1.f : tmpLength.z *= 1.f);

				_rDstCol->Set_Length(tmpLength);
			}

			if (rSrtBox->bIsDynamic)
			{
				if (rDstBox->bIsDynamic)
					bCheckDynamic = true;

				tmpLength = Get_Length(rDstBox, rSrtBox, bCheckDynamic);

				(rDstBox->vCenterPos.x < rSrtBox->vCenterPos.x ? tmpLength.x *= 1.f : tmpLength.x *= -1.f);
				(rDstBox->vCenterPos.y < rSrtBox->vCenterPos.y ? tmpLength.y *= 1.f : tmpLength.y *= -1.f);
				(rDstBox->vCenterPos.z < rSrtBox->vCenterPos.z ? tmpLength.z *= 1.f : tmpLength.z *= -1.f);

				_rSrcCol->Set_Length(tmpLength);
			}
		}

		_rDstCol->Set_IsCollision(true);
		_rSrcCol->Set_IsCollision(true);

		return true;
	}

	return false;
}

bool CCollisionMgr::Check_AABB_Bullet(ENGINE::CGameObject * rDst, ENGINE::CGameObject * rSrc, CCollider * _rDstCol, CCollider * _rSrcCol)
{
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
			float fLegnth_x = 0.f;
			float fixed_x = 0.f;
			float fDistance = 0.5f;

			(rSrtBox->vMaxPos.x < rDstBox->vCenterPos.x ? fixed_x = 1.f - fDistance : fixed_x = 1.f + fDistance);
			(rDstTrans->GetDir().x < 0 ? fLegnth_x = rSrtBox->vMaxPos.x - rDstBox->vCenterPos.x : 
										fLegnth_x = rSrtBox->vMinPos.x - rDstBox->vCenterPos.x);

			D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fLegnth_x * fixed_x , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };
			_rDstCol->Set_CenterPos(vTmpPos);
		}

		else if (tmpLength.y != 0)
		{
			float fLegnth_y = 0.f;
			float fixed_y = 0.f;
			float fDistance = 0.5f;

			(rSrtBox->vMaxPos.y < rDstBox->vCenterPos.y ? fixed_y = 1.f - fDistance : fixed_y = 1.f + fDistance);
			(rDstTrans->GetDir().y < 0 ? fLegnth_y = rSrtBox->vMaxPos.y - rDstBox->vCenterPos.y :
				fLegnth_y = rSrtBox->vMinPos.y - rDstBox->vCenterPos.y);

			D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fLegnth_y * fixed_y, rDstBox->vCenterPos.z };
			_rDstCol->Set_CenterPos(vTmpPos);
		}

		else if (tmpLength.z != 0)
		{
			float fLegnth_z = 0.f;
			float fixed_z = 0.f;
			float fDistance = 0.5f;

			(rSrtBox->vMaxPos.z < rDstBox->vCenterPos.z ? fixed_z = 1.f - fDistance : fixed_z = 1.f + fDistance);
			(rDstTrans->GetDir().z < 0 ? fLegnth_z = rSrtBox->vMaxPos.z - rDstBox->vCenterPos.y :
				fLegnth_z = rSrtBox->vMinPos.z - rDstBox->vCenterPos.z);

			D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y, rDstBox->vCenterPos.z + fLegnth_z * fixed_z };
			_rDstCol->Set_CenterPos(vTmpPos);
		}

		return true;
	}

	else
	{
		const int	iCount = 20;
		const float	fDecrease = 0.05f;

		D3DXVECTOR3 vTemPos[iCount] = {};
		D3DXVECTOR3 vTempGap = _rDstCol->Get_OldPos() - rDstBox->vCenterPos;
		D3DXVECTOR3 vOriginPos = rDstBox->vCenterPos;


		for (int i = 0; i < iCount; ++i)
		{
			vTemPos[i] = rDstBox->vCenterPos + vTempGap * ((i + 1) * fDecrease);

			_rDstCol->Set_CenterPos(vTemPos[i]);
			_rDstCol->SetUp_Box();

			if (rDstBox->vMinPos.x <= rSrtBox->vMaxPos.x && rDstBox->vMaxPos.x >= rSrtBox->vMinPos.x &&
				rDstBox->vMinPos.y <= rSrtBox->vMaxPos.y && rDstBox->vMaxPos.y >= rSrtBox->vMinPos.y &&
				rDstBox->vMinPos.z <= rSrtBox->vMaxPos.z && rDstBox->vMaxPos.z >= rSrtBox->vMinPos.z)
			{
				D3DXVECTOR3 tmpLength = Get_Length_Bullet(rDst, rSrc);

				if (tmpLength.x != 0)
				{
					float fLegnth_x = 0.f;
					float fixed_x = 0.f;
					float fDistance = 0.5f;

					(rSrtBox->vMaxPos.x < rDstBox->vCenterPos.x ? fixed_x = 1.f - fDistance : fixed_x = 1.f + fDistance);
					(rDstTrans->GetDir().x < 0 ? fLegnth_x = rSrtBox->vMaxPos.x - rDstBox->vCenterPos.x :
						fLegnth_x = rSrtBox->vMinPos.x - rDstBox->vCenterPos.x);

					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x + fLegnth_x * fixed_x , rDstBox->vCenterPos.y , rDstBox->vCenterPos.z };
					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (tmpLength.y != 0)
				{
					float fLegnth_y = 0.f;
					float fixed_y = 0.f;
					float fDistance = 0.5f;

					(rSrtBox->vMaxPos.y < rDstBox->vCenterPos.y ? fixed_y = 1.f - fDistance : fixed_y = 1.f + fDistance);
					(rDstTrans->GetDir().y < 0 ? fLegnth_y = rSrtBox->vMaxPos.y - rDstBox->vCenterPos.y :
						fLegnth_y = rSrtBox->vMinPos.y - rDstBox->vCenterPos.y);

					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y + fLegnth_y * fixed_y, rDstBox->vCenterPos.z };
					_rDstCol->Set_CenterPos(vTmpPos);
				}

				else if (tmpLength.z != 0)
				{
					float fLegnth_z = 0.f;
					float fixed_z = 0.f;
					float fDistance = 0.5f;

					(rSrtBox->vMaxPos.z < rDstBox->vCenterPos.z ? fixed_z = 1.f - fDistance : fixed_z = 1.f + fDistance);
					(rDstTrans->GetDir().z < 0 ? fLegnth_z = rSrtBox->vMaxPos.z - rDstBox->vCenterPos.y :
						fLegnth_z = rSrtBox->vMinPos.z - rDstBox->vCenterPos.z);

					D3DXVECTOR3 vTmpPos = { rDstBox->vCenterPos.x , rDstBox->vCenterPos.y, rDstBox->vCenterPos.z + fLegnth_z * fixed_z };
					_rDstCol->Set_CenterPos(vTmpPos);
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

D3DXVECTOR3 CCollisionMgr::Get_Length_Bullet(ENGINE::CGameObject * rDst, ENGINE::CGameObject * rSrc, bool _static)
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
		fabs(vCross.z) == fabs(vCross.x))
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
		return{ 0,0,0 };
	}

	(fabs(vCross.x) < fabs(vCross.y) ? 
		vCross.y = 0, fabs(vCross.x) < fabs(vCross.z) ?
		vCross.z = 0 : vCross.x = 0 :
		vCross.x = 0, fabs(vCross.y) < fabs(vCross.z) ? 
		vCross.z = 0 : vCross.y = 0);


	/*
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
	*/


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