#include "stdafx.h"
#include "PacketProcessor.h"
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
//HRESULT APIENTRY
HRESULT APIENTRY hkPresent(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{

    //draw stuff here like so:
    if (!bInit) InitImGui(pDevice);
    else {

        //Menu
        DrawGui();

        GameLoops();
    }

    return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
//HRESULT APIENTRY
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

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"game.dll");
    char* name = (char*)(moduleBase + 0xc4a6a8);
    if (name[0] == 0x59) {
        FreeLibraryAndExitThread(hModule, 0);
        return 0;
    }

    //Daoc Addresses
    LoadHooks();


    //D3d9 hook
    void* ptrPresent = NULL;
    void* ptrReset = NULL;
    window = GetProcessWindow();

    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
    {

        //Check for mojo dll
        if (!isModuleLoaded(L"mojo_remote_cp.dll", window)) {
            ptrPresent = d3d9Device[17];
            ptrReset = d3d9Device[16];
        }
        else {
            char* tempPtr = (char*)d3d9Device[17];
            tempPtr += 0x2f;
            void* ptrPresentTemp = *(void**)tempPtr;
            ptrPresent = *(void**)ptrPresentTemp;

            char* tempPtr2 = (char*)d3d9Device[16];
            tempPtr2 += 0x1E;
            void* ptrResetTemp = *(void**)tempPtr2;
            ptrReset = *(void**)ptrResetTemp;
        }

        if (ptrPresent != NULL && ptrReset != NULL) {

            //DetourRestoreAfterWith();
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)ptrPresent, hkPresent);
            long result = DetourTransactionCommit();
            if (result != NO_ERROR)
            {
                
            }
            
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)ptrReset, hkReset);
            result = DetourTransactionCommit();
            if (result != NO_ERROR)
            {

            }

            oPresent = (tPresent)ptrPresent;
            oReset = (tReset)ptrReset;
        }

    }

    origWndProc = (WNDPROC)GetWindowLongPtr(window, GWL_WNDPROC);
    oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);



    while (true) {
        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }
        //Sleep to prevent crushing the CPU
        Sleep(50);
    }
    
    //Restore WndProc
    (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)origWndProc);


    if (ptrPresent != NULL && ptrReset != NULL) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)ptrPresent, hkPresent);
        long result = DetourTransactionCommit();
        if (result != NO_ERROR)
        {

        }

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)ptrReset, hkReset);
        result = DetourTransactionCommit();
        if (result != NO_ERROR)
        {

        }

        cleanupImgui();
        bInit = false;
    }

    //Daoc Addresses
    UnloadHooks();

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


