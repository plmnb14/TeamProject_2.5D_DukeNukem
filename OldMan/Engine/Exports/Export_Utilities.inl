#include "Export_Utilities.h"

CManagement* GetManagement()
{
	return CManagement::GetInstance();
}

inline CCameraSubject * GetCameraSubject()
{
	return CCameraSubject::GetInstance();
}

inline CPlayerSubject* GetPlayerSubject()
{
	return CPlayerSubject::GetInstance();
}

inline CBossSubject * GetBossSubject()
{
	return GetBossSubject::GetInstance();
}
