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

#pragma once
#include <set>
#include <queue>
#include <map>


bool g_ShowDemo = false;
bool g_showMenu = false;
bool useItemFilter = false;

wchar_t moduleName[] = L"game.dll";

//server name @ 0xc4a6a8

//Module base address
uintptr_t moduleBase;

//The pointer scans probably need to use ?? for each byte of the wildcards but below are using just single ?

//D3D9 pointers
//pointer1 will point to the vtable for IDirect3D9* and pointer2 to IDirect3DDevice9*
//const auto pointer1 = daochook::pointers::instance().add("d3d9", "d3d9.dll", "C703????????89432C418983", 0, 0);
//const auto pointer2 = daochook::pointers::instance().add("d3d9_device", "d3d9.dll", "C706????????8986????????8986", 0, 0);

//pointer mouse or key pressed
//Address of signature = game.dll + 0x00043FB0   0x104a744
//"\x89\x35\x00\x00\x00\x00\x5F\x5E\x5B\xC9\xC3\x8B\xFF\xF0\x3D", "xx????xxxxxxxxx"
//"89 35 ? ? ? ? 5F 5E 5B C9 C3 8B FF F0 3D"


//uint32_t = [game.dll + 0xC4A7F0] + 0x64 = Local Player Entity Index
////Player ent index is at +0x64 from address in ptrPlayerEntIndex
uintptr_t ptrPlayerEntIndex; // 0x104a7f0
//Pattern
//Address of signature = game.dll + 0x00041880 0x441880 @ +0x2
const char* ptrPlayerEntIndexPattern = "\x89\x3D\x00\x00\x00\x00\xB9";
const char* ptrPlayerEntIndexMask = "xx????x";
//const char* ptrPlayerPatternNew = "893D????B9";

//Ptr to players pet entity index
//#define ptrPetEntIndex_x	0x10498d0

enum EntStates {
    petAutoAtk = 4,
    idle = 8,
    petStyle = 69,
};

typedef void(__cdecl* _SellRequest)(int slotNum);
_SellRequest SellRequest;// = (_SellRequest)0x42b2e3;

//Sell Request pattern
const char* sellRequestPattern = "\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x00\x82\x99\x00\x00\x75\x00\x56\x8B\x35\x00\x00\x00\x00\xD9\x06\xE8\x00\x00\x00\x00\xD9\x46\x00\x89\x45\x00\xE8\x00\x00\x00\x00\x89\x45\x00\x6A\x00\x58\xE8\x00\x00\x00\x00\x66\x89\x00\x00\x66\x8B\x00\x00\x8D\x75\x00\x66\x89\x00\x00\xE8\x00\x00\x00\x00\x6A\x00\x6A\x00\x8B\xC6\x6A";
const char* sellRequestMask = "xxxxx?xxxxxx?x?xxx????xxx????xx?xx?x????xx?x?xx????xx??xx??xx?xx??x????x?x?xxx";

//Use skill function
typedef void(__cdecl* _UseSkill)(int skillSlot, int hasSkillFlag);
_UseSkill UseSkill;// = (_UseSkill)0x42b5d5;

//Address of signature = game.dll + 0x0002B5D5
const char* funcUseSkillPattern = "\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x00\x82\x99\x00\x00\x0F\x85\x00\x00\x00\x00\xD9\x05\x00\x00\x00\x00\x57\x6A\x00\x33\xC0\x59\x8D\x7D\x00\xF3\x00\x8B\x0D\x00\x00\x00\x00\x5F\xD9\x41\x00\xDA\xE9\xDF\xE0\xF6\xC4\x00\x7B\x00\x80\x4D\xFB\x00\x53\x33\xDB\x38\x59\x00\x56\x74\x00\xD9\x05\x00\x00\x00\x00\xD9\x41\x00\xDA\xE9\xDF\xE0\xF6\xC4\x00\x7B\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x53\xE8\x00\x00\x00\x00\x84\xC0\x59\x74\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x3B\xC3\x75\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x53\xE8\x00\x00\x00\x00\x84\xC0\x59\x74\x00\x80\x4D\xFB\x00\xD9\x05\x00\x00\x00\x00\xD9\x05\x00\x00\x00\x00\xDA\xE9\xDF\xE0\xF6\xC4\x00\x7B\x00\xE8\x00\x00\x00\x00\x84\xC0\x74\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x53\xE8\x00\x00\x00\x00\x84\xC0\x59\x74\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x66\x89\x00\x00\x8B\x08\x89\x4D\x00\x8B\x48\x00\x89\x4D\x00\x8B\x48\x00\x8B\x40\x00\x89\x45\x00\x8A\x45\x00\x88\x45\x00\x8A\x45\x00\x8D\x75\x00\x89\x4D\x00\x88\x45\x00\xE8\x00\x00\x00\x00\x53\x6A\x00\x8B\xC6\x68";
const char* funcUseSkillMask = "xxxxx?xxxxxx?xx????xx????xx?xxxxx?x?xx????xxx?xxxxxx?x?xxx?xxxxx?xx?xx????xx?xxxxxx?x?xxx?x????????xx????xxxx?x????????xxx?xxx?x????????xx????xxxx?xxx?xx????xx????xxxxxx?x?x????xxx?xxx?x????????xx????xxxx?xxx?x????????xx??xxxx?xx?xx?xx?xx?xx?xx?xx?xx?xx?xx?xx?x????xx?xxx";

