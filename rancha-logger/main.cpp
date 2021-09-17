#define _CRT_SECURE_NO_DEPRECATE
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <WinSock2.h>
#include <detours.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <string>
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <vector>

HMODULE globalhModule;

int WINAPI MySend(SOCKET s, const char* buf, int len, int flags);
int WINAPI MyRecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesReceived, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#define MYMENU_EXIT (WM_APP + 100)
#define SEND_BUTTON (WM_APP + 101)
#define LOG_SEND (WM_APP + 102)
#define LOG_RECV (WM_APP + 103)
#define CLEAR_BUTTON (WM_APP + 104)

HMODULE inj_hModule;
HWND hCraftedPacket;
HWND hLog;

BOOL LogSend = 0;
BOOL LogRecv = 0;

HWND hLogSend;
HWND hLogRecv;

//Socket and flags value
SOCKET tmpSock;
int tmpFlags;

wchar_t craftedBuffer[533];
char bufferToSend[533];
char const hex_chars[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
bool logSentHook = false;
bool logRecvHook = false;

uintptr_t moduleBase;
std::vector<char> logText;

void Quit() {
    PostQuitMessage(0);
    FreeLibraryAndExitThread(globalhModule, 0);
}


HMENU CreateDLLWindowMenu() {
    HMENU hMenu;
    hMenu = CreateMenu();
    HMENU hMenuPopup;
    if (hMenu == NULL)
        return FALSE;
    hMenuPopup = CreatePopupMenu();
    AppendMenuW(hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("File"));
    return hMenu;
}

LRESULT CALLBACK MessageHandler(HWND hWindow, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    switch (uMessage) {
    case WM_CLOSE:
    case WM_DESTROY:
        Quit();
        return 0;
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case MYMENU_EXIT:
            Quit();
            return 0;
            break;
        case SEND_BUTTON:
            GetWindowText(hCraftedPacket, craftedBuffer, 533);
            size_t len;
            wcstombs_s(&len, bufferToSend, 533, craftedBuffer, 533);
            size_t i;
            i = 0;
            for (size_t count = 0; count < len; ++i, count += 3) {
                if (bufferToSend[count] >= 'A') {
                    bufferToSend[count] -= 'A';
                    bufferToSend[count] += 10;
                }
                else {
                    bufferToSend[count] -= 48;
                }

                if (bufferToSend[count + 1] >= 'A') {
                    bufferToSend[count + 1] -= 'A';
                    bufferToSend[count + 1] += 10;
                }
                else {
                    bufferToSend[count + 1] -= 48;
                }

                bufferToSend[i] = (__int8)(((char)bufferToSend[count]) * (char)16);
                bufferToSend[i] += (__int8)bufferToSend[count + 1];
            }
            bufferToSend[i] = '\0';
            MySend(tmpSock, bufferToSend, i, tmpFlags);
            break;
        case LOG_SEND:
            LogSend = IsDlgButtonChecked(hWindow, LOG_SEND);

            if (LogSend == BST_CHECKED) {
                CheckDlgButton(hWindow, LOG_SEND, BST_UNCHECKED);
                logSentHook = false;
            }
            else {
                CheckDlgButton(hWindow, LOG_SEND, BST_CHECKED);
                logSentHook = true;
            }
            break;
        case LOG_RECV:
            LogRecv = IsDlgButtonChecked(hWindow, LOG_RECV);

            if (LogRecv == BST_CHECKED) {
                CheckDlgButton(hWindow, LOG_RECV, BST_UNCHECKED);
                logRecvHook = false;
            }
            else {
                CheckDlgButton(hWindow, LOG_RECV, BST_CHECKED);
                logRecvHook = true;
            }
            break;
        case CLEAR_BUTTON:
            logText.erase(logText.begin(), logText.end());
            SetWindowTextA(hLog, "Cleared! :)\r\nFind Tutorials on Guidedhacking.com!");
        }
    }
    return DefWindowProc(hWindow, uMessage, wParam, lParam);
}

//Register our windows Class
BOOL RegisterDLLWindowClass(const wchar_t szClassName[]) {
    WNDCLASSEX wc;
    wc.hInstance = inj_hModule;
    wc.lpszClassName = (LPCWSTR)szClassName;
    wc.lpfnWndProc = MessageHandler;
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hIcon = LoadIcon(NULL, IDI_SHIELD);
    wc.hIconSm = LoadIcon(NULL, IDI_SHIELD);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    if (!RegisterClassEx(&wc))
        return 0;
    return 1;
}

