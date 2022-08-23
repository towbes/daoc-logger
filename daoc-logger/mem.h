#pragma once
#include <Windows.h>
#include <iostream>

bool mHook(char* src, char* dst, int len);

char* TrampHook(char* src, char* dst, unsigned int len);

bool WriteMem(void* pDst, char* pBytes, size_t size);

void Patch(char* dst, char* src, unsigned int size);