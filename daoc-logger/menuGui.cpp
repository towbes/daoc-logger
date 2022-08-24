#pragma once
#include "stdafx.h"
#include "menuGui.h"
#include "Daoc.h"

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

        static int dumpClicked = 0;
        if (ImGui::Button("Dump Entities"))
            dumpClicked++;
        if (dumpClicked & 1)
        {
            DumpEntities();
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
        

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
