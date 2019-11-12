// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

// Standard Headers
#include <iostream>

using namespace std;

// Client Headers
#include "Define.h"
#include "Extern.h"
#include "Function.h"
#include "Functor.h"
#include "Struct.h"

// Engine Headers
#include "Export_Function.h"

#pragma comment(lib, "System.lib")
#pragma comment(lib, "Utilities.lib")
#pragma comment(lib, "Resources.lib")


// �ٸ��� ����
#pragma comment(linker,"/entry:wWinMainCRTStartup /subsystem:console")


// VLD
//#include <vld.h>

