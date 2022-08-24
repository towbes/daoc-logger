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

        ImGui::Begin("Daoc Logger");                          // Create a window called "Hello, world!" and append into it.

        if (ImGui::TreeNode("Entities")) {
            static int dumpClicked = 0;
            if (ImGui::Button("Dump Entities"))
                dumpClicked++;
            if (dumpClicked & 1)
            {
                DumpEntities();
                dumpClicked++;
            }
            // Child 1: no border, enable horizontal scrollbar
            {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                static bool disable_mouse_wheel = false;
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                if (disable_mouse_wheel)
                    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
                ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);
                for (int i = 0; i < 100; i++)
                    if (EntityList[i] != 0) {
                        unsigned char* tempPtr = reinterpret_cast<unsigned char*>(EntityList[i]);
                        ImGui::Text("%d : 0x%x - %d - %s", i, EntityList[i], *(uint16_t*)(tempPtr + 0x23c), entNameList[i].name);
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
}