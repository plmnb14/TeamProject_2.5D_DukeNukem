#pragma once
#ifndef __MOUSE_H__

class CMouse
{
private:
	CMouse();
	~CMouse();

public:
	static D3DXVECTOR3 GetMouse();
	//static void Render();

private:
};

#define __MOUSE_H__
#endif
