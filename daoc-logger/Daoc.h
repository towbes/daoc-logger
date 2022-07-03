#pragma once
#include <set>

//typedef void (__thiscall* InternalSend)(void* thisClass, const char* , const char* data, DWORD length);
//InternalSend Send;
//void* thisPTR;

typedef void(__cdecl* _SendPacket)(char* packetBuffer, DWORD packetHeader, DWORD packetLen, DWORD unknown);
_SendPacket Send = (_SendPacket)0x4281df;

wchar_t moduleName[] = L"game.dll";
size_t sendFuncOffset = 0x281DF;

int sendHookLen = 8;
size_t recvFuncOffset = 0x27F5E;
int recvHookLen = 8;
DWORD sentLen;
DWORD packetHeader;
DWORD packetUnknown;
char* sentBuffer;
DWORD recvLen;
unsigned char* recvBuffer;

uintptr_t moduleBase;

//Used for pointer to the recv buffer
uintptr_t receiveBuffer;

//Used for pointer to send buffer
uintptr_t sentBuffPtr;

//const char* internalSendPattern = "\x55\x8B\xEC\x53\x8B\xD9\x83\x7B\x0C\x00\x74\x54\x8B\x8B\x1C\x00\x02\x00\x85\xC9\x74\x2E\x8B\x01\x8B\x01\x8B\x40\x18\xFF\xD0";
//const char* internalSendMask = "xxxxxxxxxx??xx????xxxxxxx";

//const char* internalRecvPattern = "\x8B\xCE\x52\xFF\75\xFC\xFF\x50\x10\x85\xDB\x75\x8D\x75\x8D\x5F\x5E\x5B\x8B\xE5";
//const char* internalRecvMask = "xxxxxxxxxxxx???xxxxx";

bool logSentHook = false;
bool logRecvHook = false;

bool filterItemFlag = false;

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


void printSendBufferToLog();
void printRecvBufferToLog();

//Packet item filter
void p_filterItems();
//Memory inventory reader
int m_readItemId(int slotNum);
//Memory based clean inventory
void m_cleanInventory();


DWORD jmpBackAddrSend;
void __declspec(naked) sendHookFunc() {
    __asm {
        pushad
        //adds 32 bytes to the stack addresses
        mov eax, [esp + 0x24]
        mov sentBuffPtr, eax
        mov eax, [esp + 0x28]
        mov packetHeader, eax
        mov eax, [esp + 0x2C]
        mov sentLen, eax
        mov eax, [esp + 0x30]
        mov packetUnknown, eax
    }

    sentBuffer = (char*)sentBuffPtr;

    if (logSentHook) {
        printSendBufferToLog();
    }
    __asm {
        popad
        //instructions we overwrote
        //55
        push ebp
        //8B EC
        mov ebp, esp
        //B8 20 11 00 00 
        mov eax, 0x1120
        jmp[jmpBackAddrSend]
    }
}

//https://docs.microsoft.com/en-us/cpp/assembler/inline/emit-pseudoinstruction?view=msvc-170
#define mv_ax __asm _emit 0x66 __asm _emit 0xA1 __asm _emit 0xB8 __asm _emit 0x77 __asm _emit 0x04 __asm _emit 0x01

DWORD jmpBackAddrRecv;
void __declspec(naked) recvHookFunc() {
    __asm {
        pushad
        mov recvLen, ebx
        //mov recvBuffer, ebx
    }

    receiveBuffer = moduleBase + 0xC477BA;
    recvBuffer = (unsigned char*)receiveBuffer;

    if (logRecvHook) {
        printRecvBufferToLog();
    }
    if (filterItemFlag) {
        p_filterItems();
    }
    __asm {
        popad
        pop ecx
        pop ecx
        mv_ax
        jmp[jmpBackAddrRecv]
    }
}


//inventory
struct item_t {
    int itemId;
    unsigned char unknown[72];
    unsigned char itemName[80];
    unsigned char unknown2[312];
};


//Move item Function
//Slot reference: https://github.com/Dawn-of-Light/DOLSharp/blob/9af87af011497c3fda852559b01a269c889b162e/GameServer/gameutils/IGameInventory.cs
//Ground = 1
//Backpack = 40-79
//count = 0 for non stacks
typedef void (__cdecl* _MoveItem)(int toSlot, int fromSlot, int count);
_MoveItem MoveItem = (_MoveItem)0x42a976;


/*
incoming inventory packet [0x02]
//[RECV]
02 02 00 00 00 FF 00 2A D0 6F 01 00 00 00 29 00 00 00 64 00 00 00 00 00 74 01 00 00 08 1F AA 17 47 61 74 65 77 61 79 20 2D 20 50 65 72 73 6F 6E 61 6C 20 42 69 6E 64 00 00 1A 50 65 72 73 6F 6E 61 6C 20 42 69 6E 64 20 52 65 63 61 6C 6C 20 53 74 6F 6E 65 28

recvBuffer[0] = 0x02 (packet code)
recvBuffer[7] = 0x2a (destination slot number)
recvBuffer[8] = int (0xD0 0x6F) (itemid)
recvBuffer[14]? = 0x29 (source slot number)
recvBuffer[32] = Start of string for item name
*/