//Player HP Address
//  int hp
//  int pow
//  int endu
//Address of signature = game.dll + 0x000B6617  at +0x1
const char* ptrPlyrHpPattern = "\xA3\x00\x00\x00\x00\x00\x00\x00\x00\x59\xF7\xF9";
const char* ptrPlyrHpMask = "x????????xxx";

void* plyrHpPtr;
int plyr_hp;
int plyr_pow;
int plyr_endu;

struct useSpell_t {
    char name[64];
    short spellLevel;
    short unknown1;
    int tickCount;
    int unknown2;
    int unknown3;
    int unknown4;
    int unknown5;
    int unknown6;
};
//array start address is 0x161d9f0
//6968 bytes total = 0x1B38
struct spellCategory_t {
    char categoryName[64];
    useSpell_t spellArray[75];
    int alignBuf;
};

// 0x161d9f0 + (0x1b38)
// it's 0x1b38 * the number of spell categories in spell window
typedef void(__cdecl* _UseSpell)(int spellCategory, int spellLevel);
_UseSpell UseSpell;

//Address of signature = game.dll + 0x0002B4B8 - 0x42b4b8
const char* funcUseSpellPattern = "\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x00\x82\x99\x00\x00\x0F\x85\x00\x00\x00\x00\xD9\x05\x00\x00\x00\x00\x57\x6A\x00\x33\xC0\x59\x8D\x7D\x00\xF3\x00\x8B\x0D\x00\x00\x00\x00\x5F\xD9\x41\x00\xDA\xE9\xDF\xE0\xF6\xC4\x00\x7B\x00\x80\x4D\xFB\x00\x53\x33\xDB\x38\x59\x00\x56\x74\x00\xD9\x05\x00\x00\x00\x00\xD9\x41\x00\xDA\xE9\xDF\xE0\xF6\xC4\x00\x7B\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x53\xE8\x00\x00\x00\x00\x84\xC0\x59\x74\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x3B\xC3\x75\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x53\xE8\x00\x00\x00\x00\x84\xC0\x59\x74\x00\x80\x4D\xFB\x00\xD9\x05\x00\x00\x00\x00\xD9\x05\x00\x00\x00\x00\xDA\xE9\xDF\xE0\xF6\xC4\x00\x7B\x00\xE8\x00\x00\x00\x00\x84\xC0\x74\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x53\xE8\x00\x00\x00\x00\x84\xC0\x59\x74\x00\x80\x4D\xFB\x00\xA1\x00\x00\x00\x00\x00\x00\x00\x00\x66\x89\x00\x00\x8B\x08\x89\x4D\x00\x8B\x48\x00\x89\x4D\x00\x8B\x48\x00\x8B\x40\x00\x89\x45\x00\x8A\x45\x00\x88\x45\x00\x8A\x45\x00\x8D\x75\x00\x89\x4D\x00\x88\x45\x00\xE8\x00\x00\x00\x00\x53\x6A\x00\x8B\xC6\x6A\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x00\x5E\x5B\xC9\xC3\x55\x8B\xEC\x83\xEC";
const char* funcUseSpellMask = "xxxxx?xxxxxx?xx????xx????xx?xxxxx?x?xx????xxx?xxxxxx?x?xxx?xxxxx?xx?xx????xx?xxxxxx?x?xxx?x????????xx????xxxx?x????????xxx?xxx?x????????xx????xxxx?xxx?xx????xx????xxxxxx?x?x????xxx?xxx?x????????xx????xxxx?xxx?x????????xx??xxxx?xx?xx?xx?xx?xx?xx?xx?xx?xx?xx?xx?x????xx?xxx?xx????xx?xxxxxxxxx";

