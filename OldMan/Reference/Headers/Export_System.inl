#include "Export_System.h"

CGraphicDev* GetGraphicDev()
{
	return CGraphicDev::GetInstance();
}

CTimeMgr* GetTimeMgr()
{
	return CTimeMgr::GetInstance();
}

CKeyMgr * GetKeyMgr()
{
	return CKeyMgr::GetInstance();
}
