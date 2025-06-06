#include "HotKeyConfig.h"
#include <iostream>
#include "List.h"
#include <thread>
#include <Windows.h>
#include <unordered_map>
using namespace std;

extern HWND hwnd;
CSimpleIniW ini;
std::vector<HotKeyCombination> hotKeyCombinations;

// 获取键名对应的虚拟键码
BYTE GetVirtualKeyFromName(const std::wstring& keyName) {
    wchar_t inputKeyName[256];
    wcscpy_s(inputKeyName, keyName.c_str());
    _wcsupr_s(inputKeyName); // 转换为大写

    // 遍历所有可能的虚拟键码
    for (int vk = 0; vk < 256; vk++) {
        // 获取扫描码
        UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
        if (scanCode == 0) continue;

        // 对于扩展键，需要设置扩展位
        LONG lParam = (scanCode << 16);
        if (vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT ||
            vk == VK_PRIOR || vk == VK_NEXT || vk == VK_HOME || vk == VK_END ||
            vk == VK_INSERT || vk == VK_DELETE || vk == VK_NUMLOCK) {
            lParam |= (1 << 24);  // 设置扩展位
        }

        // 获取键名
        wchar_t keyText[256] = { 0 };
        if (GetKeyNameTextW(lParam, keyText, 256) > 0) {
            _wcsupr_s(keyText); // 转换为大写
            if (wcscmp(keyText, inputKeyName) == 0) {
                return static_cast<BYTE>(vk);
            }
        }
    }

    // 特殊处理鼠标按键，因为GetKeyNameText不支持
    struct MouseButton {
        const wchar_t* name;
        BYTE vk;
    };
    
    static const MouseButton mouseButtons[] = {
        {L"RBUTTON", VK_RBUTTON},
        {L"MBUTTON", VK_MBUTTON},
        {L"XBUTTON1", VK_XBUTTON1},
        {L"XBUTTON2", VK_XBUTTON2},
        {nullptr, 0}  // 结束标记
    };

    // 遍历鼠标按键数组
    for (const MouseButton* btn = mouseButtons; btn->name != nullptr; ++btn) {
        if (wcscmp(inputKeyName, btn->name) == 0) {
            return btn->vk;
        }
    }

    // 如果没有找到匹配的键，返回0
    return 0;
}

// 获取主键
int GetKey(const std::wstring& keys, HotKeyCombination* combination)
{
    std::wistringstream iss(keys);
    std::wstring key;
    int keyIndex = 0;
    while (iss >> key)
    {
        char currentKey = GetVirtualKeyFromName(key);
        if (currentKey == 0) {
            // 处理无效的键名
            continue;
        }

        if (keyIndex < 4)
            combination->keys[keyIndex++] = currentKey; // 存储主键
    }
    return 0;
}

void LoadHotKeyCombinations()
{
    CSimpleIniW::TNamesDepend sections;
    ini.GetAllSections(sections);

    for (const auto& section : sections)
    {
        std::wstring sectionName = section.pItem;
        // 检查区段名是否以"HotKey"开头
        if (sectionName.find(L"HotKey") == 0)
        {
            HotKeyCombination combination;

            // 加载修饰键
            combination.modifiers[I_Alt] = ini.GetBoolValue(sectionName.c_str(), L"alt", false) ? MOD_ALT : 0;
            combination.modifiers[I_Ctrl] = ini.GetBoolValue(sectionName.c_str(), L"ctrl", false) ? MOD_CONTROL : 0;
            combination.modifiers[I_Shift] = ini.GetBoolValue(sectionName.c_str(), L"shift", false) ? MOD_SHIFT : 0;
            combination.modifiers[I_Win] = ini.GetBoolValue(sectionName.c_str(), L"win", false) ? MOD_WIN : 0;

            // 使用GetKey获取键码
            std::wstring keys = ini.GetValue(sectionName.c_str(), L"key", L"");
            GetKey(keys, &combination);

            // 加载跟随鼠标
            combination.followMouse = ini.GetBoolValue(sectionName.c_str(), L"followMouse", false);
            // 加载UP_Active
            combination.UP_Active = ini.GetBoolValue(sectionName.c_str(), L"UP_Active", false);
            // 加载ListType
            combination.ListType = ini.GetLongValue(sectionName.c_str(), L"ListType", I_ListType_Tree);

            hotKeyCombinations.push_back(combination); // 添加到热键组合列表
            cout << sections.size() << endl; // 输出区段数量
        }
    }
}