//UseSpell list start
//Address of signature = game.dll + 0x0001FFC1 at + 0x3
const char* plyrUseSpellTablePattern = "\x0F\xBF\x00\x00\x00\x00\x00\x8B\x54\x00\x00\x89\x0A";
const char* plyrUseSpellTableMask = "xx?????xx??xx";
void* plyrUseSpellTableTemp;
DWORD plyrUseSpellTableLoc;
unsigned char* plyrUseSpellTablePtr;
//useSpell_t plyrUseSpellTable[150];

//category starts at 1
spellCategory_t plyrSpellCategories[8];

int puspCount;

//All spells.csv are loaded into memory at 0x115A430

void DumpSpells() {
    plyrUseSpellTablePtr = reinterpret_cast<unsigned char*>(*(int*)(plyrUseSpellTableLoc));
    //first spell is 64bytes behind, category array starts 64bytes behind that
    plyrUseSpellTablePtr -= 0x80;
    for (puspCount = 0; puspCount < 8; puspCount++) {
        plyrSpellCategories[puspCount] = *(spellCategory_t*)(plyrUseSpellTablePtr);
        plyrUseSpellTablePtr += sizeof(spellCategory_t);
    }
}


//Player Buffs
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
//Address of signature = game.dll + 0x0001FB26  +0x1
// buff_t[75]
const char* plyrBuffTablePattern = "\xBF\x00\x00\x00\x00\xC7\x45\xF8";
const char* plyrBuffTableMask = "x????xxx";
void* plyrBuffTableTemp;
DWORD plyrBuffTableLoc;
unsigned char* plyrBuffTablePtr;
int pbtCount;


void DumpBuffs() {
    plyrBuffTablePtr = reinterpret_cast<unsigned char*>(*(int*)(plyrBuffTableLoc));
    for (pbtCount = 0; pbtCount < 75; pbtCount++) {
        plyrBuffTable[pbtCount] = *(buff_t*)(plyrBuffTablePtr);
        plyrBuffTablePtr += sizeof(buff_t);
    }
}

//Skills
struct useSkill_t {
    unsigned char name[64];
    int unknown1;
    int unknown2;
    //This is the tickcount that spell can be used again
    int tickCount;
};

buff_t plyrSkillTable[150];
useSkill_t plyrUseSkillTable[150];

//Start of skill list pattern
//Address of signature = game.dll + 0x0001EEC8
const char* plyrSkillTablePatternNew = "BA????803A";
//At +0x1
const char* plyrSkillTablePattern = "\xBA\x00\x00\x00\x00\x80\x3A";
const char* plyrSkillTableMask = "x????xx";
void* plyrSkillTableTemp;
DWORD plyrSkillTableLoc;
unsigned char* plyrSkillTablePtr;
int pstCount;

//UseSkill list start
//Address of signature = game.dll + 0x0001EF56 +0x1
const char* plyrUseSkillTablePattern = "\xBF\x00\x00\x00\x00\xF3\x00\x89\x1D";
const char* plyrUseSkillTableMask = "x????x?xx";
void* plyrUseSkillTableTemp;
DWORD plyrUseSkillTableLoc;
unsigned char* plyrUseSkillTablePtr;
int pusCount;

void DumpSkills() {
    //plyrUseSkillTablePtr = reinterpret_cast<unsigned char*>(*(int*)(plyrUseSkillTableLoc));
    plyrUseSkillTablePtr = reinterpret_cast<unsigned char*>(plyrUseSkillTableLoc);
    for (pusCount = 0; pusCount < 150; pusCount++) {
        plyrUseSkillTable[pusCount] = *(useSkill_t*)(plyrUseSkillTablePtr);
        plyrUseSkillTablePtr += sizeof(useSkill_t);
    }
}

//Each entity is 0x19B8 long?
//Total length of entity list (0x19b8 * 2000): 0xC8ED80
//Calling this function seems to crash due to race condition sometimes
typedef uintptr_t(__fastcall* _GetEntityPointer)(int entityOffset);
_GetEntityPointer GetEntityPointer;// = (_GetEntityPointer)0x43589f;

//Address of signature = game.dll + 0x0003589F
const char* getEntityPtrPattern = "\x85\xC9\x7C\x00\x81\xF9\x00\x00\x00\x00\x7D\x00\x56\x8B\x35\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x5E";
const char* getEntityPtrMask = "xxx?xx????x?xxx????x????x";

//table_dex:eax  entity_idx:ecx, Destination:stack0x4, count:stack0x8
uintptr_t oGetEntityName;// = 0x4358ee;

//Address of signature = game.dll + 0x000358EE
const char* getEntityNamePattern = "\x55\x8B\xEC\x51\x85\xC9\x57";
const char* getEntityNameMask = "xxxxxxx";

