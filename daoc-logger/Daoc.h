#pragma once
//typedef void (__thiscall* InternalSend)(void* thisClass, const char* data, DWORD length);
//InternalSend Send;
void* thisPTR;
wchar_t moduleName[] = L"game.dll";
//size_t sendFuncOffset = 0x1045270;
//size_t toHookSend = 1;
//int sendHookLen = 5;
size_t recvFuncOffset = 0x27F5E;
int recvHookLen = 6;
//DWORD sentLen;
//char* sentBuffer;
DWORD recvLen;
char* recvBuffer;
char* tmpBuffer;

//const char* internalSendPattern = "\x55\x8B\xEC\x53\x8B\xD9\x83\x7B\x0C\x00\x74\x54\x8B\x8B\x1C\x00\x02\x00\x85\xC9\x74\x2E\x8B\x01\x8B\x01\x8B\x40\x18\xFF\xD0";
//const char* internalSendMask = "xxxxxxxxxx??xx????xxxxxxx";

//const char* internalRecvPattern = "\x8B\xCE\x52\xFF\75\xFC\xFF\x50\x10\x85\xDB\x75\x8D\x75\x8D\x5F\x5E\x5B\x8B\xE5";
//const char* internalRecvMask = "xxxxxxxxxxxx???xxxxx";

//bool logSentHook = false;
bool logRecvHook = false;

//void* teax;
//void* tebx;
//void* tecx;
//void* tedx;
//void* tesi;
//void* tedi;
//void* tebp;
//void* tesp;


void* reax;
void* rebx;
void* recx;
void* redx;
void* resi;
void* redi;
void* rebp;
void* resp;


//void printSendBufferToLog();
void printRecvBufferToLog();

//DWORD jmpBackAddrSend;
//void __declspec(naked) sendHookFunc() {
//    __asm {
//        mov thisPTR, ecx
//        mov teax, eax; backup
//        mov tebx, ebx
//        mov tecx, ecx
//        mov tedx, edx
//        mov tesi, esi
//        mov tedi, edi
//        mov tebp, ebp
//        mov tesp, esp
//        mov eax, [esp + 0x8]
//        mov sentBuffer, eax
//        mov eax, [esp + 0xC]
//        mov sentLen, eax
//    }
//    if (logSentHook) {
//        printSendBufferToLog();
//    }
//    __asm{
//        mov eax, teax
//        mov ebx, tebx
//        mov ecx, tecx
//        mov edx, tedx
//        mov esi, tesi
//        mov edi, tedi
//        mov ebp, tebp
//        mov esp, tesp; end of restore
//        mov ebp, esp
//        push ebx
//        mov ebx, ecx
//        jmp[jmpBackAddrSend]
//    }
//}


DWORD jmpBackAddrRecv;
void __declspec(naked) recvHookFunc() {
    __asm {
        //mov rebx, ebx
        //mov recx, ecx
        //mov redx, edx
        //mov resi, esi
        //mov redi, edi
        //mov rebp, ebp
        //mov resp, esp
        //mov eax, [esp + 0xC]
        //mov recvBuffer, eax
        //mov recvLen, edx
        mov reax, eax
        mov rebx, ebx
        mov recx, ecx
        mov redx, edx
        mov resi, esi
        mov redi, edi
        mov rebp, ebp
        mov resp, esp
        pop ecx
        pop ebx
        mov recvBuffer, ecx
        mov recvLen, ebx
    }
    if (logRecvHook) {
        printRecvBufferToLog();
    }
    __asm {
        mov eax, reax
        mov ebx, rebx
        mov ecx, recx
        mov edx, redx
        mov esi, resi
        mov edi, redi
        mov ebp, rebp
        mov esp, resp;
        mov ecx, esi
        push edx
        push[ebp - 04]
        jmp[jmpBackAddrRecv]
    }
}

//inventory
struct item_t {
    int itemId;
    char unknown[72];
    char itemName[80];
    char unknown2[312];
};


void readSlot(int slotNum)
{
    
    size_t offset = (slotNum - 40) * sizeof(item_t);
    uintptr_t* ValLoc = (uintptr_t*)(0xf9b8d0 + offset);

    item_t slot40 = (item_t&)*ValLoc;

    printf("Slot40loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc, slot40.itemId, slot40.itemId, slot40.itemName);
    
    /*
    uintptr_t* ValLoc2 = (uintptr_t*)(0xf9b8d0 + sizeof(item_t));
    item_t slot41 = (item_t&)*ValLoc2;
    printf("Slot41loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc2, slot41.itemId, slot41.itemId, slot41.itemName);
    */
}