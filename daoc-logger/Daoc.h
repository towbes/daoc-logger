#pragma once
#include <set>
#include <queue>

typedef void(__cdecl* _SendPacket)(char* packetBuffer, DWORD packetHeader, DWORD packetLen, DWORD unknown);
_SendPacket Send;// = (_SendPacket)0x4281df;

typedef void(__cdecl* _SellRequest)(int slotNum);
_SellRequest SellRequest;// = (_SendPacket)0x42b2e3;

struct buff_t {
    unsigned char name[64];
    int unknown1;
    int buffTimeRemaining; //milliseconds
    int slotNumber;
    int tooltipId;
    unsigned char unknown2[20];
    int buffId;
    unsigned char unknown3[64];
};

buff_t plyrBuffTable[75];

//Location of Player buffs table
//Address of signature = game.dll + 0x0001FB26
// buff_t[75]
const char* plyrBuffTablePattern = "\xBF\x00\x00\x00\x00\xC7\x45\xF8";
const char* plyrBuffTableMask = "x????xxx";
void* plyrBuffTableTemp;
DWORD plyrBuffTableLoc;
unsigned char* plyrBuffTablePtr;


//hook for when to copy the player buffs table
//Address of signature = game.dll + 0x0001CDB5
int plyrBuffHookLen = 6;
const char* plyrBuffHookPattern = "\x5E\x5B\x80\x7A\x02";
const char* plyrBuffHookMask = "xxxxx";
DWORD jmpBackAddrPlyrBuff;

//Player buff table hook
#define cmp_edx __asm _emit 0x80 __asm _emit 0x7A __asm _emit 0x02 __asm _emit 0x00
void __declspec(naked) plyrBuffHookFunc() {
    __asm {
        pushad
        pushf
    }

    //copy the buff table
    plyrBuffTablePtr = reinterpret_cast<unsigned char*>(*(int*)(plyrBuffTableLoc));
    for (int i = 0; i < 75; i++) {
        plyrBuffTable[i] = *(buff_t*)(plyrBuffTablePtr);
        plyrBuffTablePtr += sizeof(buff_t);
    }

    __asm {
        popf
        popad
        //instructions we overwrote
        pop esi
        pop ebx
        cmp_edx
        jmp[jmpBackAddrPlyrBuff]
    }
}

//Each entity is 0x19B8 long?
//Total length of entity list (0x19b8 * 2000): 0xC8ED80
//Calling this function seems to crash due to race condition sometimes
typedef uintptr_t(__fastcall* _GetEntityPointer)(int entityOffset);
_GetEntityPointer GetEntityPointer = (_GetEntityPointer)0x43589f;

//Sanity checker before calling GetEntityPointer
typedef char(__fastcall* _EntityPtrSanityCheck)(int entityListOffset);
_EntityPtrSanityCheck EntityPtrSanityCheck = (_EntityPtrSanityCheck)0x4358bf;
int preCheck = *(int*)0xaa4c5c;
int entityListOffset;

//This is for npc / objects only
//Calling this function seems to crash due to race conditions sometimes
typedef int(__cdecl* _getNPCEntityOffsetFromOid)(int objectId);
_getNPCEntityOffsetFromOid GetNPCEntityOffsetFromOid = (_getNPCEntityOffsetFromOid)0x411721;

//This function is for players: 0x4116ba
//Calling this function seems to crash due to race condition sometimes
typedef int(__cdecl* _getPlyrEntityOffsetFromListIndex)(int sessionId);
_getPlyrEntityOffsetFromListIndex GetPlyrEntityOffsetFromListIndex = (_getPlyrEntityOffsetFromListIndex)0x4116ba;

//Mid fun Hook at 0x411e2f - Entity Pointer in EAX, player list index in ESI
//Address of signature = game.dll + 0x00011E2F
const char* plyrEntityLoopPattern = "\xFF\x75\x00\x8B\xF0\xE8\x00\x00\x00\x00\x89\x45";
const char* plyrEntityLoopMask = "xx?xxx????xx";
int plyrEntityHookLen = 5;

//Entity global variables/lists
int objectId;
uintptr_t ptrEntityChar;
DWORD jmpBackAddrEntity;

uintptr_t EntityList[2000] = {0};
//This list starts at index = 2
int playerEntityList[1003];
uintptr_t tempAddress;
int entityOffset;
int npcEntityListOffset;
int plyrEntityListOffset;


//EntityUpdate Loop hook
//Address of signature = game.dll + 0x00017DD6
//0x417dd6
const char* entityLoopPattern = "\x3B\x3D\x00\x00\x00\x00\x0F\x8C\x00\x00\x00\x00\x5F";
const char* entityLoopMask = "xx????xx????x";
int entityLoopHookLen = 6;

