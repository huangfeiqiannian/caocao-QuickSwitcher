#pragma once
#include "Info.h"
#include "Rule.h"
#include <vector>
#include <string>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();
    
    // 获取窗口列表，传入vector引用作为输出参数
    bool GetWindowList(std::vector<WinInfo>& outWindows);

    // 重新加载规则
    bool ReloadRules(const wchar_t* iniPath = nullptr);

private:
    // 初始化窗口信息
    static bool InitializeWindowInfo(HWND hwnd, WinInfo& info);
    
    // 处理单个窗口
    bool ProcessWindow(HWND hwnd, std::vector<WinInfo>& windows);
    
    // 修改GetWindowIcon的声明
    static HICON GetWindowIcon(WinInfo& info);

    // 添加静态回调函数
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

    std::wstring m_iniPath;  // 保存配置文件路径
}; 
