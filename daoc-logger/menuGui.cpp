#pragma once
#include "stdafx.h"
#include "menuGui.h"
#include "Daoc.h"
#include "Scan.h"

bool g_ShowDemo = false;
bool g_showMenu = true;
bool useItemFilter = true;

void DrawGui() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    if (g_ShowDemo)
    {
        bool bShow = true;
        ImGui::ShowDemoWindow(&bShow);
    }
    else if (g_showMenu) {
        static float f = 0.0f;
        static int counter = 0;
        ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_Once);
        ImGui::Begin("Daoc Logger");                          
        
        //update the zone offset values
        zoneXoffset = *(float*)ptrZoneXoffset;
        zoneYoffset = *(float*)ptrZoneYoffset;

        //update current target value
        currentTargetEntOffset = *(int*)ptrCurrentTargetEntOffset;

        //Update entitylist max
        entityListMax = *(int*)ptrEntityListMax;

        if (ImGui::TreeNode("Entities")) {
            static int dumpClicked = 0;
            if (ImGui::Button("Dump Entities"))
                dumpClicked++;
            if (dumpClicked & 1)
            {
                DumpEntities();
                dumpClicked++;
            }
            static int setTargetClicked = 0;
            static char targetname[48] = "";
            if (ImGui::Button("Set Target by Name"))
                setTargetClicked++;
            if (setTargetClicked & 1)
            {
                //SetTarget(atoi(targetnum), 0);
                //SetTargetUI();
                int tarOffset = 0;
                tarOffset = findEntityByName(targetname);
                if (tarOffset > 0) {
                    SetTarget(tarOffset, 0);
                    SetTargetUI();
                }

                setTargetClicked++;
            }
            ImGui::SameLine();
            ImGui::InputText("##targetname", targetname, IM_ARRAYSIZE(targetname));
            static int setTargetOffsetClicked = 0;
            static char targetnum[10] = "";
            if (ImGui::Button("Set Target by Offset"))
                setTargetOffsetClicked++;
            if (setTargetOffsetClicked & 1)
            {
                SetTarget(atoi(targetnum), 0);
                SetTargetUI();

                setTargetOffsetClicked++;
            }
            ImGui::SameLine();
            ImGui::InputText("##targetnum", targetnum, IM_ARRAYSIZE(targetnum));
            // Child 1: no border, enable horizontal scrollbar
            {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                static bool disable_mouse_wheel = false;
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                if (disable_mouse_wheel)
                    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
                ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);
                for (int i = 0; i < entityListMax; i++)
                    if (EntityList[i] != 0) {
                        unsigned char* tempPtr = reinterpret_cast<unsigned char*>(EntityList[i]);
                        const auto type = *(uint8_t*)(tempPtr + 0x28e);
                        const auto objId = *(uint16_t*)(tempPtr + 0x23c);
                        const auto level = ((*(uint32_t*)(tempPtr + 0x60) ^ 0xCB96) / 74) - 23;//unencode level: ((*(uint32_t*)(tempPtr + 0x60) ^ 0xCB96)/74) - 23
                        const auto health = (*(uint32_t*)(tempPtr + 0x228) ^ 0xbe00) / 0x22 - 0x23;//unencode health: (*(uint32_t*)(tempPtr + 0x228) ^ 0xbe00) / 0x22 - 0x23
                        const auto posx = *(float*)(tempPtr + 0x48) - zoneXoffset;
                        const auto posy = *(float*)(tempPtr + 0x370) - zoneYoffset;
                        const auto posz = *(float*)(tempPtr + 0xE7C);
                        const auto heading = ((((*(uint16_t*)(tempPtr + 0xcb6) + 0x800) * 0x168) / 0x1000) % 0x168); //from 0x41948d
                        ImGui::Text("%d : 0x%x - Type: %d - %d - %s - Lvl: %d | hp: %d | %.0f %.0f %.0f %d", i, EntityList[i], type, objId, entNameList[i].name, level, health, posx, posy, posz, heading);
                    }
                
                ImGui::EndChild();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Buffs"))
        {
            static int buffClicked = 0;
            if (ImGui::Button("Dump Buffs"))
                buffClicked++;
            if (buffClicked & 1)
            {

                DumpBuffs();
                buffClicked++;
            }
            // Child 1: no border, enable horizontal scrollbar
            {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                static bool disable_mouse_wheel = false;
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                if (disable_mouse_wheel)
                    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
                ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);
                for (int i = 0; i < 75; i++) {
                    if (plyrBuffTable[i].name[0] != '\0')
                        ImGui::Text("Buff %d: %s", i, plyrBuffTable[i].name);
                }

                ImGui::EndChild();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Skills"))
        {
            static int skillClicked = 0;
            if (ImGui::Button("Dump Skills"))
                skillClicked++;
            if (skillClicked & 1)
            {

                DumpSkills();
                skillClicked++;
            }
            ImGui::SameLine();
            static int useSkillClicked = 0;
            static char skillnum[10] = "";
            if (ImGui::Button("Use Skill"))
                useSkillClicked++;
            if (useSkillClicked & 1)
            {

                UseSkill(atoi(skillnum), 1);
                useSkillClicked++;
            }
            ImGui::SameLine();
            
            ImGui::InputText("##skillnum", skillnum, IM_ARRAYSIZE(skillnum));
            // Child 1: no border, enable horizontal scrollbar
            {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                static bool disable_mouse_wheel = false;
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                if (disable_mouse_wheel)
                    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
                ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);
                for (int i = 0; i < 150; i++) {
                    if (plyrUseSkillTable[i].name[0] != '\0') {
                        ImGui::Text("Skill %d: %s", i, plyrUseSkillTable[i].name);
                    }
                }

                ImGui::EndChild();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Spells"))
        {
            static int spellClicked = 0;
            if (ImGui::Button("Dump Spells"))
                spellClicked++;
            if (spellClicked & 1)
            {

                DumpSpells();
                spellClicked++;
            }
            ImGui::SameLine();
            static int useSpellClicked = 0;
            static char spellnum[10] = "";
            if (ImGui::Button("Use Spell"))
                useSpellClicked++;
            if (useSpellClicked & 1)
            {

                UseSpell(1, atoi(spellnum));
                useSpellClicked++;
            }
            ImGui::SameLine();

            ImGui::InputText("##spellnum", spellnum, IM_ARRAYSIZE(spellnum));
            // Child 1: no border, enable horizontal scrollbar
            {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                static bool disable_mouse_wheel = false;
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                if (disable_mouse_wheel)
                    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
                ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);
                for (int i = 0; i < 150; i++) {
                    if (plyrUseSpellTable[i].name[0] != '\0') {
                        ImGui::Text("Spell %d: %s", i, plyrUseSpellTable[i].name);
                    }
                }

                ImGui::EndChild();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("PartyInfo"))
        {
            if (ptrPartyMemberInfo_x != NULL) {
                //Create a new pointer and cast as unsigned char to be able to offset by single byte values
                unsigned char* ptrMemberBytePtr = reinterpret_cast<unsigned char*>(partyMembers);
                unsigned char* ptrPInfoBytePtr = reinterpret_cast<unsigned char*>(ptrPartyMemberInfo_x);
                //Copy the 8 partymemberinfo_t structures into the shared memory
                for (int i = 0; i < 8; i++) {
                    //copy party member info
                    *(partymemberinfo_t*)ptrMemberBytePtr = *(partymemberinfo_t*)ptrPInfoBytePtr;
                    //Move offset to next party member
                    ptrMemberBytePtr += sizeof(partymemberinfo_t);
                    ptrPInfoBytePtr += sizeof(partymemberinfo_t);
                    ImGui::Text("Idx %d %s - %s - Hp: %d Pow: %d", i, partyMembers[i].name, partyMembers[i].class_name, partyMembers[i].hp_pct, partyMembers[i].pow_pct);
                }
            }//Todo add exception
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("PlayerInfo"))
        {
            const auto posx = playerPosition->pos_x - zoneXoffset;
            const auto posy = playerPosition->pos_y - zoneYoffset;
            const auto posz = playerPosition->pos_z;
            const auto heading = ((((playerPosition->heading + 0x800) * 0x168) / 0x1000) % 0x168);
            ImGui::Text("Position (x,y,z,heading):");
            ImGui::Text("%.0f %.0f %.0f %d", posx, posy, posz, heading);
            unsigned char* tempPtr = reinterpret_cast<unsigned char*>(plyrHpPtr);
            plyr_hp = *(int*)tempPtr;
            plyr_pow = *(int*)(tempPtr + 0x4);
            plyr_endu = *(int*)(tempPtr + 0x8);
            ImGui::Text("HP: %d", plyr_hp);
            ImGui::Text("Pow: %d", plyr_pow);
            ImGui::Text("Endu: %d", plyr_endu);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Inventory"))
        {
            static int cleanInvClicked = 0;
            if (ImGui::Button("Clean Inventory"))
                cleanInvClicked++;
            ImGui::SameLine();
            ImGui::Text("Drops bad items");
            if (cleanInvClicked & 1)
            {

                m_cleanInventory();
                cleanInvClicked++;
            }
            static int sellInvClicked = 0;
            if (ImGui::Button("Sell Inventory"))
                sellInvClicked++;
            ImGui::SameLine();
            ImGui::Text("Sells slots 47-80 (start of backpack is 40)");
            if (sellInvClicked & 1)
            {

                sell_inv();
                sellInvClicked++;
            }

            //look through all slots
            for (int slotNum = 40; slotNum < 80; slotNum++) {
                size_t offset = (slotNum - 40) * sizeof(item_t); //* sizeof(item_t);
                unsigned char* tmpPtr = reinterpret_cast<unsigned char*>(ptrInventory);
                tmpPtr += offset;
                item_t itemTemp = (item_t&)*tmpPtr;
                ImGui::Text("Slot %u, ItemHex: %02x, ItemId - %u, ItemName - %s\n", slotNum, itemTemp.itemId, itemTemp.itemId, itemTemp.itemName);
            }
            ImGui::TreePop();
        }

        //if (ImGui::TreeNode("Chat"))
        //{
        //    memcpy(lastChatBuf
        //    ImGui::TreePop();
        //}

        static int itemFilterClicked = 0;

        if (!useItemFilter) {
            if (ImGui::Button("Toggle Item Filter On"))
                itemFilterClicked++;
        }
        else {
            if (ImGui::Button("Toggle Item Filter Off"))
                itemFilterClicked++;
        }
        if (itemFilterClicked & 1)
        {
            useItemFilter = !useItemFilter;
            itemFilterClicked++;
        }
        if (useItemFilter) {
            p_filterItems();
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

bool blockCommand = false;
DWORD cmdType = 0;

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

    if (!ProcessCmd(oCmd, &oCmdType)) {
        _asm {
            //Setup the call to game function cmd handler
            //String is put into edx, type is pushed onto stack
            //mov edx, oCmd
            //push cmdType

            //epilogue
            mov esp, ebp
            pop ebp
            //restore registers/flags
            popfd
            popad

            //instructions overwritten by hook
            push ebp
            mov ebp, esp
            sub esp, 0x408

            //jump to the game function
            jmp newCmdHandler
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
    _asm mov eax, 0x7953B8
    _asm jmp newPrintChat
}

void LoadHooks() {

    //get entity pointer function address
    GetEntityPointer = (_GetEntityPointer)(ScanModIn((char*)getEntityPtrPattern, (char*)getEntityPtrMask, "game.dll"));
    oGetEntityName = (uintptr_t)(ScanModIn((char*)getEntityNamePattern, (char*)getEntityNameMask, "game.dll"));
    EntityPtrSanityCheck = (_EntityPtrSanityCheck)(ScanModIn((char*)entityPtrCheckPattern, (char*)entityPtrCheckMask, "game.dll"));
    GetNPCEntityOffsetFromOid = (_getNPCEntityOffsetFromOid)(ScanModIn((char*)getEntityOffsetFromOidPattern, (char*)getEntityOffsetFromOidMask, "game.dll"));

    //EntityList Max
    void* tmpEntityListMax = (void*)(ScanModIn((char*)entityListMaxPattern, (char*)entityListMaxMask, "game.dll"));
    DWORD locEntityListMax = (DWORD)((size_t)tmpEntityListMax + 0x2);
    ptrEntityListMax = *(void**)locEntityListMax;

    //player buffs address
    plyrBuffTableTemp = (void*)(ScanModIn((char*)plyrBuffTablePattern, (char*)plyrBuffTableMask, "game.dll"));
    plyrBuffTableLoc = (DWORD)((size_t)plyrBuffTableTemp + 0x1);

    //Player skills
    plyrSkillTableTemp = (void*)(ScanModIn((char*)plyrSkillTablePattern, (char*)plyrSkillTableMask, "game.dll"));
    plyrSkillTableLoc = (DWORD)((size_t)plyrSkillTableTemp + 0x1);
    plyrUseSkillTableTemp = (void*)(ScanModIn((char*)plyrUseSkillTablePattern, (char*)plyrUseSkillTableMask, "game.dll"));
    plyrUseSkillTableLoc = (DWORD)((size_t)plyrUseSkillTableTemp + 0x1);

    //UseSkill Address
    UseSkill = (_UseSkill)(ScanModIn((char*)funcUseSkillPattern, (char*)funcUseSkillMask, "game.dll"));

    //Player spells
    plyrUseSpellTableTemp = (void*)(ScanModIn((char*)plyrUseSpellTablePattern, (char*)plyrUseSpellTableMask, "game.dll"));
    plyrUseSpellTableLoc = (DWORD)((size_t)plyrUseSpellTableTemp + 0x3);

    //UseSpell Address
    UseSpell = (_UseSpell)(ScanModIn((char*)funcUseSpellPattern, (char*)funcUseSpellMask, "game.dll"));
    
    //Player hp/pow/endu
    void* ptrPlayerHp = (void*)(ScanModIn((char*)ptrPlyrHpPattern, (char*)ptrPlyrHpMask, "game.dll"));
    DWORD ptrPlyrHpLocation = (DWORD)((size_t)ptrPlayerHp + 0x1);
    plyrHpPtr = *(void**)ptrPlyrHpLocation;

    //Player position
    void* ptrPlayerPosition = (void*)(ScanModIn((char*)ptrPlayerPositionPattern, (char*)ptrPlayerPositionMask, "game.dll"));
    DWORD ptrPlayerPositionLocation = (DWORD)((size_t)ptrPlayerPosition + 0x2);
    void* playerPositionPtr = *(void**)ptrPlayerPositionLocation;
    playerPosition = *(playerpos_t**)playerPositionPtr;

    //Zone x and y offsets
    void* ptrZoneOffset = (void*)(ScanModIn((char*)zoneOffsetPattern, (char*)zoneOffsetMask, "game.dll"));
    DWORD locZoneYoffset = (DWORD)((size_t)ptrZoneOffset + 0x2);
    DWORD locZoneXoffset = (DWORD)((size_t)ptrZoneOffset + 0xF);
    ptrZoneYoffset = *(void**)locZoneYoffset;
    ptrZoneXoffset = *(void**)locZoneXoffset;

    //inventory
    void* ptrInventoryTemp = (void*)(ScanModIn((char*)invStartPattern, (char*)invStartMask, "game.dll"));
    DWORD locInventory = (DWORD)((size_t)ptrInventoryTemp + 0x1);
    ptrInventory = *(void**)locInventory;

    //MoveItem function
    MoveItem = (_MoveItem)(ScanModIn((char*)funcMoveItemPattern, (char*)funcMoveItemMask, "game.dll"));

    //SellRequest
    SellRequest = (_SellRequest)(ScanModIn((char*)sellRequestPattern, (char*)sellRequestMask, "game.dll"));

    //Current target ent offset
    void* ptrCurrTarget = (void*)(ScanModIn((char*)currentTargetPattern, (char*)currentTargetMask, "game.dll"));
    DWORD locCurrTarget = (DWORD)((size_t)ptrCurrTarget + 0x2);
    ptrCurrentTargetEntOffset = *(void**)locCurrTarget;

    //target function
    SetTarget = (_SetTarget)(ScanModIn((char*)funcSetTargetPattern, (char*)funcSettargetMask, "game.dll"));
    SetTargetUI = (_SetTargetUI)(ScanModIn((char*)funcSetTargetUIPattern, (char*)funcSetTargetUIMask, "game.dll"));

    //chat
    void* ptrLastChatMsg = (void*)(ScanModIn((char*)lastChatMainPattern, (char*)lastChatMainMask, "game.dll"));
    DWORD locLastChatMsg = (DWORD)((size_t)ptrLastChatMsg + 0x1);
    ptrLastChatMain = (const char*)locLastChatMsg;

    //Cmd handler hook
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oCmdHandler, wrapCmdHandler);
    long result = DetourTransactionCommit();
    if (result != NO_ERROR)
    {

    }

    //Cmd handler hook
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oPrintChat, printChat);
    result = DetourTransactionCommit();
    if (result != NO_ERROR)
    {

    }
    
}

void UnloadHooks() {
    //Cmd handler hook
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)oCmdHandler, wrapCmdHandler);
    long result = DetourTransactionCommit();
    if (result != NO_ERROR)
    {

    }

    //Cmd handler hook
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)oPrintChat, printChat);
    result = DetourTransactionCommit();
    if (result != NO_ERROR)
    {

    }
}