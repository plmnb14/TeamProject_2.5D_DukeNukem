#pragma once

#ifndef __ANIMAITOR_H__

#include "Component.h"
//#include "Resources.h"
//#include "ResourceMgr.h"

BEGIN(ENGINE)

class ENGINE_DLL CAnimator : public CComponent
{
private:
	explicit CAnimator();

public:
	virtual ~CAnimator();

public:
	enum MAXFRAME
	{
		RESET_ZERO, RESET_STOP, RESET_MAX
	};

public:
	int RenderSet(float _DeltaTime);

public:
	void Stop_Animation(bool _Stop);
	int Play_Animation();

public:
	//void Change_Animation(CResourceMgr* _Mgr, map<wstring, CComponent*> _mapComponet, CTexture* _pTexture, wstring _StateKey);
	void Set_Frame(int _Frame);
	void Set_MaxFrame(float _MaxFrame);
	void Set_FrameAmp(float _FrameAmp);
	void Set_Reverse(bool _Reverse);
	void Set_ResetOption(MAXFRAME _FrameOption);

public:
	int  Get_Frame();
	int  Get_MaxFrame();
	bool Get_Reverse() { return m_tFrame.bIsReverse; };


public:
	static CAnimator* Create();

private:
	ENGINE::FRAME	m_tFrame;
	MAXFRAME		m_eFrame;
	float			m_fDeltaTime;
};

END

#define __CONDITON_H__
#endif