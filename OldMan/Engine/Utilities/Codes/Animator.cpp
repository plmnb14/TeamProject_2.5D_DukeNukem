#include "Animator.h"
//#include "Texture.h"
//#include "ResourceMgr.h"

USING(ENGINE)

CAnimator::CAnimator()
{
	ZeroMemory(&m_tFrame, sizeof(FRAME));
	m_tFrame.fFrameAmp = 1.f;
}

CAnimator::~CAnimator()
{
}

int CAnimator::RenderSet(float _DeltaTime)
{
	m_fDeltaTime = _DeltaTime;
	return Play_Animation();
}

void CAnimator::Stop_Animation(bool _Stop)
{
	m_tFrame.bStopPlay = _Stop;
}

int CAnimator::Play_Animation()
{
	if (m_tFrame.bIsReverse == false)
	{
		switch (m_eFrame)
		{
		case RESET_ZERO:
		{
			if (m_tFrame.fCurrentFrame >= m_tFrame.fMaxFrameCnt - 1.f)
					m_tFrame.fCurrentFrame = 0.f;
			break;
		}
		case RESET_STOP:
		{
			if (m_tFrame.fCurrentFrame >= m_tFrame.fMaxFrameCnt - 1.f)
				m_tFrame.bStopPlay = true;
			break;
		}
		}

		if (m_tFrame.bStopPlay == false)
			m_tFrame.fCurrentFrame += m_tFrame.fFrameAmp * m_fDeltaTime;

		return (int)m_tFrame.fCurrentFrame;
	}

	if (m_tFrame.bIsReverse == true)
	{
		switch (m_eFrame)
		{
		case RESET_MAX:
		{
			if (m_tFrame.fCurrentFrame < 0)
				m_tFrame.fCurrentFrame = m_tFrame.fMaxFrameCnt;
			break;
		}
		case RESET_STOP:
		{
			if (m_tFrame.fCurrentFrame < 0)
				m_tFrame.bStopPlay = true;
			break;
		}
		}

		if (m_tFrame.bStopPlay == false)
			m_tFrame.fCurrentFrame -= m_tFrame.fFrameAmp * m_fDeltaTime;

		return (int)m_tFrame.fCurrentFrame;
	}

	return 0;
}

void CAnimator::Set_Frame(int _Frame)
{
	m_tFrame.fCurrentFrame = (float)_Frame;
}

int CAnimator::Get_Frame()
{
	//if (m_tFrame.fCurrentFrame >= m_tFrame.fMaxFrameCnt)
	//	m_tFrame.fCurrentFrame = m_tFrame.fMaxFrameCnt - 1.f;

	return (int)m_tFrame.fCurrentFrame;
}

int CAnimator::Get_MaxFrame()
{
	return (int)m_tFrame.fMaxFrameCnt;;
}

//void CAnimator::Change_Animation(CResourceMgr* _Mgr, map<wstring, CComponent*> _mapComponet, CTexture* _pTexture , wstring _StateKey )
//{
//	_mapComponet.erase(L"Texture");
//
//	ENGINE::CComponent* pComponent = nullptr;
//	pComponent = _Mgr->CloneResource(ENGINE::RESOURCE_DYNAMIC, _StateKey);
//	NULL_CHECK(pComponent);
//	_mapComponet.insert({ L"Texture", pComponent });
//
//	_pTexture = dynamic_cast<ENGINE::CTexture*>(pComponent);
//	NULL_CHECK(_pTexture);
//
//
//}

void CAnimator::Set_MaxFrame(float _MaxFrame)
{
	m_tFrame.fMaxFrameCnt = _MaxFrame;
}

void CAnimator::Set_FrameAmp(float _FrameAmp)
{
	m_tFrame.fFrameAmp = _FrameAmp;
}

void CAnimator::Set_Reverse(bool _Reverse)
{
	m_tFrame.bIsReverse = _Reverse;
}

void CAnimator::Set_ResetOption(MAXFRAME _FrameOption)
{
	m_eFrame = _FrameOption;
}

CAnimator * CAnimator::Create()
{
	return new CAnimator();
}
