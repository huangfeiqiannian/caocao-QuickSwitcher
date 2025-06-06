#include "Menu.h"
#include "common.h"
#include <iostream>
using namespace std;

// 全局变量声明
extern HHOOK g_Menuhook;
extern HWND g_hMenuWindow;
extern HMENU g_hPopupMenu;
extern UserItemInfo* g_items;
extern int g_itemCount;
extern BOOL g_directSelect;
extern bool showingMenu;

// 获取指定菜单的高亮项索引
static int GetHighlightedMenuItemIndex(HMENU hMenu, int count = 0) {
    if (!hMenu) return -2; // 菜单无效
    if (count == 0)
        count = GetMenuItemCount(hMenu); // 获取菜单项数量
    for (int i = 0; i < count; i++)
        if (GetMenuState(hMenu, i, MF_BYPOSITION) & MF_HILITE)
            return i; // 返回高亮项索引
    return -1; // 未找到高亮项
}

// 清理资源的函数
void CleanupMenu();

// 窗口过程
LRESULT CALLBACK ListWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            CleanupMenu(); // 清理菜单
        }
        break;

    case WM_MENU_WHEEL_UP:
    case WM_MENU_WHEEL_DOWN: {
        HMENU hMenu = g_hPopupMenu;
        if (!hMenu) return 0; // 菜单无效

        // 获取当前菜单项信息
        int count = GetMenuItemCount(hMenu);
        int pos = GetHighlightedMenuItemIndex(hMenu, count);

        // 检查子菜单
        MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
        mii.fMask = MIIM_SUBMENU;
        GetMenuItemInfo(hMenu, pos, TRUE, &mii);
        if (mii.hSubMenu) {
            int subPos = GetHighlightedMenuItemIndex(mii.hSubMenu);
            if (subPos != -1) {
                int subCount = GetMenuItemCount(mii.hSubMenu);
                if (msg == WM_MENU_WHEEL_DOWN && subPos == subCount - 1) {
                    PostMessage(g_hMenuWindow, WM_KEYDOWN, VK_LEFT, 0);
                    PostMessage(g_hMenuWindow, WM_KEYDOWN, VK_DOWN, 0);
                    return 0;
                }
                if (msg == WM_MENU_WHEEL_UP && subPos == 0) {
                    PostMessage(g_hMenuWindow, WM_KEYDOWN, VK_LEFT, 0);
                    return 0;
                }
                PostMessage(g_hMenuWindow, WM_KEYDOWN, (msg == WM_MENU_WHEEL_UP) ? VK_UP : VK_DOWN, 0);
                return 0;
            }
        }

        // 处理上下键
        if (msg == WM_MENU_WHEEL_UP) {
            PostMessage(hwnd, WM_KEYDOWN, VK_LEFT, 0);
            PostMessage(hwnd, WM_KEYDOWN, VK_UP, 0);
            PostMessage(hwnd, WM_KEYDOWN, VK_RIGHT, 0);
            MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
            mii.fMask = MIIM_SUBMENU;
            int LastPos = pos - 1;
            if (pos == 0)
                LastPos = count - 1;
            GetMenuItemInfo(hMenu, LastPos, TRUE, &mii);
            if (mii.hSubMenu)
                PostMessage(hwnd, WM_KEYDOWN, VK_UP, 0);
            return 0;
        }
        else {
            MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
            mii.fMask = MIIM_SUBMENU;
            GetMenuItemInfo(hMenu, pos, TRUE, &mii);
            if (mii.hSubMenu) {
                PostMessage(hwnd, WM_KEYDOWN, VK_LEFT, 0);
                PostMessage(hwnd, WM_KEYDOWN, VK_RIGHT, 0);
                return 0;
            }
            PostMessage(hwnd, WM_KEYDOWN, VK_DOWN, 0);
            return 0;
        }
    }
    case WM_MENUCHAR: {
        if (LOWORD(wParam) == VK_SPACE) {
            int count = GetMenuItemCount((HMENU)lParam);
            for (int i = 0; i < count; i++)
                if (GetMenuState((HMENU)lParam, i, MF_BYPOSITION) & MF_HILITE)
                    return MAKELRESULT(i, MNC_EXECUTE); // 执行高亮项
        }
    }
    case WM_DESTROY:
        PostQuitMessage(0); // 退出消息
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam); // 默认处理
}

// 初始化ListWindow
HWND MenuInit(HINSTANCE hInstance) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = ListWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ListWindow";

    if (!RegisterClassExW(&wc)) 
        return 0;

    g_hMenuWindow = CreateWindowExW(
        0, L"ListWindow", L"窗口列表",
        WS_OVERLAPPEDWINDOW,
        10, 10, 10, 10,
        NULL, NULL, hInstance, NULL
    );

    HWND hwndPrev = GetForegroundWindow();
    ActivateWindow(g_hMenuWindow);
    ActivateWindow(hwndPrev);

    return g_hMenuWindow; // 返回窗口句柄
}

