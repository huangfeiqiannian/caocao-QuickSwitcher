#include "Setting.h"
#include "ShortcutTab.h"
#include "RuleTab.h"
#include "MiscTab.h"
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QVariant>
#include <QProcess>
#include <QCoreApplication>

Setting::Setting(QWidget* parent)
    : QMainWindow(parent),
      m_shortcutTab(new ShortcutTab(this)),
      m_ruleTab(new RuleTab(this)),
      m_miscTab(new MiscTab(this))
{
    // 初始化 UI
    ui.setupUi(this);

    // 初始化快捷键选项卡
    ui.hotkeyTab->layout()->addWidget(m_shortcutTab);

    // 初始化规则选项卡
    ui.ruleTab->layout()->addWidget(m_ruleTab);

    // 初始化杂项选项卡
    ui.miscTab->layout()->addWidget(m_miscTab);

    // 连接按钮信号
    connect(ui.saveButton, &QPushButton::clicked, this, &Setting::onSaveClicked);
    connect(ui.applyButton, &QPushButton::clicked, this, &Setting::onApplyClicked);

    // 加载设置
    loadSettings();
}

Setting::~Setting()
{
    // 资源会自动释放
}

void Setting::loadSettings()
{
    m_shortcutTab->loadSettings("");
    m_ruleTab->loadSettings("");
    m_miscTab->loadSettings("");
}

void Setting::onSaveClicked()
{
    // 保存所有标签页的设置
    m_shortcutTab->saveSettings();
    m_ruleTab->saveSettings();
    m_miscTab->saveSettings();

    // 显示保存成功对话框
    QMessageBox msgBoxSave;
    msgBoxSave.setWindowTitle("保存成功");
    msgBoxSave.setText("所有配置已保存");
    msgBoxSave.setIcon(QMessageBox::Information);
    msgBoxSave.setStandardButtons(QMessageBox::Ok);
    msgBoxSave.button(QMessageBox::Ok)->setText("确定");
    msgBoxSave.exec();
}

void Setting::onApplyClicked()
{
    // 保存并应用所有标签页的设置
    m_shortcutTab->saveSettings();
    m_ruleTab->saveSettings();
    m_miscTab->saveSettings();
    
    applyToCCSW();

    // 显示应用成功对话框
    QMessageBox msgBoxApply;
    msgBoxApply.setWindowTitle("应用成功");
    msgBoxApply.setText("所有配置已保存并应用");
    msgBoxApply.setIcon(QMessageBox::Information);
    msgBoxApply.setStandardButtons(QMessageBox::Ok);
    msgBoxApply.button(QMessageBox::Ok)->setText("确定");
    msgBoxApply.exec();
}

void Setting::applyToCCSW()
{
    HWND hwnd = FindWindow(L"{902B2675-F285-4CBE-87C8-4D221779EC6D}", L"{902B2675-F285-4CBE-87C8-4D221779EC6D}");
    if (hwnd)
        SendMessage(hwnd, WM_USER + 2, 0, 0);
    else {
        QString exePath = QCoreApplication::applicationDirPath() + "/CCSW.EXE";
        if (QFile::exists(exePath))
            QProcess::startDetached(exePath);
        else
            QMessageBox::warning(nullptr, "错误", "未找到 CCSW.EXE");
    }
}
