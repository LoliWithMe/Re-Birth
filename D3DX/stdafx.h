// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <malloc.h>
#include <memory.h>
#include <string>
#include <tchar.h>
#include <assert.h>
#include <vector>
#include <math.h>
#include <map>
#include <list>

using namespace std;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// 프레임워크
#include "Framework\Framework.h"

// 전역변수
extern HWND g_hWnd;
extern float g_fWheel;
extern POINT g_ptMouse;
extern LPD3DXSPRITE g_sprite;
extern bool g_isDebug;
extern bool g_isPause;
extern DWORD g_clearColor;
extern float g_fVolume;

#define MOUSE_WHEEL g_fWheel
#define MOUSE_POS g_ptMouse
#define SPRITE g_sprite
#define SPRITE_BEGIN g_sprite->Begin(D3DXSPRITE_ALPHABLEND)
#define SPRITE_END g_sprite->End()
#define DEBUG g_isDebug
#define PAUSE g_isPause
#define VOLUME g_fVolume