void CallList(const HotKeyCombination& combo)
{
    PostMessage(hwnd, WM_SHOWLIST, (WPARAM)(&combo), 0); // 发送消息以显示列表
    return;
}

void SetupHotKey()
{
    ini.LoadFile(L"UiSetting.ini"); // 加载配置文件

    LoadHotKeyCombinations(); // 加载热键组合

    std::wstring type = ini.GetValue(L"HKType", L"type", L"");
    if (type == L"HK_SYSTEM_HOTKEY") {
        if (!SetupSystemHotKey()) {
            MessageBox(NULL, L"ERROR!", L"ERROR", MB_OK | MB_ICONERROR);
        }
    }
    else if (type == L"HK_POLLING") {
        // 在新线程中启动轮询
        std::thread pollingThread(SetPolling);
        pollingThread.detach();
    }
    else {
        MessageBox(NULL, L"ERROR!", L"ERROR", MB_OK | MB_ICONERROR);
    }
}

#define KEYDOWN(key) (GetAsyncKeyState(key) & 0x8000)
void SetPolling()
{
    static bool lastKeyState = false;  // 跟踪上一次的按键状态

    while (true)
    {
        Sleep(50);
        
        for (const auto& combo : hotKeyCombinations)
        {
            // 检查修饰键
            bool modifiersMatch = true;
            bool hasModifiers = false;

            if (combo.modifiers[I_Alt]) {
                hasModifiers = true;
                if (!KEYDOWN(VK_MENU)) modifiersMatch = false;
            }
            if (combo.modifiers[I_Ctrl]) {
                hasModifiers = true;
                if (!KEYDOWN(VK_CONTROL)) modifiersMatch = false;
            }
            if (combo.modifiers[I_Shift]) {
                hasModifiers = true;
                if (!KEYDOWN(VK_SHIFT)) modifiersMatch = false;
            }
            if (combo.modifiers[I_Win]) {
                hasModifiers = true;
                if (!(KEYDOWN(VK_LWIN) || KEYDOWN(VK_RWIN))) modifiersMatch = false;
            }

            // 检查主键
            bool currentKeyState = true;
            for (int i = 0; i < 4; ++i) 
                if (combo.keys[i] != 0) 
                    if (!KEYDOWN(combo.keys[i])) {
                        currentKeyState = false;
                        break;
                    }

            // 只有当所有必要的按键都被按下，且是新的按下状态时才触发
            if (modifiersMatch && currentKeyState && !lastKeyState) {
                CallList(combo); // 调用列表
                Sleep(200);  // 防止重复触发
            }

            lastKeyState = currentKeyState && modifiersMatch; // 更新按键状态
        }
    }
}

bool SetupSystemHotKey()
{
    int i = 1;
    bool success = true;
    for (const auto& combo : hotKeyCombinations)
    {
        if (!RegisterHotKeyCombination(combo, i)) {
            success = false;
            // 记录失败的热键
            std::wcerr << L"注册热键失败: " 
                      << (combo.modifiers[I_Alt] ? L"Alt+" : L"")
                      << (combo.modifiers[I_Ctrl] ? L"Ctrl+" : L"")
                      << (combo.modifiers[I_Shift] ? L"Shift+" : L"")
                      << (combo.modifiers[I_Win] ? L"Win+" : L"")
                      << combo.keys[0] << std::endl;
        }
        i++;
    }
    return success;
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