std::set<int> badItemIdList{ 
    13474,	//Item Name - Ancient Troll Blood
    13462,  //Item name - Enriched quicksilver
    13485,  //Item name - Ancient necrotic brain fluid
    13466,	//Item Name - Ancient Mirror
    13470,	//Item Name - Ancient Pure Mercury
    13483,	//Item Name - Ancient Lich Tooth
    13478,	//Item Name - Ancient Giant Blood
    13482,  //ItemName - Ancient Crushed Focus Stone
    59060,	//Item Name - Perfected Armor Pattern
    6267,	//Item Name - uminescent Exeregum Stone
    6266,	//Item Name - Luminescent Exerpise Stone
    63681,	//Item Name - Ring of Arcane Gestures
    63677,	//Item Name - Ensorcelled Staff of Severity
    63671,	//Item Name - Bloodstained War Hammer
    12175,	//Item Name - Opposition Pin
    16880,	////Item Name - Golestandt's Remains
    12176,	////Item Name - Belt of the Protector
    63670,	////Item Name - Fine Steel Long Sword
    63678,	////Item Name - Golestandt's Fire
    12179,	////Item Name - Ring of Granite Enhancement
    63686,	////Item Name - Lament of Dartmoor
    63664,	////Item Name - Fallen Soldier's Rage
    12177,	////Item Name - Band of Ilmenite
    63690,	////Item Name - Charred Dragonscale Chain Boots
    63682,	////Item Name - Ancient Indigo Mail
    12178,	////Item Name - Band of Ircon
    63675,	////Item Name - Timeless Indigo Mail
    63666,	////Item Name - Dragonscale Buckler
    63684,	////Item Name - Staff of Flamestrike
    63667,	////Item Name - Defender of Scorched Dreams
    63672,	////Item Name - Virulent Soul Sapper
    63687,	////Item Name - Searing War Hammer
    63679,	////Item Name - Archaic Assassin's Vest
    63688,	////Item Name - Golestandt's Tooth
    63661,	////Item Name - Staff of Perfect Incantation
    63683,	////Item Name - Flamecrusher Gauntlets
    12171,	////Item Name - Magma Imbued Cloak
    63673,	////Item Name - Robes of the Magus
    63685,	////Item Name - Dragon Knight's Flame
    16037,	//Item Name - Ring of the Undaunted Champion
    26282,	//Item Name - Life Stone
    63680,	//Item Name - Cloth Cap
    63689,	//Item Name - Soot Covered Vest
    63662,	//Item Name - Dark Knight's Fury
    63674,	//Item Name - Ageless Fluted Protector
    63669,	//Item Name - Ring of Arcane Strength
    63665,	//Item Name - Diamond Spiked Vest
    63668,  //Item Name - Dragonflame Protecto
    63676,  //Item Name - Ensorcelled Robes of Severit
    12173,  //Item Name - Band of Eldsp
    22391,  //Item Name - Staff of the Lifeblinde
    12174,  //ItemName - Belt of Granite Enhancement
    12180,  //ItemName - Stonewatch Bracer
    12172,  //ItemName - Stonewatch Pin
};

std::set<int> goodItemIdList{ };

void p_filterItems() 
{
    //if it's an incoming item packet
    if (recvBuffer[0] == '\x02' && recvLen > 10) {
        int slotId = recvBuffer[7];
        int itemId = recvBuffer[8] << 8 | recvBuffer[9];
        //m_readSlot(slotId);
        printf("Incoming Item to slot %u - ItemHex: %02x, ItemId: %u, Item Name - ", slotId, itemId, itemId);
        //print the chars
        for (unsigned int i = 32; i < recvLen; i++) {
            std::cout << recvBuffer[i];
        }
        printf("\n");
        if (badItemIdList.find(itemId) != badItemIdList.end() && m_readItemId(slotId) == itemId) {
        //if (goodItemIdList.find(itemId) == goodItemIdList.end() && m_readItemId(slotId) == itemId) {
            MoveItem(1, slotId, 0);
        }
    }
}

//Read item slot information from memory
void m_cleanInventory()
{
    //look through all slots
    for (int slotNum = 40; slotNum < 80; slotNum++) {
        size_t offset = (slotNum - 40) * sizeof(item_t); //* sizeof(item_t);
        uintptr_t* ValLoc2 = (uintptr_t*)(0xf9b8d0 + offset);
        item_t itemTemp = (item_t&)*ValLoc2;
        printf("Slot %u, ItemHex: %02x, ItemId - %u, ItemName - %s\n", slotNum, itemTemp.itemId, itemTemp.itemId, itemTemp.itemName);
        if (badItemIdList.find(itemTemp.itemId) != badItemIdList.end() && m_readItemId(slotNum) == itemTemp.itemId) {
            MoveItem(1, slotNum, 0);
        }
        Sleep(10);
    }


    //printf("Slot40loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc, slot40.itemId, slot40.itemId, slot40.itemName);

    /*
    uintptr_t* ValLoc2 = (uintptr_t*)(0xf9b8d0 + sizeof(item_t));
    item_t slot41 = (item_t&)*ValLoc2;
    printf("Slot41loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc2, slot41.itemId, slot41.itemId, slot41.itemName);
    */
}

//Read item slot information from memory
int m_readItemId(int slotNum)
{
    
    size_t offset = (slotNum - 40) * 0x1D4; //* sizeof(item_t);
    uintptr_t* ValLoc = (uintptr_t*)(0xf9b8d0 + offset);

    return (int&)*ValLoc;

    //printf("Slot40loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc, slot40.itemId, slot40.itemId, slot40.itemName);
    
    /*
    uintptr_t* ValLoc2 = (uintptr_t*)(0xf9b8d0 + sizeof(item_t));
    item_t slot41 = (item_t&)*ValLoc2;
    printf("Slot41loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc2, slot41.itemId, slot41.itemId, slot41.itemName);
    */
}