// (c) 2022 atom0s [atom0s@live.com]
//use __stdcall to make stack setup/cleanup simpler
void __declspec(naked) __stdcall GetEntityName(int table_idx, int entity_idx, char* Destination, size_t Count) {
        
    __asm {
        //prepare stackframe
        push ebp
        mov ebp, esp

        //save the registers/flags
        pushad
        pushfd

        //Setup the call to game function GetEntityname
        push Count
        push[Destination]
        push table_idx
        mov ecx, entity_idx
        pop eax

        //call the game function
        call oGetEntityName

        //pop the two values off stack
        pop eax
        pop eax

        //restore registers/flags
        popfd
        popad

        //restore stack frame and return
        mov esp, ebp
        pop ebp
        ret 0x10

    }
}

//chat input command handler

void* oCmdHandler;
uintptr_t newCmdHandler = 0x41644D;
//Address of signature = game.dll + 0x00016444  0x416444
const char* cmdHandlerPattern = "\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\x85\xD2";
const char* cmdHandlerMask = "xxxxx????xx";
//"55 8B EC 81 EC ? ? ? ? 85 D2"

//Print to chatlog
void* oPrintChat;// = 0x4190e3;

//Address of signature = game.dll + 0x000190E3
const char* printChatPattern = "\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x81\xEC\x00\x00\x00\x00\x83\x3D\x94\xA7\x04\x01";
const char* printChatMask = "x????x????xx????xxxxxx";
//"B8 ? ? ? ? E8 ? ? ? ? 81 EC ? ? ? ? 83 3D 94 A7 04 01"


uintptr_t ptrChatiMode;
// = (_GetEntityPointer)0x43589f;
//Address of signature = game.dll + 0x0002BC08 0x42bc08
const char* sendCmdPattern = "\x83\x3D\x00\x82\x99\x00\x00\x0F\x85\x00\x00\x00\x00\x56";
const char* sendCmdMask = "xxxxxx?xx????x";
//"83 3D 00 82 99 00 ? 0F 85 ? ? ? ? 56"


//Sanity checker before calling GetEntityPointer
typedef bool(__fastcall* _EntityPtrSanityCheck)(int entityListOffset);
_EntityPtrSanityCheck EntityPtrSanityCheck;// = (_EntityPtrSanityCheck)0x4358bf;
int entityListOffset;

//Address of signature = game.dll + 0x000358BF 
const char* entityPtrCheckPattern = "\x85\xC9\x7C\x00\x81\xF9\x00\x00\x00\x00\x7D\x00\x56\x8B\x35\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x85\xC0";
const char* entityPtrCheckMask = "xxx?xx????x?xxx????x????xx";

//This is for npc / objects only
//Calling this function seems to crash due to race conditions sometimes
typedef int(__cdecl* _getNPCEntityOffsetFromOid)(int objectId);
_getNPCEntityOffsetFromOid GetNPCEntityOffsetFromOid;// = (_getNPCEntityOffsetFromOid)0x411721;

//Address of signature = game.dll + 0x00011721
const char* getEntityOffsetFromOidPattern = "\x56\x33\xF6\x39\x35\x00\x00\x00\x00\x7E\x00\x8B\xCE\xE8\x00\x00\x00\x00\x84\xC0\x74\x00\x8B\xCE\xE8\x00\x00\x00\x00\x66\x8B";
const char* getEntityOffsetFromOidMask = "xxxxx????x?xxx????xxx?xxx????xx";

//This function is for players: 0x4116ba
//Calling this function seems to crash due to race condition sometimes
typedef int(__cdecl* _getPlyrEntityOffsetFromListIndex)(int sessionId);
_getPlyrEntityOffsetFromListIndex GetPlyrEntityOffsetFromListIndex = (_getPlyrEntityOffsetFromListIndex)0x4116ba;

//Address of signature = game.dll + 0x000116BA
const char* getPlyrEntityOffsetFromPlyrListPattern = "\x57\xFF\x74\x00\x00\xE8\x00\x00\x00\x00\x8B\xF8";
const char* getPlyrEntityOffsetFromPlyrListMask = "xxx??x????xx";

//True will block the command, false will pass it through
bool ProcessCmd(const char* command, int* cmdType) {
    ::OutputDebugStringA(std::format("Type {}, Command: {}", *cmdType, command).c_str());
    if (strcmp(command, "/menu") == 0) {
        g_showMenu = !g_showMenu;
        return true;
    }
    return false;
}

