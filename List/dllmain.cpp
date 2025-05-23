// dllmain.cpp : 定义 DLL 应用程序的入口点。
#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <List.h>
#include <InfoManager.h>
#include <UserInfo.h>
#include <CommCtrl.h>
#include <iostream>
#include <shellapi.h>
#include "Tree.h"
#include "Menu.h"
#include "common.h"

#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Info.lib")
#pragma comment(lib, "comctl32.lib")

// 全局变量定义
HINSTANCE g_hInstance = NULL;
HMENU g_hPopupMenu = NULL;
HWND hwnd = NULL;
UserItemInfo* g_items = nullptr;
int g_itemCount = 0;
bool showingTree = false;
bool showingMenu = false;

HWND g_hMenuWindow = NULL;
HWND g_hTreeWindow = NULL;
UserItemInfo* g_treeItems = nullptr;
int g_treeItemCount = 0;

HHOOK g_Menuhook = 0;
BOOL g_directSelect = TRUE;

// DLL 主入口函数
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MenuInit(hModule); // 初始化菜单
        TreeInit(hModule); // 创建树状窗口
        ImmDisableIME(GetCurrentThreadId());
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// 显示菜单函数，根据参数决定显示位置
extern "C" __declspec(dllexport) HWND WINAPI List_Menu(POINT pt, bool followMouse, UserItemInfo* items, int itemCount) {
    if (followMouse) {
        ShowWindowMenu(pt, 0, items, itemCount);
    } else {
        pt = GetMonitorCenterPoint(pt);
        ShowWindowMenu(pt, TPM_CENTERALIGN | TPM_VCENTERALIGN, items, itemCount);
    }
    return g_hMenuWindow;
}

// 显示树状视图函数，根据参数决定显示位置
extern "C" __declspec(dllexport) HWND WINAPI List_Tree(POINT pt, bool followMouse, UserItemInfo* treeItems, int treeItemCount) {
    if (showingTree)
        return g_hTreeWindow;
    showingTree = true;

    PopulateTreeView(treeItems, treeItemCount); // 填充树状视图
    SetTreeWindowPos(pt, followMouse); // 设置窗口位置

    ShowWindow(g_hTreeWindow, SW_SHOW);
    UpdateWindow(g_hTreeWindow); // 更新窗口
    return g_hTreeWindow;
}

// 在 DLL 导出函数后添加
extern "C" __declspec(dllexport) void WINAPI List_SetTheme(bool darkMode) {
    SetTreeTheme(darkMode ? ThemeMode::Dark : ThemeMode::Light);
}

