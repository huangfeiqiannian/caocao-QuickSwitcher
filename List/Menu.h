#pragma once
#include <Windows.h>
#include <List.h>
#include "common.h"

// 全局变量声明
extern HMENU g_hPopupMenu;
extern UserItemInfo* g_items;
extern int g_itemCount;

// 显示窗口菜单
void __stdcall ShowWindowMenu(POINT pt, UINT flags, UserItemInfo* items, int itemCount);

// 初始化菜单窗口
HWND MenuInit(HINSTANCE hInstance);

