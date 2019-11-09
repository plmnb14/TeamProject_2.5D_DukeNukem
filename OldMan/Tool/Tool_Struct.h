#pragma once
#ifndef __TOOL_STRUCT_H__

typedef struct tagPathInfo
{
	wstring wstrObjectKey = L"";
	wstring wstrStateKey = L"";
	wstring wstrImgPath = L"";
	wstring wstrFileName = L"";
	int iImgCount = 0;
}PATH_INFO;

typedef struct tagRay
{
	D3DXVECTOR3 vOrigin;
	D3DXVECTOR3 vDir;
}RAY;

#define __TOOL_STRUCT_H__
#endif