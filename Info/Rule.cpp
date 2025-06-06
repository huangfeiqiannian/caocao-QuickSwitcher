#include "Rule.h"
#include <algorithm>
#include "Info.h"
#include <Windows.h>

void WindowRuleManager::LoadDefaultHotkeys(const CSimpleIniW& ini) {
    m_defaultHotkeys.clear();
    
    // 从配置文件中读取默认快捷键列表
    const wchar_t* hotkeyList = ini.GetValue(L"DefaultHotkeys", L"Keys", L"");
    
    std::wstring keys(hotkeyList);
    for (wchar_t c : keys) {
        wchar_t key = towlower(c);
        if ((key >= L'0' && key <= L'9') || (key >= L'a' && key <= L'z')) {
            m_defaultHotkeys.push_back({ key, false});
        }
    }
}


bool WindowRuleManager::LoadRules(const wchar_t* iniPath) {
    Clear();
    
    CSimpleIniW ini;
    if (ini.LoadFile(iniPath) != SI_OK) {
        return false;
    }

    // 加载默认快捷键列表
    LoadDefaultHotkeys(ini);

    CSimpleIniW::TNamesDepend sections;
    ini.GetAllSections(sections);

    for (const auto& section : sections) {
        WindowRule rule;
        rule.name = section.pItem;  // 保存段名称
        
        // 处理标题匹配
        if (const wchar_t* pattern = ini.GetValue(section.pItem, L"Title")) {
            RuleMatch match;
            match.type = RuleMatchType::Title;
            if (ProcessMatchPattern(pattern, match)) {
                rule.matches.push_back(match);
                rule.matchFlags |= Match_Title;
                rule.hasRegex |= match.isRegex;
            }
        }

        // 处理类名匹配
        if (const wchar_t* pattern = ini.GetValue(section.pItem, L"ClassName")) {
            RuleMatch match;
            match.type = RuleMatchType::ClassName;
            if (ProcessMatchPattern(pattern, match)) {
                rule.matches.push_back(match);
                rule.matchFlags |= Match_Class;
                rule.hasRegex |= match.isRegex;
            }
        }

        // 处理路径匹配
        if (const wchar_t* pattern = ini.GetValue(section.pItem, L"ExePath")) {
            RuleMatch match;
            match.type = RuleMatchType::ExePath;
            if (ProcessMatchPattern(pattern, match)) {
                rule.matches.push_back(match);
                rule.matchFlags |= Match_ExePath;
                rule.hasRegex |= match.isRegex;
            }
        }

        // 处理风格匹配
        if (const wchar_t* styleStr = ini.GetValue(section.pItem, L"Style")) {
            RuleMatch match;
            match.type = RuleMatchType::Style;
            match.pattern = styleStr;
            if (ProcessMatchPattern(styleStr, match)) {
                rule.matches.push_back(match);
                rule.matchFlags |= Match_Style;
            }
        }

        // 处理动作
        if (const wchar_t* action = ini.GetValue(section.pItem, L"Action")) {
            if (_wcsicmp(action, L"Ignore") == 0) {
                rule.action.type = RuleActionType::Ignore;
                m_ignoreRules.push_back(std::move(rule));
            }
            else if (_wcsicmp(action, L"TopMost") == 0) {
                rule.action.type = RuleActionType::TopMost;
                m_topMostRules.push_back(std::move(rule));
            }
            else if (_wcsicmp(action, L"Hotkey") == 0) {
                rule.action.type = RuleActionType::Hotkey;
                
                // 检查是否指定了具体的快捷键
                const wchar_t* keyStr = ini.GetValue(section.pItem, L"Key", L"");
                if (keyStr && keyStr[0]) {
                    // 使用指定的快捷键
                    wchar_t key = towlower(keyStr[0]);
                    if ((key >= L'0' && key <= L'9') || (key >= L'a' && key <= L'z')) {
                        rule.action.hotkey = key;
                        m_hotkeyRules.push_back(std::move(rule));
                    }
                }
            }
        }
    }

    return true;
}

bool WindowRuleManager::ProcessMatchPattern(const std::wstring& pattern, RuleMatch& match) {
    match.pattern = pattern;
    match.isRegex = (pattern.length() > 2 && pattern.front() == L'/' && pattern.back() == L'/');
    
    if (match.isRegex) {
        try {
            std::wstring regexPattern = pattern.substr(1, pattern.length() - 2);
            match.regex = std::make_shared<std::wregex>(regexPattern);
            return true;
        }
        catch (const std::regex_error&) {
            return false;
        }
    }
    
    // 处理Style类型
    if (match.type == RuleMatchType::Style) {
        // 直接解析风格数值
        try {
            match.styleMask = std::stoul(pattern, nullptr, 16);
            match.styleValue = match.styleMask;  // 使用相同的值进行匹配
            return true;
        }
        catch (const std::invalid_argument&) {
            return false;
        }
    }
    
    return true;
}