//wrapper for the cmd handler hook
//ty again atom0s for help in setting this up
__declspec(naked) void __stdcall wrapCmdHandler() {
    //static char mCmd[512];
    const char* oCmd;
    int32_t oCmdType;

    //save the registers/flags;
    _asm pushad;
    _asm pushfd;
    //prologue;
    _asm push ebp;
    _asm mov ebp, esp;
    _asm sub esp, __LOCAL_SIZE;

    //getthe cmd type
    _asm push[ebp + 0x2C];
    _asm pop oCmdType;
    //get the command buffer address
    _asm mov oCmd, edx;

    // Prepare the modifiable command buffer..
    //_asm pushad;
    //_asm pushfd;
    //::strcpy_s(mCmd, c_cmd);
    //_asm popfd;
    //_asm popad;

    if (!ProcessCmd(oCmd, &oCmdType)) {
        _asm {
            //Setup the call to game function cmd handler
            //String is put into edx, type is pushed onto stack
            //mov edx, oCmd
            //push cmdType
            // Update the modified command mode..
            //_asm push eax;
            //_asm mov eax, p_mode;
            //_asm mov[ebp + 0x2C], eax;
            //_asm pop eax;


            //epilogue
            mov esp, ebp
            pop ebp
            //restore registers/flags
            popfd
            popad

            // Update the modified command..
            //_asm lea edx, [p_cmd];

            //jump to the game function
            jmp oCmdHandler
        }
    }
    else {
        _asm {
            //epilogue
            mov esp, ebp
            pop ebp
            //restore register/flags
            popfd
            popad
            ret
        }

    }

}

void grabChat(const char* buffer) {
    std::string strBuff = std::string(buffer);
    ::OutputDebugStringA(std::format("Text: {}", strBuff).c_str());
}

__declspec(naked) void __stdcall printChat() {
    const char* ptrBuff;
    //save the registers/flags;
    _asm pushad;
    _asm pushfd;
    //prologue;
    _asm push ebp;
    _asm mov ebp, esp;
    _asm sub esp, __LOCAL_SIZE;

    _asm mov ptrBuff, ebx;

    ptrBuff += 1;
    grabChat(ptrBuff);

    //epilogue
    _asm mov esp, ebp;
    _asm pop ebp;
    //restore registers/flags
    _asm popfd;
    _asm popad;

    //instruction we overwrote
    _asm jmp oPrintChat
}

//Send command function
//Commands prefixed with & (not /)
void __declspec(naked) SendCommand(int cmdMode, int iMode, const char* cmdBuffer) {
//void SendCommand(int cmdMode, int iMode, const char* cmdBuffer) {
    //prepare stackframe
    _asm push ebp;
    _asm mov ebp, esp;
    _asm sub esp, __LOCAL_SIZE;

    *(int*)ptrChatiMode = iMode;
    _asm push cmdMode;
    _asm mov edx, cmdBuffer

    _asm call wrapCmdHandler;
    //wrapCmdHandler();

    //epilogue
    _asm mov esp, ebp;
    _asm pop ebp;
    //
    _asm ret;

}

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
int findEntOffset;
int entbyoidCount;

int entityListMax;// = *(int*)0xaa4c5c;
void* ptrEntityListMax;
//Address of signature = game.dll + 0x00011724 +0x2
const char* entityListMaxPattern = "\x39\x35\x00\x00\x00\x00\x7E\x00\x8B\xCE\xE8\x00\x00\x00\x00\x84\xC0\x74\x00\x8B\xCE\xE8\x00\x00\x00\x00\x66\x8B";
const char* entityListMaxMask = "xx????x?xxx????xxx?xxx????xx";

struct entName_t {
    char name[48];
};

//Entity names
int entOffsetNum;
int entNameLen;
int entObjId;

entName_t entNameList[2000];


void DumpEntities() {
    memset(EntityList, 0, sizeof(EntityList));
    memset(entNameList, 0, sizeof(entNameList));
    for (int i = 0; i < entityListMax; i++) {
        if (EntityPtrSanityCheck(i)) {
            tempAddress = GetEntityPointer(i);
            if (tempAddress) {
                EntityList[i] = tempAddress;
                GetEntityName(3, i, entNameList[i].name, 48);
            }
        }

    }
};

int findEntityByName(char* name) {
    for (int i = 0; i < 2000; i++) {
        if (EntityPtrSanityCheck(i)) {
            std::string temp1 = name;
            std::string temp2 = entNameList[i].name;
            if (temp2.rfind(temp1, 0) == 0) {
                    return i;
            }
        }
    }
    return false;
}

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


int findEntityByOid(short oid) {
    for (entbyoidCount = 0; entbyoidCount < 2000; entbyoidCount++) {
        if ((int)EntityList[entbyoidCount] == 0) {
            //std::cout << "Not found" << std::endl;
            return 0;
        }
        unsigned char* tempPtr = reinterpret_cast<unsigned char*>(EntityList[entbyoidCount]);
        tempPtr += 0x23c;
        if (oid == *(short*)tempPtr) {
            //std::cout << "Entity Offset: " << std::dec << i << " Address: "  << std::hex << (uintptr_t)EntityList[i] << " ObjectID: " << *(uint16_t*)tempPtr << std::endl;
            return entbyoidCount;
        }
    }
    //std::cout << "Not found" << std::endl;
    return 0;
}



