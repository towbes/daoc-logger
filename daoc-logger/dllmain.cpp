#include "stdafx.h"

#include "Hook.h"

#include "PacketProcessor.h"
#include "mem.h"
#include "gh_d3d9.h"

#include "menuGui.h"


bool bInit = false;

tPresent oPresent = nullptr;
tReset oReset = nullptr;
LPDIRECT3DDEVICE9 pD3DDevice = nullptr;
static WNDPROC origWndProc = nullptr;
static WNDPROC oWndProc = nullptr;
void* d3d9Device[119];

char oPresBytes[11];
char oResetBytes[11];

bool done = false;

HHOOK mouseHook = NULL;

void cleanupImgui() {
    /* Delete imgui to avoid errors */
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void InitImGui(IDirect3DDevice9* pDevice) {
    D3DDEVICE_CREATION_PARAMETERS CP;
    pDevice->GetCreationParameters(&CP);
    window = CP.hFocusWindow;
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->AddFontDefault();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);
    bInit = true;
    return;
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return 1;
    }


    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Check if ImGui wants to handle the keyboard..
    if (msg >= WM_KEYFIRST && msg <= WM_KEYLAST)
    {
        if (io.WantTextInput || io.WantCaptureKeyboard || ImGui::IsAnyItemActive())
            return 1;
    }

    // Check if ImGui wants to handle the mouse..
    if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
    {
        if (io.WantCaptureMouse)
            return 1;
    }

    return ::CallWindowProcA(oWndProc, hWnd, msg, wParam, lParam);
}

HRESULT APIENTRY hkPresent(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{

    //draw stuff here like so:
    if (!bInit) InitImGui(pDevice);
    else {

        DrawGui();
    }

    return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT APIENTRY hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{

    if (bInit) {
        /* Delete imgui to avoid errors */
        cleanupImgui();
    }


    return oReset(pDevice, pPresentationParameters);
}



DWORD WINAPI Init(HMODULE hModule)
{
    //#ifdef _DEBUG
    //	//Create Console
    //	AllocConsole();
    //	FILE* f;
    //	freopen_s(&f, "CONOUT$", "w", stdout);
    //	std::cout << "DLL got injected!" << std::endl;
    //#endif 

    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
    {
        //write original bytes to buffer for cleanup later
        memcpy(oPresBytes, (char*)d3d9Device[17], 5);
        memcpy(oResetBytes, (char*)d3d9Device[16], 5);

        //mojo
        //memcpy(oPresBytes, (char*)d3d9Device[17], 7);
        //memcpy(oResetBytes, (char*)d3d9Device[16], 10);

        std::cout << "present: 0x" << std::hex << d3d9Device[17] << " reset: 0x" << d3d9Device[16] << std::endl;
        oPresent = (tPresent)TrampHook((char*)d3d9Device[17], (char*)hkPresent, 5);
        oReset = (tReset)TrampHook((char*)d3d9Device[16], (char*)hkReset, 5);
        // mojo
        //oPresent = (tPresent)TrampHook((char*)d3d9Device[17], (char*)hkPresent, 7);
        //oReset = (tReset)TrampHook((char*)d3d9Device[16], (char*)hkReset, 10);
    }
    window = GetProcessWindow();
    origWndProc = (WNDPROC)GetWindowLongPtr(window, GWL_WNDPROC);
    oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);

    //Daoc Addresses
    LoadHooks();

    while (true) {
        if (GetAsyncKeyState(VK_RCONTROL) & 1) {
            break;
        }
    }

    //#ifdef _DEBUG
    //	if (f != 0) {
    //		fclose(f);
    //	}
    //	FreeConsole();
    //#endif 
    

    (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)origWndProc);
    //original
    WriteMem((char*)d3d9Device[17], oPresBytes, 5);
    WriteMem((char*)d3d9Device[16], oResetBytes, 5);
    //mojo
    //WriteMem((char*)d3d9Device[17], oPresBytes, 7);
    //WriteMem((char*)d3d9Device[16], oResetBytes, 10);
    cleanupImgui();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Init, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


