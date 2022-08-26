#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "mem.h"

//typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);

typedef HRESULT(APIENTRY* tPresent)(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
typedef HRESULT(APIENTRY* tReset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

static HWND window;

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);

HWND GetProcessWindow();

bool GetD3D9Device(void** pTable, size_t Size);

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev);

bool isModuleLoaded(const wchar_t* modulename, HWND hWnd);