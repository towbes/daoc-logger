#pragma once

#include "ntdll_dbg.h"

//This portion of Scan.h from GuidedHacking.com

char* ScanBasic(const char* pattern, const char* mask, char* begin, size_t size) {
    size_t patternLen = strlen(mask);

    for (size_t i = 0; i < size; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j)) {
                found = false;
                break;
            }
        }
        if (found) {
            return (begin + i);
        }
    }
    return nullptr;
}


char* ScanInternal(const char* pattern, const char* mask, char* begin, size_t size) {
    char* match{ nullptr };
    MEMORY_BASIC_INFORMATION mbi{};

    for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize) {
        if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;
        match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

        if (match != nullptr && match != pattern) {
            break;
        }
    }
    return match;
}

char* TO_CHAR(wchar_t* string)
{
    size_t len = wcslen(string) + 1;
    char* c_string = new char[len];
    size_t numCharsRead;
    wcstombs_s(&numCharsRead, c_string, len, string, _TRUNCATE);
    return c_string;
}

PEB* GetPEB()
{
#ifdef _WIN64
    PEB* peb = (PEB*)__readgsqword(0x60);

#else
    PEB* peb = (PEB*)__readfsdword(0x30);
#endif

    return peb;
}

LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name)
{
    LDR_DATA_TABLE_ENTRY* ldr = nullptr;

    PEB* peb = GetPEB();

    LIST_ENTRY head = peb->Ldr->InMemoryOrderModuleList;

    LIST_ENTRY curr = head;

    while (curr.Flink != head.Blink)
    {
        LDR_DATA_TABLE_ENTRY* mod = (LDR_DATA_TABLE_ENTRY*)CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (mod->FullDllName.Buffer)
        {
            char* cName = TO_CHAR(mod->BaseDllName.Buffer);

            if (_stricmp(cName, name.c_str()) == 0)
            {
                ldr = mod;
                break;
            }
            delete[] cName;
        }
        curr = *curr.Flink;
    }
    return ldr;
}

char* ScanModIn(char* pattern, char* mask, std::string modName)
{
    LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(modName);

    char* match = ScanInternal(pattern, mask, (char*)ldr->DllBase, ldr->SizeOfImage);

    return match;
}


/**
 * Ashita SDK - Copyright (c) 2021 Ashita Development Team
 * Contact: https://www.ashitaxi.com/
 * Contact: https://discord.gg/Ashita
 *
 * This file is part of Ashita.
 *
 * Ashita is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ashita is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Ashita.  If not, see <https://www.gnu.org/licenses/>.
 */

template<typename T>
struct scannableiterator_t
{
    uintptr_t m_BaseAddress;
    uintptr_t m_BaseSize;

    scannableiterator_t(uintptr_t base, uintptr_t size)
        : m_BaseAddress(base), m_BaseSize(size)
    { }
    scannableiterator_t(const scannableiterator_t&) = delete;

    T* begin(void) { return (T*)this->m_BaseAddress; }
    T* end(void) { return (T*)(this->m_BaseAddress + this->m_BaseSize); }
};

/**
 * Finds the given pattern within the given address space.
 *
 * @param {uintptr_t} baseAddress - The address to start searching for the pattern within.
 * @param {uintptr_t} size - The size of data to search within. (Starting from baseAddress.)
 * @param {const char*} pattern - The pattern to search for.
 * @param {intptr_t} offset - The offset to add to the result if the pattern is found.
 * @param {uintptr_t} count - The result count to use if the pattern is known to be found more than once.
 * @return {uintptr_t} The address where the pattern was found on success, 0 otherwise.
 */
static uintptr_t FindPattern(const uintptr_t baseAddress, const uintptr_t size, const char* pattern, const intptr_t offset, const uintptr_t count)
{
    // Validate the base address and size parameters..
    if (baseAddress == 0 || size == 0)
        return 0;

    // Validate the incoming pattern is properly aligned..
    const auto len = strlen(pattern);
    if (len == 0 || len % 2 > 0)
        return 0;

    // Convert the pattern to a vectored pair..
    std::vector<std::pair<uint8_t, bool>> vpattern;
    for (size_t x = 0, y = len / 2; x < y; x++)
    {
        // Convert the current byte into the vectored pattern data..
        if (std::stringstream stream(std::string(pattern + (x * 2), 2)); stream.str() == "??")
            vpattern.push_back(std::make_pair((uint8_t)0, false));
        else
        {
            const auto b = strtol(stream.str().c_str(), nullptr, 16);
            vpattern.push_back(std::make_pair((uint8_t)b, true));
        }
    }

    // Create a scannable iterator..
    scannableiterator_t<uint8_t> data(baseAddress, size);
    auto scanStart = data.begin();
    auto result = (uintptr_t)0;

    while (true)
    {
        // Search for the pattern..
        auto ret = std::search(scanStart, data.end(), vpattern.begin(), vpattern.end(),
            [&](const uint8_t curr, const std::pair<uint8_t, bool> currPattern) {
                return !currPattern.second || curr == currPattern.first;
            });

        // Check for a match..
        if (ret != data.end())
        {
            // Use the current result if no increased count expected..
            if (result == count || count == 0)
                return std::distance(data.begin(), ret) + baseAddress + offset;

            // Scan for the next result..
            ++result;
            scanStart = ++ret;
        }
        else
            break;
    }

    return 0;
}

//End Ashita Code

uintptr_t NewScanModIn(std::string modName, const char* pattern, int offset)
{
    LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(modName);

    uintptr_t match = FindPattern((uintptr_t)ldr->DllBase, ldr->SizeOfImage, pattern, offset, 0);

    return match;
}