bool WindowRuleManager::ProcessWindow(const WinInfo& info, RuleAction& outAction,std::wstring& ruleName) {
    // 棣栧厛妫€鏌ュ拷鐣ヨ鍒?
    for (const auto& rule : m_ignoreRules) {
        if (CheckRule(rule, info)) {
            outAction = rule.action;
            ruleName = rule.name;
            return true;
        }
    }

    // 妫€鏌ョ疆椤惰鍒?
    for (const auto& rule : m_topMostRules) {
        if (CheckRule(rule, info)) {
            outAction = rule.action;
            ruleName = rule.name;
            return true;
        }
    }

    // 妫€鏌ュ揩鎹烽敭瑙勫垯
    for (const auto& rule : m_hotkeyRules) {
        if (CheckRule(rule, info)) {
            outAction = rule.action;
            ruleName = rule.name;
            return true;
        }
    }

    return false;  // 娌℃湁鍖归厤鐨勮鍒?
}

bool WindowRuleManager::CheckRule(const WindowRule& rule, const WinInfo& info) {
    for (const auto& match : rule.matches) {
        bool matched = false;
        
        switch (match.type) {
            case RuleMatchType::Title:
                if (match.isRegex)
                    matched = std::regex_match(info.winTitle, *match.regex);
                else
                    matched = (match.pattern == info.winTitle);
                break;
            
            case RuleMatchType::ClassName:
                if (match.isRegex)
                    matched = std::regex_match(info.className, *match.regex);
                else
                    matched = (match.pattern == info.className);
                break;
            
            case RuleMatchType::ExePath:
                if (match.isRegex)
                    matched = std::regex_match(info.exePath, *match.regex);
                else
                    matched = (match.pattern == info.exePath);
                break;
            
            case RuleMatchType::Style:
                matched = ((info.style & match.styleMask) != 0);  // 检查是否包含指定风格
                break;
        }
        
        if (matched) return true;
    }
    
    return false;
}

void WindowRuleManager::Clear() {
    m_ignoreRules.clear();
    m_topMostRules.clear();
    m_hotkeyRules.clear();
    
    // 閲嶇疆榛樿蹇嵎閿娇鐢ㄧ姸鎬?
    for (auto& hotkey : m_defaultHotkeys) {
        hotkey.used = false;
    }
}

bool WindowRuleManager::ApplyRules(std::vector<WinInfo>& windows) {
    // 閲嶇疆鎵€鏈夐粯璁ゅ揩鎹烽敭鐘舵€?
    for(auto& hotkey : m_defaultHotkeys) {
        hotkey.used = false;
    }
    
    // 棣栧厛鏍囪宸叉湁缁勪腑浣跨敤鐨勫揩鎹烽敭
    for(const auto& window : windows) {
        if (window.hotkey != 0) {
            MarkHotkeyAsUsed(window.hotkey);
        }
    }
    
    // 绗竴杞細搴旂敤瑙勫垯鎸囧畾鐨勫揩鎹烽敭
    for (auto& window : windows) {
        RuleAction action;
        std::wstring ruleName;
        if (ProcessWindow(window, action, ruleName)) {
            window.ruleName = ruleName;  // 鍏堣缃鍒欏悕绉?
            
            switch (action.type) {
                case RuleActionType::Ignore:
                    window.isNew = false;
                    break;
                
                case RuleActionType::TopMost:
                    window.isTopMost = true;
                    break;
                
                case RuleActionType::Hotkey:
                    // 妫€鏌ュ揩鎹烽敭鏄惁宸茶浣跨敤
                    if (action.hotkey != 0 && !IsHotkeyUsed(action.hotkey)) {
                        window.hotkey = action.hotkey;
                        MarkHotkeyAsUsed(action.hotkey);
                    }
                    break;
            }
        }
    }

    // 绗簩杞細涓烘湭鍒嗛厤蹇嵎閿殑鏂扮獥鍙ｅ垎閰嶉粯璁ゅ揩鎹烽敭
    for (auto& window : windows) {
        if (window.isNew && window.hotkey == 0) {
            AssignDefaultHotkey(window);
        }
    }

    return true;
}

// 鏂板锛氭爣璁板揩鎹烽敭涓哄凡浣跨敤
void WindowRuleManager::MarkHotkeyAsUsed(wchar_t key) {
    for (auto& hotkey : m_defaultHotkeys) {
        if (hotkey.key == key) {
            hotkey.used = true;
            break;
        }
    }
}

// 淇敼锛氫负鍗曚釜绐楀彛鍒嗛厤榛樿蹇嵎閿?
bool WindowRuleManager::AssignDefaultHotkey(WinInfo& window) {
    // 鍒嗛厤鏂扮殑蹇嵎閿?
    for (auto& hotkey : m_defaultHotkeys) {
        if (!hotkey.used) {
            window.hotkey = hotkey.key;
            hotkey.used = true;
            return true;
        }
    }
    return false;
}

// 鏂板锛氭鏌ュ揩鎹烽敭鏄惁宸茶浣跨敤
bool WindowRuleManager::IsHotkeyUsed(wchar_t key) {
    if (key == 0) return false;
    for (const auto& hotkey : m_defaultHotkeys) {
        if (hotkey.key == key) {
            return hotkey.used;
        }
    }
    return false;
}