// 清理资源
void WINAPI CleanupMenu() {
    if (g_hPopupMenu) {
        DestroyMenu(g_hPopupMenu); // 销毁菜单
        g_hPopupMenu = NULL;
    }
    if (g_items) {
        FreeWindowList(g_items, g_itemCount); // 释放窗口列表
        g_items = nullptr;
        g_itemCount = 0;
    }
    if (g_Menuhook)
        UnhookWindowsHookEx(g_Menuhook); // 解除钩子
    showingMenu = false;
}

// 钩子回调函数
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        if (wParam == WM_MOUSEWHEEL) {
            SHORT wheelDelta = HIWORD(pMouseStruct->mouseData);
            SendMessage(g_hMenuWindow,
                wheelDelta > 0 ? WM_MENU_WHEEL_UP : WM_MENU_WHEEL_DOWN,
                (WPARAM)0, 0);
            return 1; // 已处理
        }
    }
    return CallNextHookEx(g_Menuhook, nCode, wParam, lParam); // 默认处理
}

// 显示窗口菜单
void WINAPI ShowWindowMenu(POINT pt, UINT flags, UserItemInfo* items, int itemCount) {
    if (showingMenu)
        return;
    showingMenu = true;

    g_hPopupMenu = CreatePopupMenu(); // 创建弹出菜单
    if (!g_hPopupMenu) {
        CleanupMenu(); // 创建失败，清理
        return;
    }

    MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
    mii.fMask = MIIM_BITMAP;

    for (int i = 0; i < itemCount; i++) {
        const UserItemInfo& item = items[i];

        if (item.windowCount == 1) {
            wchar_t menuText[MAX_NAME_LENGTH + 4] = { 0 };
            if (item.hotkey)
                swprintf_s(menuText, g_directSelect ? L"&%c %s" : L"%c %s", item.hotkey, ProcTitle((WCHAR*)(LPCWSTR)item.name));
            else
                wcscpy_s(menuText, ProcTitle((WCHAR*)(LPCWSTR)item.name));

            UINT menuId = (UINT)(UINT_PTR)item.windows[0].hWnd;
            AppendMenuW(g_hPopupMenu, MF_STRING, menuId, menuText); // 添加菜单项

            if (item.windows[0].hIcon) {
                HBITMAP hBitmap = CreateMenuIconBitmap(item.windows[0].hIcon);
                if (hBitmap) {
                    mii.hbmpItem = hBitmap;
                    SetMenuItemInfo(g_hPopupMenu, menuId, FALSE, &mii); // 设置菜单项图标
                }
            }
        }
        else if (item.windowCount > 1) {
            HMENU hSubMenu = CreatePopupMenu(); // 创建子菜单
            HICON parentIcon = NULL;

            for (int j = 0; j < item.windowCount; j++) {
                const UserWinInfo& win = item.windows[j];
                UINT menuId = (UINT)(UINT_PTR)win.hWnd;
                wchar_t subMenuText[MAX_NAME_LENGTH + 4] = { 0 };
                if (item.hotkey)
                    swprintf_s(subMenuText, L"%c %s", item.hotkey, win.winTitle);
                else
                    wcscpy_s(subMenuText, win.winTitle);

                AppendMenuW(hSubMenu, MF_STRING, menuId, subMenuText); // 添加子菜单项

                if (win.hIcon) {
                    HBITMAP hBitmap = CreateMenuIconBitmap(win.hIcon);
                    if (hBitmap) {
                        mii.hbmpItem = hBitmap;
                        SetMenuItemInfo(hSubMenu, menuId, FALSE, &mii); // 设置子菜单项图标
                    }
                    if (!parentIcon) parentIcon = win.hIcon; // 记录父项图标
                }
            }

            wchar_t menuText[MAX_NAME_LENGTH + 4] = { 0 };
            if (item.hotkey)
                swprintf_s(menuText, L"%c %s", item.hotkey, item.name);
            else
                wcscpy_s(menuText, item.name);

            UINT_PTR subMenuId = (UINT_PTR)hSubMenu;
            AppendMenuW(g_hPopupMenu, MF_POPUP, subMenuId, menuText); // 添加子菜单

            if (parentIcon) {
                HBITMAP hBitmap = CreateMenuIconBitmap(parentIcon);
                if (hBitmap) {
                    MENUITEMINFO parentMii = { sizeof(MENUITEMINFO) };
                    parentMii.fMask = MIIM_BITMAP;
                    parentMii.hbmpItem = hBitmap;
                    int position = GetMenuItemCount(g_hPopupMenu) - 1;
                    SetMenuItemInfo(g_hPopupMenu, position, TRUE, &parentMii); // 设置父项图标
                }
            }
        }
    }

    // 强制刷新菜单
    ActivateWindow(g_hMenuWindow);
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_Menuhook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0); // 设置鼠标钩子
    UINT_PTR cmd = TrackPopupMenu(g_hPopupMenu,
        TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_NOANIMATION | flags,
        pt.x, pt.y, 0, g_hMenuWindow, NULL);

    // 处理用户选择
    ActivateWindow((HWND)cmd);
    CleanupMenu(); // 清理菜单
}

