#pragma once
#include "stdafx.h"
#include "menuGui.h"
#include "Daoc.h"
#include "Scan.h"

bool g_ShowDemo = false;

void DrawGui() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    if (g_ShowDemo)
    {
        bool bShow = true;
        ImGui::ShowDemoWindow(&bShow);
    }
    else {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Daoc Logger");                          

        //update the zone offset values
        zoneXoffset = *(float*)0x149581C;
        zoneYoffset = *(float*)0x1495820;

        //update current target value
        currentTargetEnt = *(int*)0x10498b0;

        if (ImGui::TreeNode("Entities")) {
            static int dumpClicked = 0;
            if (ImGui::Button("Dump Entities"))
                dumpClicked++;
            if (dumpClicked & 1)
            {
                DumpEntities();
                dumpClicked++;
            }
            ImGui::SameLine();
            static int setTargetClicked = 0;
            static char targetnum[48] = "";
            if (ImGui::Button("Set Target"))
                setTargetClicked++;
            if (setTargetClicked & 1)
            {
                //SetTarget(atoi(targetnum), 0);
                //SetTargetUI();
                int tarOffset = 0;
                tarOffset = findEntityByName(targetnum);
                if (tarOffset > 0) {
                    SetTarget(tarOffset, 0);
                    SetTargetUI();
                }

                setTargetClicked++;
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
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Inventory"))
        {
            //look through all slots
            for (int slotNum = 40; slotNum < 80; slotNum++) {
                size_t offset = (slotNum - 40) * sizeof(item_t); //* sizeof(item_t);
                uintptr_t* ValLoc2 = (uintptr_t*)(0xf9b8d0 + offset);
                item_t itemTemp = (item_t&)*ValLoc2;
                ImGui::Text("Slot %u, ItemHex: %02x, ItemId - %u, ItemName - %s\n", slotNum, itemTemp.itemId, itemTemp.itemId, itemTemp.itemName);
            }
            ImGui::TreePop();
        }



        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void LoadHooks() {
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
    

    //Player position
    void* ptrPlayerPosition = (void*)(ScanModIn((char*)ptrPlayerPositionPattern, (char*)ptrPlayerPositionMask, "game.dll"));
    DWORD ptrPlayerPositionLocation = (DWORD)((size_t)ptrPlayerPosition + 0x2);
    void* playerPositionPtr = *(void**)ptrPlayerPositionLocation;
    playerPosition = *(playerpos_t**)playerPositionPtr;

    //target function
    SetTarget = (_SetTarget)(ScanModIn((char*)funcSetTargetPattern, (char*)funcSettargetMask, "game.dll"));
    SetTargetUI = (_SetTargetUI)(ScanModIn((char*)funcSetTargetUIPattern, (char*)funcSetTargetUIMask, "game.dll"));
}