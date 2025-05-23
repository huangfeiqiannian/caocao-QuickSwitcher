#pragma once
#include "Info.h"
#include <vector>
#include <map>
#include <string>

// 窗口项类
class WindowItem {
public:
    // 创建窗口组
    void CreateGroups(const std::vector<WinInfo>& windows, std::map<std::wstring, ItemInfo>& groups);

    // 根据名称查找组
    static ItemInfo* FindGroup(std::map<std::wstring, ItemInfo>& items, const std::wstring& name);

    // 根据窗口句柄查找组
    static ItemInfo* FindGroupByWindow(std::map<std::wstring, ItemInfo>& items, HWND hwnd);

    // 设置窗口为最上层
    static void SetTopMost(ItemInfo& item, bool topMost);

    // 设置窗口热键
    static void SetHotkey(ItemInfo& item, wchar_t hotkey);

private:
    // 获取组键
    static std::wstring GetGroupKey(const WinInfo& window);
}; 
