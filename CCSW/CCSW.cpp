// List_TEST.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "resource.h"
#include "HotKeyConfig.h"
#include "InfoManager.h"
#include "List.h"
#include "SimpleIni/SimpleIni.h"
#include "userInfo.h"
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <Windows.h>
#include <shellapi.h>
#include <mutex>

#pragma comment(lib, "Info.lib")
#pragma comment(lib, "List.lib")

#define MUTEX_NAME L"{5D30D54D-63C7-4BD6-BA0C-FC81F0167E12}"
#define WINDOW_NAME L"{902B2675-F285-4CBE-87C8-4D221779EC6D}"

// 添加以下全局变量
HWND hwnd;
HHOOK g_mouseHook_1 = NULL;
HWND g_menuOwner = NULL;  // 菜单所有者窗口
bool g_menuActive = false;
std::unordered_map<std::string, bool> g_hotkeySettings;
UINT g_modifierKeys = 0;

// 添加全局变量
#define WM_TRAYICON (WM_USER + 1)
#define WM_RELOAD (WM_USER + 2)
#define ID_TRAY_EXIT WM_USER + 0x501
#define ID_TRAY_SETTINGS WM_USER + 0x502
#define ID_TRAY_RELOAD WM_USER + 0x503
NOTIFYICONDATA g_trayIconData = {0};
HMENU g_trayMenu = NULL;

HANDLE g_hMutex = NULL;

// 添加函数声明
bool LoadHotkeySettings();
bool CheckHotkey(WPARAM wParam, LPARAM lParam);
void CreateTrayIcon(HWND hwnd);
void ShowTrayMenu(HWND hwnd);
void LoadConfig();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    // 使用互斥体检查是否已存在实例
    g_hMutex = CreateMutex(NULL, TRUE, MUTEX_NAME);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_hMutex);
        return 1;
    }

    // 创建隐藏窗口用于处理托盘消息
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_NAME;
    RegisterClassEx(&wc);
    hwnd = CreateWindowEx(0, WINDOW_NAME, WINDOW_NAME, 0, 0, 0, 0, 0, 0, 0, hInstance, 0);

    CreateTrayIcon(hwnd);
    SetupHotKey();
    LoadConfig();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);
    }

    // 程序退出前清理资源
    CleanupSystemHotKey();
    Shell_NotifyIcon(NIM_DELETE, &g_trayIconData);
    ReleaseMutex(g_hMutex);
    CloseHandle(g_hMutex);

    return 0;
}

// 创建托盘图标
void CreateTrayIcon(HWND hwnd) {
    g_trayIconData.cbSize = sizeof(NOTIFYICONDATA);
    g_trayIconData.hWnd = hwnd;
    g_trayIconData.uID = 1;
    g_trayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_trayIconData.uCallbackMessage = WM_TRAYICON;
    g_trayIconData.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    wcscpy_s(g_trayIconData.szTip, L"CCSW Application");
    Shell_NotifyIcon(NIM_ADD, &g_trayIconData);

    // 创建托盘菜单
    g_trayMenu = CreatePopupMenu();
    AppendMenu(g_trayMenu, MF_STRING, ID_TRAY_RELOAD, L"重载");
    AppendMenu(g_trayMenu, MF_STRING, ID_TRAY_SETTINGS, L"设置");
    AppendMenu(g_trayMenu, MF_STRING, ID_TRAY_EXIT, L"退出");
}

bool isDark()
{
    HKEY hKey;
    const wchar_t* subKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    const wchar_t* valueName = L"AppsUseLightTheme";
    DWORD value = -1;
    DWORD valueSize = sizeof(DWORD);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        HRESULT hr = RegGetValue(hKey, nullptr, valueName, RRF_RT_REG_DWORD, nullptr, &value, &valueSize);
        if (hr != S_OK) value = true;
        RegCloseKey(hKey);
    }
    /*
    if (value == 0)
        return true;
    else
        return false;
        */
    return (!value);
}

// 显示托盘菜单
void ShowTrayMenu(HWND hwnd) {
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(g_trayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
}
 
// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        ChangeWindowMessageFilter(WM_RELOAD, MSGFLT_ADD);
        break;
    case WM_SETTINGCHANGE:
        if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) 
            LoadConfig();
        break;
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) 
            ShowTrayMenu(hwnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_SETTINGS:
            ShellExecute(NULL, L"open", L"setting.exe", NULL, NULL, SW_SHOWNORMAL);
            break;
        case ID_TRAY_RELOAD: 
            PostMessage(hwnd, WM_RELOAD, 0, 0);
            break;
        case ID_TRAY_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_SHOWLIST:
    {
        UserItemInfo *windows;
        int windowCount;
        EnumerateWindows(&windows, &windowCount);

        HotKeyCombination *combo;
        combo = (HotKeyCombination *)wParam;
        POINT pt;
        if (combo->followMouse)
            GetCursorPos(&pt);
        if (combo->ListType == I_ListType_Tree)
            hwnd = List_Tree(pt, combo->followMouse, windows, windowCount);
        else if (combo->ListType == I_ListType_Menu)
            hwnd = List_Menu(pt, combo->followMouse, windows, windowCount);
        break;
    }
    case WM_RELOAD:
        wchar_t exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);
        ReleaseMutex(g_hMutex);
        CloseHandle(g_hMutex);
        ShellExecute(NULL, L"open", exePath, NULL, NULL, SW_SHOWNORMAL);
        // PostQuitMessage(0);
        // break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void LoadConfig() {
    // 在初始化DLL之前读取配置文件
    CSimpleIniA ini;
    ini.LoadFile("UiSetting.ini");
    int maxTitleLength = ini.GetLongValue("Settings", "MaxTitleLength", 50);
    LoadMaxTitleLength(maxTitleLength);

    std::string treeTheme = ini.GetValue("Settings", "TreeTheme", "Light");
    bool isdark = true;
    if (treeTheme == "Auto")
        isdark = isDark();
    if (treeTheme == "Dark")
        isdark = true;
    if (treeTheme == "Light")
        isdark = false;

    List_SetTheme(isdark);
}