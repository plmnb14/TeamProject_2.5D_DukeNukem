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

typedef struct tagTexture
{
	LPDIRECT3DTEXTURE9	pTexture;
	D3DXIMAGE_INFO		tImgInfo;
}TEX_INFO;


#define __TOOL_STRUCT_H__
#endif