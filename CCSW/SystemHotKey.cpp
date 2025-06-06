#include <windows.h>
#include "HotKeyConfig.h"
#include <iostream>
#include <List.h>
#include <QtCore/QSettings>

extern std::vector<HotKeyCombination> hotKeyCombinations;
bool RegisterHotKeyCombination(const HotKeyCombination& combo, int i);

void SetupSystemHotKey()
{
    // 注册所有热键
    int i = 1;
    for (const auto& combo : hotKeyCombinations)
    {
        if (!RegisterHotKeyCombination(combo, i)) {
            wchar_t errorMsg[256];
            swprintf_s(errorMsg, L"Failed to register hotkey #%d!", i);
            MessageBox(NULL, errorMsg, L"Error", MB_OK | MB_ICONERROR);
        }
        i++;
    }

    // 线程消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_HOTKEY)
        {
            // 处理热键消息
            int index = msg.wParam - 1;
            if (index >= 0 && index < hotKeyCombinations.size()) {
                bool followMouse = hotKeyCombinations[index].followMouse;
                bool UP_Active = hotKeyCombinations[index].UP_Active;
                CallList(hotKeyCombinations[index]);
            }
        }
    }
}

void CleanupSystemHotKey()
{
    for (int i = 0; i < hotKeyCombinations.size(); ++i)
        UnregisterHotKey(NULL, i + 1); // 清理注册的热键
}

bool RegisterHotKeyCombination(const HotKeyCombination& combo, int i)
{
    int modifiers = 0;
    if (combo.modifiers[I_Alt]) modifiers = modifiers | MOD_ALT;
    if (combo.modifiers[I_Ctrl]) modifiers = modifiers | MOD_CONTROL;
    if (combo.modifiers[I_Shift]) modifiers = modifiers | MOD_SHIFT;
    if (combo.modifiers[I_Win]) modifiers = modifiers | MOD_WIN;

    // 使用热键组合的索引作为ID
    if (!RegisterHotKey(NULL, i, modifiers, combo.keys[0]))
    {
        DWORD errorCode = GetLastError();
        std::wcerr << L"Failed to register hotkey! Error code: " << errorCode << std::endl;
        return false;
    }
    return true;
}