inline void printSendBufferToLog(int sentLen, const char* sentBuffer) {
    char sendID[] = "[SEND] ";

    while (logText.size() > 4096) {
        logText.erase(logText.begin(), logText.begin() + 400);
    }
    if (logText.size() > 1) {
        logText.pop_back();
        logText.push_back('\r');
        logText.push_back('\n');
    }

    for (DWORD i = 0; i < sentLen + 7; ++i) {
        if (i < 7) {
            logText.push_back(sendID[i]);
        }
        else {
            logText.push_back(hex_chars[((sentBuffer)[i - 7] & 0xF0) >> 4]);
            logText.push_back(hex_chars[((sentBuffer)[i - 7] & 0x0F) >> 0]);
            logText.push_back(' ');
        }
    }
    logText.push_back('\0');
    SetWindowTextA(hLog, &logText[0]);
}

inline void printRecvBufferToLog(DWORD recvLen, char* recvBuffer) {
    char recvID[] = "[RECV] ";

    while (logText.size() > 4096) {
        logText.erase(logText.begin(), logText.begin() + 400);
    }
    if (logText.size() > 1) {
        logText.pop_back();
        logText.push_back('\r');
        logText.push_back('\n');
    }

    for (DWORD i = 0; i < recvLen + 7; ++i) {
        if (i < 7) {
            logText.push_back(recvID[i]);
        }
        else {
            logText.push_back(hex_chars[((recvBuffer)[i - 7] & 0xF0) >> 4]);
            logText.push_back(hex_chars[((recvBuffer)[i - 7] & 0x0F) >> 0]);
            logText.push_back(' ');
        }
    }
    logText.push_back('\0');
    SetWindowTextA(hLog, &logText[0]);
}

