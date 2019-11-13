#include "Export_Utilities.h"

CManagement* GetManagement()
{
	return CManagement::GetInstance();
}

inline CCameraSubject * GetCameraSubject()
{
	return CCameraSubject::GetInstance();
}