//Party member info
struct partymemberinfo_t {
    int hp_pct;
    int endu_pct;
    int unknown;
    int pow_pct;
    unsigned char name[20];
    unsigned char unknown2[24];
    unsigned char class_name[24];
    unsigned char unknown3[4616];
};

#define ptrPartyMemberInfo_x		0x01672AE0

partymemberinfo_t partyMembers[8];



//_SendPacket Send;// = (_SendPacket)0x4281df;
typedef int(__cdecl* _SendPacket)(const char* packetBuffer, DWORD packetHeader, DWORD packetLen, DWORD unknown);
_SendPacket oSendPacket;


struct send_packet {
    std::vector<char> packetBuffer;
    DWORD packetHeader;
    DWORD packetLen;
    DWORD unknown;
};

char* newBuff;
char bufferToSend[533];
std::queue<std::shared_ptr<send_packet>> sendQueue;
char const hex_chars[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
//Commands prefixed with & (not /)
int __cdecl SendHook(const char* packetBuffer, DWORD packetHeader, DWORD packetLen, DWORD unknown) {
    //std::vector<char> byteArray(packetBuffer, packetBuffer + packetLen);
    //std::string_view strBytes(byteArray.data(), byteArray.size());
    //
    //
    if (packetHeader != 0xA9 && packetHeader != 0xA3) {
        std::vector<char> logText;
        for (DWORD i = 0; i <= packetLen; ++i) {
            logText.push_back(hex_chars[((packetBuffer)[i] & 0xF0) >> 4]);
            logText.push_back(hex_chars[((packetBuffer)[i] & 0x0F) >> 0]);
            logText.push_back(' ');
        }
        logText.push_back('\0');
        ::OutputDebugStringA(std::format("{:X} {} len: {}", packetHeader, &logText[0], packetLen).c_str());
        //::OutputDebugStringA(std::format("{} len: {}", &logText[0], packetLen).c_str());
    }

    return oSendPacket(packetBuffer, packetHeader, packetLen, unknown);
}

std::vector<char> HexToBytes(const std::string& hex) {
    std::vector<char> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    return bytes;
}

//Send hook variables
//size_t sendFuncOffset = 0x281DF;
int sendHookLen = 8;
const char* internalSendPattern = "\x55\x8B\xEC\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\x3D\x00\x82\x99\x00\x00\x0F\x85";
const char* internalSendMask = "xxxx????x????xxxxxx?xx";


//New recv hook
// incoming packet handler 0x41052a



//Player position struct pointer
//Address of signature = game.dll + 0x000418CD , address at +0x2 offset
const char* ptrPlayerPositionPattern = "\x89\x3D\x00\x00\x00\x00\xF3";
const char* ptrPlayerPositionMask = "xx????x";

//Address of signature = game.dll + 0x00062F4C  ZoneYOffset at +0x2 , ZoneXOffset at + 0xF
const char* zoneOffsetPattern = "\xD9\x05\x00\x00\x00\x00\xD8\x45\x00\xD9\x5C\x00\x00\xD9\x05\x00\x00\x00\x00\xD8\x45\x00\xD9\x1C\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x7D";
const char* zoneOffsetMask = "xx????xx?xx??xx????xx?xx?x????x????xx";

void* ptrZoneXoffset;
float zoneXoffset;// = *(float*)0x149581C;
void* ptrZoneYoffset;
float zoneYoffset;// = *(float*)0x1495820;

//autorun pattern
const char* autorunPattern = "\x39\x3D\x00\x00\x00\x00\x75\x00\x39\x3D\x00\x00\x00\x00\x75";
const char* autorunMask = "xx????x?xx????x";
BYTE autorunToggle;



struct playerpos_t {
    float pos_x;
    short heading;
    unsigned char unknown[20];
    int playerSpeedFwd; //writable
    unsigned char unknown2[12];
    int momentumMaxFwdBack;
    float momentumFwdBack;
    float momentumLeftRight;
    unsigned char unknown3[12];
    float momentumFwdBackWrite;
    int unknown4;
    float pos_y;
    float writeablePos_zAdd;
    int unknown5;
    float pos_z;
    unsigned char unknown6[16];
    int rotatePlayer;
};

DWORD playerPositionPtr;
playerpos_t* playerPosition = NULL;

//pet window packet function
//aggroState // 1-Aggressive, 2-Deffensive, 3-Passive
//walkState // 1-Follow, 2-Stay, 3-GoTarg, 4-Here
//command // 1-Attack, 2-Release
typedef void(__cdecl* _PetWindow)(char aggroState, char walkState, char command);
_PetWindow PetWindow;
//Address of signature = game.dll + 0x0002AD78
const char* funcPetWindowPattern = "\x55\x8B\xEC\x51\x83\x3D\x00\x82\x99\x00\x00\x75\x00\x8A\x45\x00\x88\x45\x00\x8A\x45\x00\x88\x45\x00\x8A\x45";
const char* funcPetWindowMask = "xxxxxxxxxx?x?xx?xx?xx?xx?xx";
//"55 8B EC 51 83 3D 00 82 99 00 ? 75 ? 8A 45 ? 88 45 ? 8A 45 ? 88 45 ? 8A 45"

//Object interact request
typedef void(__cdecl* _InteractRequest)(int objId);
_InteractRequest InteractRequest;
//Address of signature = game.dll + 0x0002AE06 0x42ae06
const char* funcObjIntReqPattern = "\x55\x8B\xEC\x83\xEC\x00\x83\x3D\x28\x98\x04\x01\x00\x7E\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x59\x59\xC9\xC3\x83\x3D\x00\x82\x99\x00\x00\x75\x00\x56";
const char* funcObjIntReqMask = "xxxxx?xxxxxx?x?x????x????x????xxxxxxxxxx?x?x";
//"55 8B EC 83 EC ? 83 3D 28 98 04 01 ? 7E ? 68 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 59 59 C9 C3 83 3D 00 82 99 00 ? 75 ? 56"


//Packet item filter
void p_filterItems();
//Flags for actions to take on packets
bool filterItemFlag = false;


//Memory inventory reader
int m_readItemId(int slotNum);
//Memory based clean inventory
void m_cleanInventory();

//Set target fun
typedef void(__cdecl* _SetTarget)(int entIdx, bool hasTarget);
_SetTarget SetTarget;// = (_SetTarget)0x42b2e3;

//This function updates the UI with current target set via SetTarget
typedef void(__cdecl* _SetTargetUI)();
_SetTargetUI SetTargetUI; //= (_SetTargetUI)0x48f194;

//Address of signature = game.dll + 0x0003A8F3
const char* funcSetTargetPattern = "\x55\x8B\xEC\x56\x8B\x75\x00\x3B\x35";
const char* funcSettargetMask = "xxxxxx?xx";

//Address of signature = game.dll + 0x0008F194
const char* funcSetTargetUIPattern = "\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xFF\x35";
const char* funcSetTargetUIMask = "xxxxx????xx";

//targeting
int currentTargetEntOffset;// = *(int*)0x10498b0;
//Address of signature = game.dll + 0x00024DA2  + 0x2
const char* currentTargetPattern = "\x89\x0D\x00\x00\x00\x00\x89\x0D\x00\x00\x00\x00\x89\x0D\x00\x00\x00\x00\x5F";
const char* currentTargetMask = "xx????xx????xx????x";
void* ptrCurrentTargetEntOffset;

//item structure starts at 0xf9b8d0
struct item_t {
    int itemId;
    unsigned char unknown[72];
    unsigned char itemName[80];
    unsigned char unknown2[312];
};

//DOL client does not seem to have itemID at same location
//uintptr_t* ValLoc = (uintptr_t*)(0xf9b8d0 + offset);
//Address of signature = game.dll + 0x0001BAB8 at +0x1
const char* invStartPattern = "\xBF\x00\x00\x00\x00\xF3\x00\xE8\x00\x00\x00\x00\xEB";
const char* invStartMask = "x????x?x????x";
void* ptrInventory;



//Move item Function
//Slot reference: https://github.com/Dawn-of-Light/DOLSharp/blob/9af87af011497c3fda852559b01a269c889b162e/GameServer/gameutils/IGameInventory.cs
//Ground = 1
//Backpack = 40-79
//count = 0 for non stacks
typedef void (__cdecl* _MoveItem)(int toSlot, int fromSlot, int count);
_MoveItem MoveItem; //  = (_MoveItem)0x42a976;

//Address of signature = game.dll + 0x0002A976
const char* funcMoveItemPattern = "\x55\x8B\xEC\x51\x51\x83\x3D\x00\x82\x99\x00\x00\x75\x00\x56\x6A\x00\x58\xE8\x00\x00\x00\x00\x66\x89\x00\x00\x66\x8B\x00\x00\x66\x89";
const char* funcMoveItemMask = "xxxxxxxxxxx?x?xx?xx????xx??xx??xx";




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



//Read item slot information from memory
void m_cleanInventory()
{

    //look through all slots
    for (int slotNum = 40; slotNum < 80; slotNum++) {
        unsigned char* tmpPtr = reinterpret_cast<unsigned char*>(ptrInventory);
        size_t offset = (slotNum - 40) * sizeof(item_t); //* sizeof(item_t);
        tmpPtr += offset;
        item_t itemTemp = *(item_t*)tmpPtr;
        printf("Slot %u, ItemHex: %02x, ItemId - %u, ItemName - %s\n", slotNum, itemTemp.itemId, itemTemp.itemId, itemTemp.itemName);
        if (badItemIdList.find(itemTemp.itemId) != badItemIdList.end() && m_readItemId(slotNum) == itemTemp.itemId) {
            MoveItem(1, slotNum, 0);
        }
    }


    //printf("Slot40loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc, slot40.itemId, slot40.itemId, slot40.itemName);

    /*
    uintptr_t* ValLoc2 = (uintptr_t*)(0xf9b8d0 + sizeof(item_t));
    item_t slot41 = (item_t&)*ValLoc2;

    printf("Slot41loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc2, slot41.itemId, slot41.itemId, slot41.itemName);
    */
}

void p_filterItems()
{
    //Use random number to prevent spam for calling cleaninventory
    int randomchance = 100;
    int randomnum = rand();
    
    if (randomchance > randomnum) {
        m_cleanInventory();
    }

    //if it's an incoming item packet
    //if (recvBuffer[0] == '\x02' && recvLen > 10) {
    //    int slotId = recvBuffer[7];
    //    int itemId = recvBuffer[8] << 8 | recvBuffer[9];
    //    //m_readSlot(slotId);
    //    //printf("Incoming Item to slot %u - ItemHex: %02x, ItemId: %u, Item Name - ", slotId, itemId, itemId);
    //    //print the chars
    //    for (unsigned int i = 32; i < recvLen; i++) {
    //        std::cout << recvBuffer[i];
    //    }
    //    printf("\n");
    //    if (badItemIdList.find(itemId) != badItemIdList.end()) {
    //        //if (goodItemIdList.find(itemId) == goodItemIdList.end() && m_readItemId(slotId) == itemId) {
    //        printf("Dropping slot %u - ItemHex: %02x, ItemId: %u, Item Name - ", slotId, itemId, itemId);
    //        MoveItem(1, slotId, 0);
    //    }
    //}
}

//Read item slot information from memory
int m_readItemId(int slotNum)
{
    
    size_t offset = (slotNum - 40) * sizeof(item_t); //* sizeof(item_t);
    unsigned char* tmpPtr = reinterpret_cast<unsigned char*>(ptrInventory);
    tmpPtr += offset;

    return *(int*)tmpPtr;

    //printf("Slot40loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc, slot40.itemId, slot40.itemId, slot40.itemName);
    
    /*
    uintptr_t* ValLoc2 = (uintptr_t*)(0xf9b8d0 + sizeof(item_t));
    item_t slot41 = (item_t&)*ValLoc2;
    printf("Slot41loc - %p, ItemHex: %02x, ItemId - %u, ItemName - %s\n", ValLoc2, slot41.itemId, slot41.itemId, slot41.itemName);
    */
}

void sell_inv() {
    for (int i = 47; i < 80; i++) {
        if (m_readItemId(i) > 0) {
            SellRequest(i);
        }
    }
}

int ItemSlotByName(char* name) {
    std::string targetItem = name;
    for (int slotNum = 40; slotNum < 80; slotNum++) {
        unsigned char* tmpPtr = reinterpret_cast<unsigned char*>(ptrInventory);
        size_t offset = (slotNum - 40) * sizeof(item_t); //* sizeof(item_t);
        tmpPtr += offset;
        item_t itemTemp = *(item_t*)tmpPtr;
        std::string currentItem(reinterpret_cast<char*>(itemTemp.itemName));
        //printf("Slot %u, ItemHex: %02x, ItemId - %u, ItemName - %s\n", slotNum, itemTemp.itemId, itemTemp.itemId, itemTemp.itemName);
        if (currentItem.rfind(targetItem, 0) == 0) {
            return slotNum;
        }
    }
    return false;
}

//Chat info / functions

//Latest line in Main chat window
//Address of signature = game.dll + 0x000191E4
const char* lastChatMainPattern = "\xBE\x00\x00\x00\x00\x56\x8D\x47\x00\xE8";
const char* lastChatMainMask = "x????xxx?x";
//Not sure buffer size but 512 to be safe
char lastChatBuf[256];
const char* ptrLastChatMain;
//"BE ? ? ? ? 56 8D 47 ? E8"