DWORD WINAPI WindowThread(HMODULE hModule) {
    logText = std::vector<char>();


    MSG messages;
    HMENU hMenu = CreateDLLWindowMenu();
    HWND hSendButton;
    HWND hClearButton;

    RegisterDLLWindowClass(L"InjectedDLLWindowClass");
    HWND hwnd = CreateWindowEx(0, L"InjectedDLLWindowClass", L"Erarnitox's Tera Proxy | GuidedHacking.com", WS_EX_LAYERED, CW_USEDEFAULT, CW_USEDEFAULT, 1020, 885, NULL, hMenu, inj_hModule, NULL);
    hLog = CreateWindowEx(0, L"edit", L"Tera Proxy made by Erarnitox\r\n!!! visit GuidedHacking.com !!!", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | WS_BORDER | ES_READONLY, 5, 5, 1005, 700, hwnd, NULL, hModule, NULL);

    hClearButton = CreateWindowEx(0, L"button", L"Clear Log", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 710, 100, 30, hwnd, (HMENU)CLEAR_BUTTON, hModule, NULL);
    hSendButton = CreateWindowEx(0, L"button", L"Send", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 800, 100, 30, hwnd, (HMENU)SEND_BUTTON, hModule, NULL);
    hCraftedPacket = CreateWindowEx(0, L"edit", L"<Packet Data>", WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER, 110, 730, 900, 100, hwnd, NULL, hModule, NULL);

    hLogSend = CreateWindowEx(0, L"button", L"Log Send", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 110, 705, 100, 25, hwnd, (HMENU)LOG_SEND, hModule, NULL);
    hLogRecv = CreateWindowEx(0, L"button", L"Log Recv", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 225, 705, 100, 25, hwnd, (HMENU)LOG_RECV, hModule, NULL);

    ShowWindow(hwnd, SW_SHOWNORMAL);

    while (GetMessage(&messages, NULL, 0, 0)) {
        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    //exit:
    Quit();
    
    return 0;
}

#define SEND_LOG_FILE L"C:\\Users\\ritzgames\\Desktop\\daoc\\packetlogs\\SendLogFile.txt"
#define RECV_LOG_FILE L"C:\\Users\\ritzgames\\Desktop\\daoc\\packetlogs\\RecvLogFile.txt"

void WriteSendLog(const char* text, int len)
{
    std::fstream f(SEND_LOG_FILE, std::ios::in | std::ios::out);
    if (f.fail()) {
        std::fstream f(SEND_LOG_FILE, std::ios::trunc | std::ios::out);
        f.seekg(0, std::ios::end); //put pointer to the end of file
        f << "Length: " << std::to_string(len) << "| ";
        for (int i = 0; i < len; i++) {
            f << std::hex << std::setw(2) << std::noshowbase << std::setfill('0') << ((unsigned int)(unsigned char)text[i]) << " ";
        }
        //f.write(const_cast<char*>(text), len);
        f.close();
    }
    else if (f.good()) {
        f.seekg(0, std::ios::end); //put pointer to the end of file
        f.write("\n", 1);
        f << "Length: " << std::to_string(len) << " ";
        for (int i = 0; i < len; i++) {
            f << std::hex << std::setw(2) << std::noshowbase << std::setfill('0') << ((unsigned int)(unsigned char)text[i]) << " ";
        }
        //f.write(const_cast<char*>(text), len);
        f.close();
    }

}

void WriteRecvLog(char* text, int len)
{
    std::fstream f(RECV_LOG_FILE, std::ios::in | std::ios::out);
    if (f.fail()) {
        std::fstream f(RECV_LOG_FILE, std::ios::trunc | std::ios::out);
        f.seekg(0, std::ios::end); //put pointer to the end of file
        f.write("\n", 1);
        f.write(text, len);
        f.close();
    }
    else if (f.good()) {
        f.seekg(0, std::ios::end); //put pointer to the end of file
        f.write("\n", 1);
        f.write(text, len);
        f.close();
    }

}

//https://stackoverflow.com/questions/10065903/winsock-recv-hooking-with-detours
FILE* pSendLogFile;
FILE* pRecvLogFile;

typedef int (WINAPI* SendPtr)(SOCKET s, const char* buf, int len, int flags);
HMODULE hLib = GetModuleHandle(L"ws2_32.dll");
SendPtr pSend = (SendPtr)GetProcAddress(hLib, "send");

typedef int (WINAPI* RecvPtr)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesReceived, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
//typedef int (WINAPI* RecvPtr)(SOCKET s, const char* buf, int len, int flags);
RecvPtr pRecv = (RecvPtr)GetProcAddress(hLib, "WSARecv");

//int (WINAPI* pSend)(SOCKET s, const char* buf, int len, int flags) = send;
int WINAPI MySend(SOCKET s, const char* buf, int len, int flags);
//int (WINAPI* pRecv)(SOCKET s, char* buf, int len, int flags) = recv;
int WINAPI MyRecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesReceived, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);


INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
    LONG error;
    globalhModule = hDLL;

    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hDLL);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)pSend, MySend);
        error = DetourTransactionCommit();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)pRecv, MyRecv);
        error = DetourTransactionCommit();

        HANDLE ThreadHandle = CreateThread(0, NULL, (LPTHREAD_START_ROUTINE)WindowThread, hDLL, NULL, NULL);

        if (ThreadHandle != NULL) {
            CloseHandle(ThreadHandle);
        }
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        //Remove the detour
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)pSend, MySend);
        error = DetourTransactionCommit();

        //MessageBox(0, L"send() detached ", L"asd", MB_OK);

        //Remove the detour
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)pRecv, MyRecv);
        error = DetourTransactionCommit();

        //MessageBox(0, L"recv() detached ", L"asd", MB_OK);

        //error = DetourTransactionCommit();
        //error = DetourTransactionCommit();

        //printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
        //    " Removed Send Detour Removed\n", error, dwSlept);
        fflush(stdout);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}




int WINAPI MySend(SOCKET s, const char* buf, int len, int flags)
{
    tmpSock = s;
    tmpFlags = flags;
    if (logSentHook) {
        printSendBufferToLog(len, buf);
    }
    //WriteSendLog(buf, len);
    //WriteSendLog(buf, len);
    return pSend(s, buf, len, flags);
}

//https://stackoverflow.com/questions/18558702/how-to-handle-hooked-wsarecv
int WINAPI MyRecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesReceived, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    //https://stackoverflow.com/questions/65442338/how-to-convert-printf-output-to-char-array
    if (logRecvHook) {
        printRecvBufferToLog(lpBuffers->len, (char*)lpBuffers->buf);
    }
    //WriteRecvLog(buf, len);
    return pRecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesReceived, lpFlags, lpOverlapped, lpCompletionRoutine);
}