bool findEntityByOffset(int offset) {
    for (int i = 0; i < 2000; i++) {
        if ((int)EntityList[i] == 0) {
            return false;
        }
        if (offset == (int)EntityList[i]) {
            unsigned char* tempPtr = reinterpret_cast<unsigned char*>(EntityList[i]);
            tempPtr += 0x23c;
            std::cout << "Player Offset " << offset << ": " << std::hex << (uintptr_t)EntityList[i] << " ObjectID: " << *(uint16_t*)tempPtr << std::endl;
            return true;
        }
    }
    return false;
}

bool findEntityByOid(short oid) {
    for (int i = 0; i < 2000; i++) {
        if ((int)EntityList[i] == 0) {
            std::cout << "Not found" << std::endl;
            return false;
        }
        unsigned char* tempPtr = reinterpret_cast<unsigned char*>(EntityList[i]);
        tempPtr += 0x23c;
        if (oid == *(uint16_t*)tempPtr) {

            std::cout << "Entity Offset: " << std::hex << (uintptr_t)EntityList[i] << " ObjectID: " << *(uint16_t*)tempPtr << std::endl;
            return true;
        }
    }
    std::cout << "Not found" << std::endl;
    return false;
}

//EntityUpdateLoopHook
#define cmp_edi __asm _emit 0x3B __asm _emit 0x3D __asm _emit 0x5C __asm _emit 0x4C __asm _emit 0xAA __asm _emit 0x00
void __declspec(naked) entityLoopFunc() {
    __asm {
        pushad
        pushf
        mov tempAddress, esi
        mov entityOffset, edi
    }

    //subtract one because hook is right after the inc instruction
    entityOffset -= 1;

    if (entityOffset == 0) {
        memset(EntityList, 0, sizeof(EntityList));
    }

    if (entityOffset > -1 && entityOffset < 2000) {
        EntityList[(int)entityOffset] = tempAddress;
    }

    tempAddress = 0;
    __asm {
        popf
        popad
        //instructions we overwrote
        cmp_edi
        jmp[jmpBackAddrEntity]
    }
}



wchar_t moduleName[] = L"game.dll";

//server name @ 0xc4a6a8

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

DWORD newRunspeed = 0xEE;
bool changeRunSpeed = false;

//Player position struct pointer
//Address of signature = game.dll + 0x000418CD , address at +0x2 offset
const char* ptrPlayerPositionPattern = "\x89\x3D\x00\x00\x00\x00\xF3";
const char* ptrPlayerPositionMask = "xx????x";

//autorun pattern
const char* autorunPattern = "\x39\x3D\x00\x00\x00\x00\x75\x00\x39\x3D\x00\x00\x00\x00\x75";
const char* autorunMask = "xx????x?xx????x";
BYTE autorunToggle;



//Sell Request pattern
const char* sellRequestPattern = "\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x00\x82\x99\x00\x00\x75\x00\x56\x8B\x35\x00\x00\x00\x00\xD9\x06\xE8\x00\x00\x00\x00\xD9\x46\x00\x89\x45\x00\xE8\x00\x00\x00\x00\x89\x45\x00\x6A\x00\x58\xE8\x00\x00\x00\x00\x66\x89\x00\x00\x66\x8B\x00\x00\x8D\x75\x00\x66\x89\x00\x00\xE8\x00\x00\x00\x00\x6A\x00\x6A\x00\x8B\xC6\x6A";
const char* sellRequestMask = "xxxxx?xxxxxx?x?xxx????xxx????xx?xx?x????xx?x?xx????xx??xx??xx?xx??x????x?x?xxx";

struct playerpos_t {
    float pos_x;
    short heading;
    unsigned char unknown[68];
    float pos_y;
    unsigned char unknown2[8];
    float pos_z;
    char unknown4;
};

DWORD playerPositionPtr;
playerpos_t* playerPosition = NULL;


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

    receiveBuffer = moduleBase + 0xC477BA; // we probably need to read this value from the stack
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
    }


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
    //13474,	//Item Name - Ancient Troll Blood
    //13462,  //Item name - Enriched quicksilver
    //13485,  //Item name - Ancient necrotic brain fluid
    //13466,	//Item Name - Ancient Mirror
    //13470,	//Item Name - Ancient Pure Mercury
    //13483,	//Item Name - Ancient Lich Tooth
    //13478,	//Item Name - Ancient Giant Blood
    //13482,  //ItemName - Ancient Crushed Focus Stone
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
        //printf("Incoming Item to slot %u - ItemHex: %02x, ItemId: %u, Item Name - ", slotId, itemId, itemId);
        //print the chars
        for (unsigned int i = 32; i < recvLen; i++) {
            std::cout << recvBuffer[i];
        }
        printf("\n");
        Sleep(50);
        if (badItemIdList.find(itemId) != badItemIdList.end()) {
        //if (goodItemIdList.find(itemId) == goodItemIdList.end() && m_readItemId(slotId) == itemId) {
            printf("Dropping slot %u - ItemHex: %02x, ItemId: %u, Item Name - ", slotId, itemId, itemId);
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