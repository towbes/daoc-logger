/*
daoc-logger - Copyright (c) 2022 towbes
Contact: discord afkgamer#0162
Contact: https://github.com/towbes

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
/*
#define MYMENU_EXIT (WM_APP + 100)
#define SEND_BUTTON (WM_APP + 101)
#define LOG_SEND (WM_APP + 102)
#define LOG_RECV (WM_APP + 103)
#define CLEAR_BUTTON (WM_APP + 104)
#define FILTER_ITEMS (WM_APP + 105)
#define CLEANITEM_BUTTON (WM_APP + 106)
#define SELLINV_BUTTON (WM_APP + 107)
#define TOGGLE_RUNSPEED (WM_APP + 108)
#define CMB_SPEED (WM_APP + 109)
#define GO_POS (WM_APP + 110)
#define ENT_LIST (WM_APP + 111)
#define PLYR_OFFSET (WM_APP + 112)
#define NPC_OFFSET (WM_APP + 113)
#define PLYR_BUFFS (WM_APP + 114)
#define PLYR_SKILLS (WM_APP + 115)
#define USE_SKILL (WM_APP + 116)

HMODULE inj_hModule;
HWND hCraftedPacket;
HWND hLog;

BOOL LogSend = 0;
BOOL LogRecv = 0;

HWND hLogSend;
HWND hLogRecv;
HWND hRunSpeedCheck;
HWND hRunSpeedValues;
//Item index for run speed values combo box
int ItemIndex = 0;
BOOL runToggle = 0;
BOOL sendThreadRunning = false;

//Position and heading
HWND hDlgPosX;
HWND hDlgPosY;
HWND hDlgPosZ;
HWND hDlgPosHeading;
#define TIMER_DURATION 100
static UINT idTimer = 1;
void updatePos();
wchar_t posBuffer[10];
int newPosX = 0;
int newPosY = 0;

//Position and heading
HWND hDlgNewPosX;
HWND hDlgNewPosY;
HWND hDlgNewPosZ;
HWND hDlgNewPosHeading;

HWND hDlgPlyrOid;
HWND hDlgNPCOid;
HWND hEntButton;
HWND hPlyrOffset;
HWND hNPCOffset;

//Skill Id
HWND hDlgSkillId;
wchar_t strSkillId[10];

wchar_t craftedBuffer[533];
char bufferToSend[533];
std::vector<char> newBufferToSend(65);
//Pointer used in send function
char* newBuff;
char headerToSend[10];
char appedToLog[533];
char const hex_chars[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
std::vector<char> logText;

//uintptr_t moduleBase;
//HMODULE kernel32base;

//stores object id to pass to GetEntityPointer
wchar_t strNPCObjectId[10];
wchar_t strPlyrObjectId[10];



HMENU CreateDLLWindowMenu() {
    HMENU hMenu;
    hMenu = CreateMenu();
    HMENU hMenuPopup;
    if (hMenu == NULL)
        return FALSE;
    hMenuPopup = CreatePopupMenu();
    AppendMenuW(hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
    //AppendMenuW(hMenuPopup, MF_STRING, NULL, TEXT("Export Log"));
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, TEXT("File"));

    return hMenu;
}

LRESULT CALLBACK MessageHandler(HWND hWindow, UINT uMessage, WPARAM wParam, LPARAM lParam) {

    switch (uMessage) {
    case WM_TIMER:
        // update position labels whenever timer ticks
        updatePos();

        return 0;
    case WM_CLOSE:
    case WM_DESTROY:
        KillTimer(hWindow, idTimer);
        PostQuitMessage(0);
        return 0;
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case MYMENU_EXIT:
            PostQuitMessage(0);
            return 0;
            break;
        case CMB_SPEED:
            ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                (WPARAM)0, (LPARAM)0);
            //Set the run speed based on index
            switch (ItemIndex) {
            case 0:
                newRunspeed = 0xEE; // default run speed
                break;
            case 1:
                newRunspeed = 0x144; // caster speed
                break;
            case 2:
                newRunspeed = 0x19A; // between speed
                break;
            case 3:
                newRunspeed = 0x1F0; // bard speed
                break;
            }
#ifdef _DEBUG
            std::cout << "Run speed value : " << std::hex << newRunspeed << std::endl;
#endif
            break;
        case TOGGLE_RUNSPEED:
            runToggle = IsDlgButtonChecked(hWindow, TOGGLE_RUNSPEED);
#ifdef _DEBUG
            std::cout << "Run speed is " << ((runToggle != BST_CHECKED) ? "enabled" : "disabled") << std::endl;
            if (playerPositionPtr != 0x00) {
                std::cout << "PlayerPosPtr is " << std::hex << playerPositionPtr << std::endl;
                std::cout << "Player x is " << std::setprecision(10) << playerPosition->pos_x << std::endl;
            }
#endif
            if (runToggle == BST_CHECKED) {
                CheckDlgButton(hWindow, TOGGLE_RUNSPEED, BST_UNCHECKED);
                changeRunSpeed = false;
            }
            else {
                CheckDlgButton(hWindow, TOGGLE_RUNSPEED, BST_CHECKED);
                changeRunSpeed = true;
            }
            break;
        case SEND_BUTTON:
#ifdef _DEBUG
            std::cout << "Send pressed!" << std::endl;
#endif
            GetWindowText(hCraftedPacket, craftedBuffer, 533);
            size_t len;

            //convert buffer to chars:
            wcstombs_s(&len, bufferToSend, 533, craftedBuffer, 533);
#ifdef _DEBUG
            std::cout << bufferToSend << std::endl;
            printf("Len: %zu\n", len);
#endif
            //convert buffer to Bytes:
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

#ifdef _DEBUG
            //Debug output
            for (size_t x = 0; x < i; ++x) {
                std::cout << std::hex << (((int)bufferToSend[x]) & 0xff) << " ";
            }
            std::cout << std::endl;

            //call internal send function
            printf("Len: %zu\n", i);
#endif
            //make a pointer to the buffer+1 to account for packet header
            newBuff = bufferToSend;
            newBuff++;
            //send first char of bufferToSend for the header
            //Subtract 1 from i to account for the packet header
            // last parameter is usually 0xffff
            Send(newBuff, bufferToSend[0], i - 1, 0xffff);
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
#ifdef _DEBUG
            std::cout << "Recv Logging " << ((LogRecv != BST_CHECKED) ? "enabled" : "disabled") << std::endl;
#endif
            if (LogRecv == BST_CHECKED) {
                CheckDlgButton(hWindow, LOG_RECV, BST_UNCHECKED);
                logRecvHook = false;
            }
            else {
                CheckDlgButton(hWindow, LOG_RECV, BST_CHECKED);
                logRecvHook = true;
            }
            break;

        case FILTER_ITEMS:
            LogRecv = IsDlgButtonChecked(hWindow, FILTER_ITEMS);
#ifdef _DEBUG
            std::cout << "Filtering items " << ((LogRecv != BST_CHECKED) ? "enabled" : "disabled") << std::endl;
#endif
            if (LogRecv == BST_CHECKED) {
                CheckDlgButton(hWindow, FILTER_ITEMS, BST_UNCHECKED);
                filterItemFlag = false;
            }
            else {
                CheckDlgButton(hWindow, FILTER_ITEMS, BST_CHECKED);
                filterItemFlag = true;
            }
            break;
        case CLEANITEM_BUTTON:
            m_cleanInventory();
            break;
        case SELLINV_BUTTON:

            for (int i = 47; i < 80; i++) {
                if (m_readItemId(i) > 0) {
#ifdef _DEBUG
                    std::cout << "Slot number: " << std::hex << i << std::endl;

#endif
                    SellRequest(i);

                }
            }
            break;
        case CLEAR_BUTTON:
            logText.erase(logText.begin(), logText.end());
            SetWindowTextA(hLog, "Cleared! :)\r\nFind Tutorials on Guidedhacking.com!");
            break;
        case GO_POS:
            break;

        case ENT_LIST:
            //
            for (int i = 0; i < 2000; i++) {
                if (EntityList[i] != 0) {
                    unsigned char* tempPtr = reinterpret_cast<unsigned char*>(EntityList[i]);
                    tempPtr += 0x23c;
                    std::cout << std::dec << "Offset " << i << ": " << std::hex << (uintptr_t)EntityList[i] << " ObjectID: " << std::dec << *(short*)tempPtr << " Name: " << entNameList[i].name << std::endl;
                }
            }

            break;
        case NPC_OFFSET:
            // offset from OID
            GetWindowText(hDlgNPCOid, strNPCObjectId, 10);
            //objectId = 9256;
            //npcEntityListOffset = 0;
            //npcEntityListOffset = GetNPCEntityOffsetFromOid(_wtoi(strNPCObjectId));

            findEntOffset = findEntityByOid(_wtoi(strNPCObjectId));
            std::cout << "Entity offset: " << std::dec << findEntOffset << std::endl;
            //if (npcEntityListOffset != 0) {
            //    std::cout << "Entity offset: " << std::hex << (int)npcEntityListOffset << std::endl;
            //}
            ////reset the strObjectId buffer
            //for (int i = 0; i < 10; i++) {
            //    strObjectId[i] = '\0';
            //}
            break;
        case PLYR_OFFSET:

            //player flag is at 0x28e, 4 is a player
            std::cout << "---Current Players---\n";
            for (int i = 0; i < 2000; i++) {
                if (EntityList[i] != 0) {
                    unsigned char* tempPtr = reinterpret_cast<unsigned char*>(EntityList[i]);
                    if (*(short*)(tempPtr + 0x28e) == 4) {
                        std::cout << "Offset " << i << ": " << std::hex << (uintptr_t)EntityList[i] << " ObjectID: " << *(uint16_t*)(tempPtr + 0x23c) << std::endl;
                    }
                }
            }
            //
            ////Now lookup that offset in our EntityList
            //for (int i = 0; i < 1000; i++) {
            //    //break on 0xff
            //    if (playerEntityOffsets[i] == 0 || playerEntityOffsets[i] == 0xffffffff) {
            //        std::cout << "broke loop" << std::endl;
            //        break;
            //    }
            //    std::cout << "search plyrEntityListOffset: " << std::hex << playerEntityOffsets[i] << std::endl;
            //    findEntityByOffset(playerEntityOffsets[i]);
            //}

            // offset from OID
            //GetWindowText(hDlgPlyrOid, strPlyrObjectId, 10);
            ////objectId = 9256;
            //plyrEntityListOffset = 0;
            //plyrEntityListOffset = GetPlyrEntityOffsetFromListIndex(_wtoi(strPlyrObjectId));
            //
            //if (plyrEntityListOffset != 0) {
            //    std::cout << "Entity offset: " << std::hex << (int)plyrEntityListOffset << std::endl;
            //}
            break;
        case PLYR_BUFFS:

            for (int i = 0; i < 75; i++) {
                if (plyrBuffTable[i].name[0] != '\0')
                    std::cout << std::dec << "Buff " << i << ": " << plyrBuffTable[i].name << std::endl;
            }
            break;
        case PLYR_SKILLS:
            for (int i = 0; i < 150; i++) {
                if (plyrUseSkillTable[i].name[0] != '\0') {
                    std::cout << std::dec << "Skill " << i << ": " << plyrUseSkillTable[i].name << std::endl;
                }
            }
            break;
        case USE_SKILL:
            GetWindowText(hDlgSkillId, strSkillId, 10);
            UseSkill(_wtoi(strSkillId), 1);
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

inline void updatePos() {

    if (playerPosition != NULL) {
        std::wstringstream tmp;
        tmp << std::fixed << std::setprecision(0) << playerPosition->pos_x;
        SetWindowText(hDlgPosX, tmp.str().c_str());
        tmp.str(L"");
        tmp << std::fixed << std::setprecision(0) << playerPosition->pos_y;
        SetWindowText(hDlgPosY, tmp.str().c_str());
        tmp.str(L"");
        tmp << std::fixed << std::setprecision(0) << playerPosition->pos_z;
        SetWindowText(hDlgPosZ, tmp.str().c_str());
        SetWindowText(hDlgPosHeading, std::to_wstring(playerPosition->heading).c_str());
    }
}

inline void printSendBufferToLog() {
    char sendID[] = "[SEND] ";
    char packHeader = (char)packetHeader;
#ifdef _DEBUG
    std::cout << "Sent Packet len: " << std::dec << sentLen << std::endl;
    std::cout << "Packet header is: " << std::hex << packetHeader << std::endl;
#endif
    while (logText.size() > 4096) {
        logText.erase(logText.begin(), logText.begin() + 400);
    }
    if (logText.size() > 1) {
        logText.pop_back();
        logText.push_back('\r');
        logText.push_back('\n');
    }
    //Packet header is in 
    for (DWORD i = 0; i < sentLen + 8; ++i) {
        if (i < 7) {
            logText.push_back(sendID[i]);
        }
        //do this once
        else if (i < 8) {
            logText.push_back(hex_chars[((packHeader) & 0xF0) >> 4]);
            logText.push_back(hex_chars[((packHeader) & 0x0F) >> 0]);
            logText.push_back(' ');
        }
        else {
            logText.push_back(hex_chars[((sentBuffer)[i - 8] & 0xF0) >> 4]);
            logText.push_back(hex_chars[((sentBuffer)[i - 8] & 0x0F) >> 0]);
            logText.push_back(' ');
        }
    }
    logText.push_back('\0');
    SetWindowTextA(hLog, &logText[0]);
}

//variables to track timer between sending packets isntead of using sleep
const std::chrono::milliseconds duration = std::chrono::milliseconds(1000);
auto begin = std::chrono::high_resolution_clock::now();
auto end = std::chrono::high_resolution_clock::now();


//Might have to use Semapores if Recv and Send run in different threads
inline void printRecvBufferToLog() {
    char recvID[] = "[RECV] ";
#ifdef _DEBUG
    std::cout << "Recieved Packet len: " << std::dec << recvLen << std::endl;
#endif
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

#ifdef _DEBUG
    //Create Console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "DLL got injected!" << std::endl;
#endif 

    logText = std::vector<char>();

    moduleBase = (uintptr_t)GetModuleHandle(moduleName);

    ////patch antidebug https://guidedhacking.com/threads/how-to-bypass-anticheat-start-here-beginners-guide.9882/
    //kernel32base = GetModuleHandle(L"kernel32.dll");
    //char* functionAddress = (char*)GetProcAddress(kernel32base, "IsDebuggerPresent");
    ////xor eax, eax; return near, nop nop
    //Patch(functionAddress, (char*)"\x31\xC0\xC3\x90\x90", 5);
    //uintptr_t pebAddress = __readfsdword(0x30);
    //Patch((char*)pebAddress + 0x2, (char*)"\x0", 1);

    char* name = (char*)(moduleBase + 0xc4a6a8);
    std::cout << "Server name is: " << name[0] << std::endl;
    if (name[0] != 'G' && name[0] != 'D') {
#ifdef _DEBUG
        if (f != 0) {
            fclose(f);
        }
        FreeConsole();
#endif 

        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
        return 0;
    }

    //Send hook
    Send = (_SendPacket)(ScanModIn((char*)internalSendPattern, (char*)internalSendMask, "game.dll"));
    //Sell Hook
    SellRequest = (_SellRequest)(ScanModIn((char*)sellRequestPattern, (char*)sellRequestMask, "game.dll"));
    //UseSkill Hook
    UseSkill = (_UseSkill)(ScanModIn((char*)funcUseSkillPattern, (char*)funcUseSkillMask, "game.dll"));
    //Receive hook
    void* toHookRecv = (void*)(ScanModIn((char*)internalRecvPattern, (char*)internalRecvMask, "game.dll"));

    //entity list hook
    void* toHookEntity = (void*)(ScanModIn((char*)entityLoopPattern, (char*)entityLoopMask, "game.dll"));

    //entity name list
    void* toHookEntityNames = (void*)(ScanModIn((char*)entNameHookPattern, (char*)entNameHookMask, "game.dll"));

    //player buffs address
    plyrBuffTableTemp = (void*)(ScanModIn((char*)plyrBuffTablePattern, (char*)plyrBuffTableMask, "game.dll"));
    plyrBuffTableLoc = (DWORD)((size_t)plyrBuffTableTemp + 0x1);


    //Player skills
    plyrSkillTableTemp = (void*)(ScanModIn((char*)plyrSkillTablePattern, (char*)plyrSkillTableMask, "game.dll"));
    plyrSkillTableLoc = (DWORD)((size_t)plyrSkillTableTemp + 0x1);
    plyrUseSkillTableTemp = (void*)(ScanModIn((char*)plyrUseSkillTablePattern, (char*)plyrUseSkillTableMask, "game.dll"));
    plyrUseSkillTableLoc = (DWORD)((size_t)plyrUseSkillTableTemp + 0x1);


    //Runspeed Hook
    void* toHookRunSpeed = (void*)(ScanModIn((char*)runSpeedPattern, (char*)runSpeedMask, "game.dll"));
    //Player position
    void* ptrPlayerPosition = (void*)(ScanModIn((char*)ptrPlayerPositionPattern, (char*)ptrPlayerPositionMask, "game.dll"));
    DWORD ptrPlayerPositionLocation = (DWORD)((size_t)ptrPlayerPosition + 0x2);
    void* playerPositionPtr = *(void**)ptrPlayerPositionLocation;
    playerPosition = *(playerpos_t**)playerPositionPtr;

    //Autorun Hook
    void* autorunPtr = (void*)(ScanModIn((char*)autorunPattern, (char*)autorunMask, "game.dll"));
    void* autorunTogglePtr = (void*)((size_t)autorunPtr + 2);
    void* autorunTogglePtr2 = *(void**)autorunTogglePtr;
    autorunToggle = *(BYTE*)autorunTogglePtr2;
    //endscene hook location
    //void* endScenePtr = (void*)(ScanModIn((char*)endScenePattern, (char*)endSceneMask, "d3d9.dll"));


#ifdef _DEBUG
    std::cout << "module base is:" << std::hex << (int)moduleBase << std::endl;
    std::cout << "send function location:" << std::hex << (int)Send << std::endl;
    std::cout << "entity function location:" << std::hex << (int)toHookEntity << std::endl;
    std::cout << "entityNames function location:" << std::hex << (int)toHookEntityNames << std::endl;
    std::cout << "plyrBuff Table location:" << std::hex << *(int*)plyrBuffTableLoc << std::endl;
    std::cout << "plyrSkill Table location:" << std::hex << *(int*)plyrSkillTableLoc << std::endl;
    std::cout << "plyrUseSkill Table location:" << std::hex << *(int*)plyrUseSkillTableLoc << std::endl;
    std::cout << "recv function location:" << std::hex << (int)toHookRecv << std::endl;
    std::cout << "Sell request location:" << std::hex << (int)SellRequest << std::endl;
    std::cout << "runspeed function location:" << std::hex << (int)toHookRunSpeed << std::endl;
    std::cout << "Player position location:" << std::hex << (int)playerPosition << std::endl << std::endl;
    //std::cout << "autorunPtr location:" << std::hex << (int)autorunPtr << std::endl;
    std::cout << "autorunTogglePtr: " << std::hex << (int)autorunTogglePtr << std::endl;
    std::cout << "autorunTogglePtr2: " << std::hex << (int)autorunTogglePtr2 << std::endl << std::endl;
    std::cout << "autorunToggle value: " << std::hex << (int)autorunToggle << std::endl;

    //std::cout << "d3d9 endscene location:" << std::hex << (int)endScenePtr << std::endl;
#endif // _DEBUG

    jmpBackAddrSend = (size_t)Send + sendHookLen;
    jmpBackAddrRecv = (size_t)toHookRecv + recvHookLen;
    jmpBackAddrRunSpeed = (size_t)toHookRunSpeed + runSpeedHookLen;
    jmpBackAddrEntity = (size_t)toHookEntity + entityLoopHookLen;
    jmpBackEntityNames = (size_t)toHookEntityNames + entityNamesHookLen;

#ifdef _DEBUG
    std::cout << "[Send Jump Back Addy:] 0x" << std::hex << jmpBackAddrSend << std::endl;
    std::cout << "[Recv Jump Back Addy:] 0x" << std::hex << jmpBackAddrRecv << std::endl;
    std::cout << "[RunSpeed Jump Back Addy:] 0x" << std::hex << jmpBackAddrRunSpeed << std::endl;
    std::cout << "[Entity Jump Back Addy:] 0x" << std::hex << jmpBackAddrEntity << std::endl;
    std::cout << "[EntityNames Jump Back Addy:] 0x" << std::hex << jmpBackEntityNames << std::endl;

#endif

    //Hook* sendHook = new Hook((void*)Send, (void*)sendHookFunc, sendHookLen);
    //Hook* recvHook = new Hook(toHookRecv, recvHookFunc, recvHookLen);
    Hook* runSpeedHook = new Hook(toHookRunSpeed, runSpeedHookFunc, runSpeedHookLen);
    Hook* entityLoopHook = new Hook(toHookEntity, entityLoopFunc, entityLoopHookLen);
    Hook* entityNamesHook = new Hook(toHookEntityNames, entityNamesFunc, entityNamesHookLen);

#ifdef _DEBUG
    std::cout << "[Player Position Pointer:] 0x" << std::hex << (int)playerPosition << std::endl;
    if (ptrPlayerPositionLocation > 0) {
        std::cout << "[Player Position X:] " << std::setprecision(5) << playerPosition->pos_x << std::endl;
    }
#endif

    MSG messages;
    HMENU hMenu = CreateDLLWindowMenu();
    HWND hSendButton;
    HWND hClearButton;
    HWND hFilterItems;
    HWND hCleanItemButton;
    HWND hSellInventoryButton;
    HWND hPlyrBuffsButton;
    HWND hPlyrSkillsButton;
    HWND hUseSkillButton;
    HWND hGoButton;

    RegisterDLLWindowClass(L"InjectedDLLWindowClass");
    HWND hwnd = CreateWindowEx(0, L"InjectedDLLWindowClass", L"DAOC-Logger and Tools| Based on code from GuidedHacking.com", WS_EX_LAYERED, CW_USEDEFAULT, CW_USEDEFAULT, 1020, 1085, NULL, hMenu, inj_hModule, NULL);
    hLog = CreateWindowEx(0, L"edit", L"DAOC-Logger and Tools - Based on Tera Proxy made by Erarnitox", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER | ES_READONLY, 5, 5, 1005, 700, hwnd, NULL, hModule, NULL);

    hClearButton = CreateWindowEx(0, L"button", L"Clear Log", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 710, 100, 30, hwnd, (HMENU)CLEAR_BUTTON, hModule, NULL);
    hCleanItemButton = CreateWindowEx(0, L"button", L"Clean Items", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 750, 100, 30, hwnd, (HMENU)CLEANITEM_BUTTON, hModule, NULL);
    hSellInventoryButton = CreateWindowEx(0, L"button", L"Sell Inventory", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 790, 100, 30, hwnd, (HMENU)SELLINV_BUTTON, hModule, NULL);
    hPlyrBuffsButton = CreateWindowEx(0, L"button", L"Show Buffs", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 830, 100, 30, hwnd, (HMENU)PLYR_BUFFS, hModule, NULL);
    hPlyrSkillsButton = CreateWindowEx(0, L"button", L"Show Skills", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 870, 100, 30, hwnd, (HMENU)PLYR_SKILLS, hModule, NULL);
    hSendButton = CreateWindowEx(0, L"button", L"Send", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 5, 1000, 100, 30, hwnd, (HMENU)SEND_BUTTON, hModule, NULL);
    hCraftedPacket = CreateWindowEx(0, L"edit", L"<Packet Data>", WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER, 110, 1000, 900, 100, hwnd, NULL, hModule, NULL);

    //Current Position labels
    HWND hLabelPos = CreateWindowEx(0, L"STATIC", L"CurrPos", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 730, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelX = CreateWindowEx(0, L"STATIC", L"X: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 750, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelY = CreateWindowEx(0, L"STATIC", L"Y: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 775, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelZ = CreateWindowEx(0, L"STATIC", L"Z: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 800, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelH = CreateWindowEx(0, L"STATIC", L"H: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 825, 100, 25, hwnd, NULL, hModule, NULL);
    SetTimer(hwnd, idTimer, TIMER_DURATION, (TIMERPROC)NULL);

    //Current position dialogs
    hDlgPosX = CreateWindowEx(0, L"STATIC", L"55555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140, 750, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgPosY = CreateWindowEx(0, L"STATIC", L"55555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140, 775, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgPosZ = CreateWindowEx(0, L"STATIC", L"5555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140, 800, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgPosHeading = CreateWindowEx(0, L"STATIC", L"5555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140, 825, 100, 25, hwnd, NULL, hModule, NULL);

    int x_offset = 100;
    //New position labels
    HWND hLabelNewPos = CreateWindowEx(0, L"STATIC", L"New Pos", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x_offset, 730, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelNewX = CreateWindowEx(0, L"STATIC", L"New X: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x_offset, 750, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelNewY = CreateWindowEx(0, L"STATIC", L"New Y: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x_offset, 775, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelNewZ = CreateWindowEx(0, L"STATIC", L"New Z: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x_offset, 800, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelNewH = CreateWindowEx(0, L"STATIC", L"H: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x_offset, 825, 100, 25, hwnd, NULL, hModule, NULL);

    int x2_offset = x_offset + 60;
    //New position dialogs
    hDlgNewPosX = CreateWindowEx(0, L"EDIT", L"55555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x2_offset, 750, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgNewPosY = CreateWindowEx(0, L"EDIT", L"55555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x2_offset, 775, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgNewPosZ = CreateWindowEx(0, L"EDIT", L"5555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x2_offset, 800, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgNewPosHeading = CreateWindowEx(0, L"EDIT", L"5555", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x2_offset, 825, 100, 25, hwnd, NULL, hModule, NULL);
    hGoButton = CreateWindowEx(0, L"button", L"Go Pos", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 140 + x2_offset, 850, 100, 25, hwnd, (HMENU)GO_POS, hModule, NULL);

    int x3_offset = x2_offset + 120;
    //New position labels
    HWND hLabelEntOff = CreateWindowEx(0, L"STATIC", L"Entity Offsets", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x3_offset, 730, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelPOID = CreateWindowEx(0, L"STATIC", L"Player OID: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x3_offset, 750, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelNPCOID = CreateWindowEx(0, L"STATIC", L"NPC OID: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x3_offset, 775, 100, 25, hwnd, NULL, hModule, NULL);
    HWND hLabelEntList = CreateWindowEx(0, L"STATIC", L"EntityList: ", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x3_offset, 800, 100, 25, hwnd, NULL, hModule, NULL);
    hEntButton = CreateWindowEx(0, L"button", L"Show", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 140 + x3_offset, 825, 100, 25, hwnd, (HMENU)ENT_LIST, hModule, NULL);

    int x4_offset = x3_offset + 100;
    //New position dialogs
    hDlgPlyrOid = CreateWindowEx(0, L"EDIT", L"12345", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x4_offset, 750, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgNPCOid = CreateWindowEx(0, L"EDIT", L"1111", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 140 + x4_offset, 775, 100, 25, hwnd, NULL, hModule, NULL);

    int x5_offset = x4_offset + 110;
    hPlyrOffset = CreateWindowEx(0, L"button", L"Get", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 140 + x5_offset, 750, 100, 25, hwnd, (HMENU)PLYR_OFFSET, hModule, NULL);
    hNPCOffset = CreateWindowEx(0, L"button", L"Get", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 140 + x5_offset, 775, 100, 25, hwnd, (HMENU)NPC_OFFSET, hModule, NULL);

    hLogSend = CreateWindowEx(0, L"button", L"Log Send", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 110, 705, 100, 25, hwnd, (HMENU)LOG_SEND, hModule, NULL);
    hLogRecv = CreateWindowEx(0, L"button", L"Log Recv", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 210, 705, 100, 25, hwnd, (HMENU)LOG_RECV, hModule, NULL);
    hFilterItems = CreateWindowEx(0, L"button", L"Filter Items", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 310, 705, 100, 25, hwnd, (HMENU)FILTER_ITEMS, hModule, NULL);
    hRunSpeedCheck = CreateWindowEx(0, L"button", L"Run Speed", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 410, 705, 100, 25, hwnd, (HMENU)TOGGLE_RUNSPEED, hModule, NULL);
    hRunSpeedValues = CreateWindow(WC_COMBOBOX, TEXT(""), CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 510, 705, 100, 250, hwnd, (HMENU)CMB_SPEED, hModule, NULL);

    //Use Skill
    HWND hLabelSkillId = CreateWindowEx(0, L"STATIC", L"SkillSlot", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 850, 100, 25, hwnd, NULL, hModule, NULL);
    hDlgSkillId = CreateWindowEx(0, L"EDIT", L"12345", WS_CHILD | WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 875, 100, 25, hwnd, NULL, hModule, NULL);
    hUseSkillButton = CreateWindowEx(0, L"button", L"Use Skill", WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | BS_DEFPUSHBUTTON, 120, 900, 100, 30, hwnd, (HMENU)USE_SKILL, hModule, NULL);

    //Fill the combobox with run speed values
    TCHAR runSpeeds[4][10] =
    {
        TEXT(" "), TEXT("caster"), TEXT("between"), TEXT("bard")
    };

    TCHAR A[16];
    int  k = 0;

    memset(&A, 0, sizeof(A));
    for (k = 0; k < 4; k += 1)
    {
        wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)runSpeeds[k]);

        // Add string to combobox.
        SendMessage(hRunSpeedValues, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
    }

    // Send the CB_SETCURSEL message to display an initial item 
    //  in the selection field  
    SendMessage(hRunSpeedValues, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd); // redraw window;


    while (GetMessage(&messages, NULL, 0, 0)) {
        if (GetAsyncKeyState(VK_RCONTROL) & 1) {
            break;
        }
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }


    //exit:

    //delete sendHook;
    //delete recvHook;
    delete runSpeedHook;
    delete entityLoopHook;
    delete entityNamesHook;


#ifdef _DEBUG
    if (f != 0) {
        fclose(f);
    }
    FreeConsole();
#endif 

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}


*/