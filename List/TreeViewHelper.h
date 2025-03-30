#pragma once
#include "common.h"
#include "Tree.h"
#include <CommCtrl.h>
#include <List.h>
#include <uxtheme.h>
#include <windows.h>
#include <windowsx.h>

// 获取树状图选中项的信息
static bool GetSelectedTreeItem(HWND hTreeView, TVITEM& item, LPWSTR& pszText) {
    HTREEITEM hItem = TreeView_GetSelection(hTreeView);
    if (!hItem) return false;

    item = { 0 };
    item.mask = TVIF_PARAM | TVIF_TEXT;
    item.hItem = hItem;
    item.pszText = pszText = (LPWSTR)malloc(MAX_PATH * sizeof(WCHAR));
    item.cchTextMax = MAX_PATH;

    if (!TreeView_GetItem(hTreeView, &item)) {
        free(pszText);
        return false;
    }
    return true;
}

// 激活当前选中项
static void HandleSpaceKey(HWND hwnd)
{
    // 获取当前选中项
    HTREEITEM hItem = TreeView_GetSelection(g_hTreeView);
    if (!hItem) return;

    // 检查是否有子项
    HTREEITEM hChild = TreeView_GetChild(g_hTreeView, hItem);
    if (hChild) {
        // 如果有子项，切换展开/收起状态
        UINT state = TreeView_GetItemState(g_hTreeView, hItem, TVIS_EXPANDED);
        TreeView_Expand(g_hTreeView, hItem, state & TVIS_EXPANDED ? TVE_COLLAPSE : TVE_EXPAND);
    }
    else {
        // 如果没有子项，激活窗口
        LPWSTR pszText = nullptr;
        TVITEM item;
        if (GetSelectedTreeItem(g_hTreeView, item, pszText)) {
            ActivateWindow((HWND)item.lParam);
            CleanupTree();
            free(pszText);
        }
    }
}

// 根据快捷键码选定项
static void HandleHotkey(HWND hwnd, WPARAM wVKey)
{
    // 如果是字母键，转换为小写
// VK_A=0x41 VK_Z=0x5A
// VK_0=0x30 VK_9=0x39
    if (wVKey >= 0x41 && wVKey <= 0x5A)
        wVKey = wVKey - 0x41 + 'a';
    else if (wVKey >= VK_NUMPAD0 && wVKey <= VK_NUMPAD9)
        wVKey = wVKey - VK_NUMPAD0 + '0';
    else if (wVKey >= 0x30 && wVKey <= 0x39)
        wVKey = wVKey - 0x30 + '0';

    // 搜索匹配的快捷键项
    HTREEITEM hFoundItem = NULL;
    HTREEITEM hCurrent = TreeView_GetSelection(g_hTreeView);
    HTREEITEM hRoot = TreeView_GetRoot(g_hTreeView);

    // 从当前项开始搜索
    HTREEITEM hItem = hCurrent ? hCurrent : hRoot;
    while (hItem) {
        // 获取项文本
        wchar_t szText[MAX_PATH];
        TVITEM tvi = { 0 };
        tvi.mask = TVIF_TEXT | TVIF_HANDLE;
        tvi.hItem = hItem;
        tvi.pszText = szText;
        tvi.cchTextMax = MAX_PATH;
        if (TreeView_GetItem(g_hTreeView, &tvi)) {
            // 检查第一个字符是否匹配快捷键
            if (szText[0] == wVKey) {
                hFoundItem = hItem;
                break;
            }
        }

        // 移动到下一个项
        hItem = TreeView_GetNextItem(g_hTreeView, hItem, TVGN_NEXT);
        if (!hItem && hCurrent) { // 如果到达末尾，从开始处继续搜索
            hItem = hRoot;
            hCurrent = NULL; // 防止无限循环
        }
    }

    // 如果找到匹配项
    if (hFoundItem) {
        // 选中该项
        TreeView_SelectItem(g_hTreeView, hFoundItem);
        // 执行空间键处理
        HandleSpaceKey(hwnd);
    }

}

// 添加新函数来设置行距
static void SetTreeRowHeight(int height) {
    if (!g_hTreeView) return;

    // 获取当前 DPI
    int dpi = CGetDpiForWindow(g_hTreeView);

    // 根据 DPI 缩放高度
    int scaledHeight = ScaleForDpi(height, dpi);

    // 设置行高
    TreeView_SetItemHeight(g_hTreeView, scaledHeight);
}
