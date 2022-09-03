#pragma once
#include "targetver.h"

//windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <thread>
#include <chrono>
#include "commctrl.h"
#include <TlHelp32.h>

//detours
#include "detours.h"

//d3d9
#include <d3d9.h>
#include <d3dx9.h>

//imgui
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"