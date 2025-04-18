#pragma once
#include <Windows.h>
#include <CommCtrl.h>

// 激活窗口
void ActivateWindow(HWND hwndTarget);

// 菜单窗口初始化
HWND MenuInit(HINSTANCE hInstance);

// 创建菜单图标位图
HBITMAP CreateMenuIconBitmap(HICON hIcon, COLORREF bgColor = CLR_NONE);

// 获取窗口 DPI
int CGetDpiForWindow(HWND hwnd);
int ScaleForDpi(int value, int dpi);

// 计算显示器中心点
POINT GetMonitorCenterPoint(POINT pt);

// 设置最大标题长度
extern "C" __declspec(dllexport) void WINAPI LoadMaxTitleLength(int maxTitleLength);
wchar_t* WINAPI ProcTitle(wchar_t* Title);

// 处理输入法
void SetInputMethod(HWND hwnd, const wchar_t* method);