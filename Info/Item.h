#pragma once
#include "Info.h"
#include <vector>
#include <map>
#include <string>

class WindowItem {
public:
    // 淇敼涓哄湪鐜版湁map鍩虹涓婃坊鍔犵獥鍙?
    void CreateGroups(const std::vector<WinInfo>& windows, std::map<std::wstring, ItemInfo>& groups);

    // 鏍规嵁缁勫悕鏌ユ壘缁?
    static ItemInfo* FindGroup(std::map<std::wstring, ItemInfo>& items, const std::wstring& name);

    // 鏍规嵁绐楀彛鍙ユ焺鏌ユ壘鎵€鍦ㄧ殑缁?
    static ItemInfo* FindGroupByWindow(std::map<std::wstring, ItemInfo>& items, HWND hwnd);

    // 璁剧疆缁勭殑缃《鐘舵€?
    static void SetTopMost(ItemInfo& item, bool topMost);

    // 璁剧疆缁勭殑蹇嵎閿?
    static void SetHotkey(ItemInfo& item, wchar_t hotkey);

private:
    // 鑾峰彇鍒嗙粍閿紙瑙勫垯鍚嶇О鎴栫獥鍙ｆ爣棰橈級
    static std::wstring GetGroupKey(const WinInfo& window);
}; 
