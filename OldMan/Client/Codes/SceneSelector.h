#pragma once

#ifndef __SCENESELECTOR_H__

#include "Logo.h"
#include "Stage.h"
#include "Stage_02.h"
#include "Stage_Boss.h"

class CSceneSelector
{
public:
	enum SCENE_TYPE { LOGO, STAGE, STAGE_02, STAGE_BOSS, SCENE_END };

public:
	explicit CSceneSelector(SCENE_TYPE eType)
		: m_eCurrentScene(eType)
	{}

public:
	HRESULT operator()(ENGINE::CScene** ppScene, LPDIRECT3DDEVICE9 pGraphicDev)
	{
		switch (m_eCurrentScene)
		{
		case LOGO:
			*ppScene = CLogo::Create(pGraphicDev);
			break;
		case STAGE:
			*ppScene = CStage::Create(pGraphicDev);
			break;
		case STAGE_02:
			*ppScene = CStage_02::Create(pGraphicDev);
			break;
		case STAGE_BOSS:
			*ppScene = CStage_Boss::Create(pGraphicDev);
			break;
		}

		NULL_CHECK_RETURN(*ppScene, E_FAIL);
		return S_OK;
	}

private:
	SCENE_TYPE	m_eCurrentScene;	
};

#define __SCENESELECTOR_H__
#endif
