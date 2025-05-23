#pragma once
#include <UserInfo.h>
#include <Windows.h>

extern "C" {
    __declspec(dllimport) bool WINAPI EnumerateWindows(UserItemInfo** outItems, int* outItemCount);
    __declspec(dllimport) void WINAPI FreeWindowList(UserItemInfo* windows, int count);
    // __declspec(dllimport) bool WINAPI ReloadRules(const wchar_t * iniPath);
}
