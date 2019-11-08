#pragma once

#ifndef __ENGINE_FUNCTION_H__

namespace ENGINE
{
	template <typename T>
	void Safe_Delete(T& ptr)
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& ptr)
	{
		if (ptr)
		{
			delete[] ptr;
			ptr = nullptr;
		}
	}

	template <typename T>
	DWORD Safe_Release(T& ptr)
	{
		NULL_CHECK_RETURN(ptr, 0);

		DWORD dwRefCnt = ptr->Release();

		if (0 == dwRefCnt)
			ptr = nullptr;

		return dwRefCnt;
	}
}

#define __ENGINE_FUNCTION_H__
#endif