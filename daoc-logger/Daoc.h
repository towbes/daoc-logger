#pragma once
#include <set>
#include <queue>

typedef void(__cdecl* _SendPacket)(char* packetBuffer, DWORD packetHeader, DWORD packetLen, DWORD unknown);
_SendPacket Send;// = (_SendPacket)0x4281df;

typedef void(__cdecl* _SellRequest)(int slotNum);
_SellRequest SellRequest;// = (_SendPacket)0x4281df;


wchar_t moduleName[] = L"game.dll";

//Module base address
uintptr_t moduleBase;

struct send_packet {
    std::vector<char> packetBuffer;
    DWORD packetHeader;
    DWORD packetLen;
    DWORD unknown;
};

std::queue<std::shared_ptr<send_packet>> sendQueue;

//Send hook variables
//size_t sendFuncOffset = 0x281DF;
int sendHookLen = 8;
const char* internalSendPattern = "\x55\x8B\xEC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\x3D\x00\x82\x99\x00\x00\x0F\x85";
const char* internalSendMask = "xxxx????x????xxxxxx?xx";
//Used for pointer to send buffer
uintptr_t sentBuffPtr;
//Variables for storing logged packets
DWORD sentLen;
DWORD packetHeader;
DWORD packetUnknown;
char* sentBuffer;
//Flag for actions to take on packets
bool logSentHook = false;
//Function in dllmain
void printSendBufferToLog();

//Receive hook variables
//size_t recvFuncOffset = 0x27F5E;
int recvHookLen = 8;
const char* internalRecvPattern = "\x59\x59\x66\x00\x00\x00\x00\x00\x00\x00\x00\x00\x9E";
const char* internalRecvMask = "xxx?????????x";
DWORD recvLen;
unsigned char* recvBuffer;
//Used for pointer to the recv buffer
uintptr_t receiveBuffer;
//Flags for actions to take on packets
bool logRecvHook = false;
//Function in dllmain
void printRecvBufferToLog();


//Hook right after run speed function, replace EAX with desired run speed
//func loc is 438db7 on 7/4/2022
//Player position pointer is at [ebx - 0x2C] from this instruction
int runSpeedHookLen = 8;
const char* runSpeedPattern = "\x8B\xD0\x89\x55\x00\xDB\x45\x00\x59\x59\x8B\x0D\x00\x00\x00\x00\xD8\x51\x00\xDF\xE0\xF6\xC4\x00\x7A";
const char* runSpeedMask = "xxxx?xx?xxxx????xx?xxxx?x";
DWORD newRunspeed;
bool changeRunSpeed = false;

//autorun pattern
const char* autorunPattern = "\x39\x3D\x00\x00\x00\x00\x75\x00\x39\x3D\x00\x00\x00\x00\x75";
const char* autorunMask = "xx????x?xx????x";
BYTE autorunToggle;

//Sell Request pattern
const char* sellRequestPattern = "\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x00\x82\x99\x00\x00\x75\x00\x56\x8B\x35\x00\x00\x00\x00\xD9\x06\xE8\x00\x00\x00\x00\xD9\x46\x00\x89\x45\x00\xE8\x00\x00\x00\x00\x89\x45\x00\x6A\x00\x58\xE8\x00\x00\x00\x00\x66\x89\x00\x00\x66\x8B\x00\x00\x8D\x75\x00\x66\x89\x00\x00\xE8\x00\x00\x00\x00\x6A\x00\x6A\x00\x8B\xC6\x6A";
const char* sellRequestMask = "xxxxx?xxxxxx?x?xxx????xxx????xx?xx?x????xx?x?xx????xx??xx??xx?xx??x????x?x?xxx";

struct playerpos_t {
    float pos_x;
    int heading;
    unsigned char unknown[68];
    float pos_y;
    unsigned char unknown2[8];
    char unknown3;
    uint16_t pos_z;
    char unknown4;
};

DWORD playerPositionPtr;
playerpos_t* playerPosition;


//Packet item filter
void p_filterItems();
//Flags for actions to take on packets
bool filterItemFlag = false;


//Memory inventory reader
int m_readItemId(int slotNum);
//Memory based clean inventory
void m_cleanInventory();

//Send Hook
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

//Receive Hook
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


//Run speed hook function
//assigns new run speed to eax register
DWORD jmpBackAddrRunSpeed;
void __declspec(naked) runSpeedHookFunc() {
    __asm {
        pushad
        //get the player position ptr out of ebx-0x2c
        //Have to move forward at least once after loading to trigger this load
        mov eax, [ebx - 0x2c]
        mov playerPositionPtr, eax
    }

    //this causes
    playerPosition = (playerpos_t*)playerPositionPtr;

    if (changeRunSpeed) {
        //new runspeed is set in dllmain.cpp
        __asm {
            popad
            //move run speed into eax
            mov eax, newRunspeed
            //instructions we overwrote
            mov edx, eax
            mov[ebp - 0x10], edx
            fild dword ptr[ebp - 0x10]
            jmp[jmpBackAddrRunSpeed]
        }
    }
    else {
        __asm {
            popad
            //instructions we overwrote
            mov edx, eax
            mov[ebp - 0x10], edx
            fild dword ptr[ebp - 0x10]
            jmp[jmpBackAddrRunSpeed]
        }
    }
}


//item structure starts at 0xf9b8d0
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