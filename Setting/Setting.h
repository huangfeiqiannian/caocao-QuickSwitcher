#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Setting.h"
#include "ShortcutTab.h"
#include "RuleTab.h"
#include "MiscTab.h"

class Setting : public QMainWindow
{
    Q_OBJECT

public:
    Setting(QWidget *parent = nullptr);
    ~Setting();

private slots:
    void onSaveClicked();
    void applyToCCSW();
    void onApplyClicked();   // 应用按钮点击事件
    
private:
    void loadSettings();     // 加载设置
    
private:
    Ui::SettingClass ui;     // UI 界面
    ShortcutTab *m_shortcutTab;  // 快捷键标签页
    RuleTab *m_ruleTab;      // 规则标签页
    MiscTab *m_miscTab;      // 杂项标签页
    QString m_iniPath;       // INI 文件路径
};
