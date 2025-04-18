#pragma once
#include <string>
#include <regex>
#include <vector>
#include <memory>
#include <Windows.h>
#include "simpleini/simpleini.h"
#include "Info.h"

enum class RuleMatchType {
    Title,
    ClassName,
    ExePath,
    Style,
    Count  // 鐢ㄤ簬鏁扮粍澶у皬
};

// 瑙勫垯鐨勫姩浣滅被鍨?
enum class RuleActionType {
    Ignore,     // 蹇界暐绐楀彛
    TopMost,    // 璁剧疆缃《
    Hotkey      // 璁剧疆蹇嵎閿?
};

// 浣跨敤浣嶆帺鐮佹爣璁拌鍒欏寘鍚殑鍖归厤绫诲瀷
enum RuleMatchFlags {
    Match_None    = 0,
    Match_Title   = 1 << static_cast<int>(RuleMatchType::Title),
    Match_Class   = 1 << static_cast<int>(RuleMatchType::ClassName),
    Match_ExePath = 1 << static_cast<int>(RuleMatchType::ExePath),
    Match_Style   = 1 << static_cast<int>(RuleMatchType::Style)
};

// 鍗曚釜鍖归厤鏉′欢
struct RuleMatch {
    RuleMatchType type;
    std::wstring pattern;           // 原始模式字符串
    std::shared_ptr<std::wregex> regex;  // 预编译的正则表达式
    bool isRegex;
    
    // 对于Style类型的特殊处理
    DWORD styleMask = 0;           // 要检查的风格位
    DWORD styleValue = 0;          // 期望的风格值
};

// 瑙勫垯鍔ㄤ綔
struct RuleAction {
    RuleActionType type;
    wchar_t hotkey;
};

// 瀹屾暣鐨勮鍒?
struct WindowRule {
    std::wstring name;              // 瑙勫垯鍚嶇О锛坕ni娈靛悕锛?
    std::vector<RuleMatch> matches;  // 鎵€鏈夊尮閰嶆潯浠讹紙AND鍏崇郴锛?
    RuleAction action;               // 鍖归厤鍚庣殑鍔ㄤ綔
    int matchFlags = Match_None;    // 蹇€熷垽鏂寘鍚摢浜涘尮閰嶇被鍨?

    // 浼樺寲锛氶鍏堣绠楁槸鍚﹂渶瑕佹鍒欏尮閰?
    bool hasRegex = false;
};

class WindowRuleManager {
public:
    bool LoadRules(const wchar_t* iniPath);
    bool ProcessWindow(const WinInfo& info, RuleAction& outAction,std::wstring& ruleName);
    void Clear();

    // 鏂板锛氬绐楀彛鍒楄〃搴旂敤瑙勫垯
    bool ApplyRules(std::vector<WinInfo>& windows);

    void MarkHotkeyAsUsed(wchar_t key);

    bool IsHotkeyUsed(wchar_t key);  // 鏂板锛氭鏌ュ揩鎹烽敭鏄惁宸茶浣跨敤

private:
    bool ProcessRule(const CSimpleIniW::Entry& entry, WindowRule& rule);
    bool ProcessMatchPattern(const std::wstring& pattern, RuleMatch& match);
    bool CheckRule(const WindowRule& rule, const WinInfo& info);
    void ApplyAction(HWND hwnd, const RuleAction& action);

    // 鏂板锛氬揩鎹烽敭鍒楄〃鐩稿叧
    struct DefaultHotkey {
        wchar_t key;     // 绠€鍖栦负鍙瓨鍌ㄥ瓧绗?
        bool used;       // 鏍囪鏄惁宸茶浣跨敤
    };
    std::vector<DefaultHotkey> m_defaultHotkeys;  // 榛樿蹇嵎閿垪琛?
    void LoadDefaultHotkeys(const CSimpleIniW& ini);  // 鍔犺浇榛樿蹇嵎閿?
    bool AssignDefaultHotkey(WinInfo& window);     // 鍒嗛厤榛樿蹇嵎閿?

private:
    std::vector<WindowRule> m_ignoreRules;
    std::vector<WindowRule> m_topMostRules;
    std::vector<WindowRule> m_hotkeyRules;
}; 
