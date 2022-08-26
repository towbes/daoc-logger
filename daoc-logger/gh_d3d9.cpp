#pragma once
#include "stdafx.h"
#include "gh_d3d9.h"

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	DWORD wndProcId;
	GetWindowThreadProcessId(handle, &wndProcId);

	if (GetCurrentProcessId() != wndProcId)
		return TRUE; // skip to next window

	window = handle;
	return FALSE; // window found abort search
}

HWND GetProcessWindow()
{
	window = NULL;
	EnumWindows(EnumWindowsCallback, NULL);
	return window;
}

bool GetD3D9Device(void** pTable, size_t Size)
{
	if (!pTable)
		return false;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return false;

	IDirect3DDevice9* pDummyDevice = NULL;

	// options to create dummy device
	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();

	HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

	if (dummyDeviceCreated != S_OK)
	{
		// may fail in windowed fullscreen mode, trying again with windowed mode
		d3dpp.Windowed = !d3dpp.Windowed;

		dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDeviceCreated != S_OK)
		{
			pD3D->Release();
			return false;
		}
	}

	memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

	pDummyDevice->Release();
	pD3D->Release();
	return true;
}

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev)
{
	D3DRECT BarRect = { x, y, x + w, y + h };
	dev->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}

bool isModuleLoaded(const wchar_t* modulename, HWND hWnd) {
	// Obtain the Process ID however you like. I used GetWindowThreadProcessId.
	DWORD procId = GetCurrentProcessId();
	if (procId != 0)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
		if (snapshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32W moduleInfo = { 0 };
			moduleInfo.dwSize = sizeof(MODULEENTRY32W);

			BOOL ok = Module32FirstW(snapshot, &moduleInfo);
			if (!ok)
			{
				// The read failed, handle the error here.
			}
			do
			{
				HANDLE hFile = CreateFileW(moduleInfo.szExePath,
					0,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
				if (hFile)
				{
					WCHAR realPath[MAX_PATH];
					DWORD result = GetFinalPathNameByHandleW(hFile,
						realPath,
						MAX_PATH,
						FILE_NAME_NORMALIZED);
					if (result > 0)
					{
						std::wstring loadedModule = realPath;
						std::wstring checkModule = modulename;
						if (loadedModule.find(checkModule) != std::wstring::npos) {
							//std::wcout << L"Module: " << realPath << std::endl;
							return true;
						}
						//std:: wcout << L"Module: " << realPath << std::endl;
					}

					CloseHandle(hFile);
				}
				else
				{
					//std::wcout << L"Module Name: " << moduleInfo.szExePath << std::endl;
				}
			} while (Module32NextW(snapshot, &moduleInfo));

			CloseHandle(snapshot);
		}
	}
	return false;
}