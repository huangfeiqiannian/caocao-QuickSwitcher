#include "RuleTab.h"
#include "ui_RuleTab.h"
#include "RuleDetailDialog.h"
#include <QTableWidget>
#include <QSettings>
#include <QHeaderView>
#include <QMessageBox>
#include <QCoreApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

RuleTab::RuleTab(QWidget *parent) 
    : QWidget(parent),
      ui(new Ui::RuleTabWidget),
      m_iniPath(QCoreApplication::applicationDirPath() + "/UiSetting.ini")
{
    ui->setupUi(this);
    
    // 设置表格列宽策略
    ui->ruleTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); // 第一列拉伸
    ui->ruleTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // 第二列自适应内容
    
    connect(ui->ruleTable, &QTableWidget::itemDoubleClicked, this, &RuleTab::onItemDoubleClicked);
    connect(ui->newButton, &QPushButton::clicked, this, &RuleTab::onNewButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &RuleTab::onDeleteButtonClicked);
    
    detailDialog = new RuleDetailDialog(this);
}

RuleTab::~RuleTab()
{
    delete ui;
    delete detailDialog;
}

void RuleTab::loadSettings(const QString& iniPath)
{
    m_iniPath = iniPath.isEmpty() ? 
        QCoreApplication::applicationDirPath() + "/Setting.ini" : iniPath;

    // 使用传入的文件路径
    QSettings settings(m_iniPath, QSettings::IniFormat);
    
    ui->ruleTable->setRowCount(0); // 清空表格
    ruleDetails.clear(); // 清空详细信息
    
    // 获取所有节（sections）
    QStringList sections = settings.childGroups();
    int row = 0;
    
    for (const QString& section : sections) {
        // 跳过 DefaultHotkeys 部分
        if (section == "DefaultHotkeys") {
            continue;
        }
        
        settings.beginGroup(section);
        QStringList keys = settings.childKeys();
        QStringList details;
        QString action;
        
        // 收集所有键值对
        for (const QString& key : keys) {
            QString value = settings.value(key).toString();
            QString line = key + "=" + value;
            details.append(line);
            
            // 检查是否是 Action 键
            if (key == "Action") {
                action = value;
            }
        }
        
        // 如果有 Action，添加到表格
        if (!action.isEmpty()) {
            ui->ruleTable->insertRow(row);
            ui->ruleTable->setItem(row, 0, new QTableWidgetItem(section));
            ui->ruleTable->setItem(row, 1, new QTableWidgetItem(action));
            row++;
            
            // 保存详细信息
            ruleDetails[section] = details;
        }
        
        settings.endGroup();
    }
    
    if (ui->ruleTable->rowCount() == 0) {
        QMessageBox::warning(nullptr, "加载失败", "未找到有效的规则配置: " + m_iniPath);
    } else {
        // 数据加载完成后调整列宽
        ui->ruleTable->resizeColumnsToContents();
    }
}

void RuleTab::onItemDoubleClicked(QTableWidgetItem *item)
{
    int row = item->row();
    QString ruleName = ui->ruleTable->item(row, 0)->text();
    
    if (ruleDetails.contains(ruleName)) {
        detailDialog->setDetails(ruleName, ruleDetails[ruleName]);
        if (detailDialog->exec() == QDialog::Accepted) {
            // 获取修改后的规则名称和详情
            QString newRuleName = detailDialog->getRuleName();
            QStringList newDetails = detailDialog->getDetails();
            
            // 更新表格和规则详情
            if (newRuleName != ruleName) {
                // 规则名称已更改，需要删除旧规则并添加新规则
                ruleDetails.remove(ruleName);
                ruleDetails[newRuleName] = newDetails;
                ui->ruleTable->item(row, 0)->setText(newRuleName);
            } else {
                // 只更新规则详情
                ruleDetails[ruleName] = newDetails;
            }
            
            // 更新操作列
            QString action;
            for (const QString& detail : newDetails) {
                if (detail.startsWith("Action=")) {
                    action = detail.mid(7);
                    break;
                }
            }
            ui->ruleTable->item(row, 1)->setText(action);
        }
    }
}

void RuleTab::saveSettings()
{
    // First read the original file to preserve DefaultHotkeys section
    QSettings originalSettings(m_iniPath, QSettings::IniFormat);
    QStringList defaultHotkeys;
    
    // Save DefaultHotkeys section
    if (originalSettings.childGroups().contains("DefaultHotkeys")) {
        originalSettings.beginGroup("DefaultHotkeys");
        QStringList keys = originalSettings.childKeys();
        for (const QString& key : keys) {
            defaultHotkeys.append(key);
            defaultHotkeys.append(originalSettings.value(key).toString());
        }
        originalSettings.endGroup();
    }
    
    // Create new settings file
    QSettings settings(m_iniPath, QSettings::IniFormat);
    settings.clear(); // Clear all existing settings
    
    // Restore DefaultHotkeys section
    if (!defaultHotkeys.isEmpty()) {
        settings.beginGroup("DefaultHotkeys");
        for (int i = 0; i < defaultHotkeys.size(); i += 2) {
            settings.setValue(defaultHotkeys[i], defaultHotkeys[i+1]);
        }
        settings.endGroup();
    }
    
    // Write rule sections
    for (auto it = ruleDetails.begin(); it != ruleDetails.end(); ++it) {
        QString section = it.key();
        QStringList details = it.value();
        
        settings.beginGroup(section);
        for (const QString& detail : details) {
            int equalsPos = detail.indexOf('=');
            if (equalsPos > 0) {
                QString key = detail.left(equalsPos);
                QString value = detail.mid(equalsPos + 1);
                settings.setValue(key, value);
            }
        }
        settings.endGroup();
    }
    
    // Ensure settings are written
    settings.sync();
    
    if (settings.status() != QSettings::NoError) {
        QMessageBox::warning(nullptr, "保存失败", "无法写入规则配置文件: " + m_iniPath);
    }
}

// New button clicked handler
void RuleTab::onNewButtonClicked()
{
    detailDialog->setDetails("NewRule", QStringList() << "Action=");
    if (detailDialog->exec() == QDialog::Accepted) {
        // Get the new rule name and details
        QString newRuleName = detailDialog->getRuleName();
        QStringList newDetails = detailDialog->getDetails();
        
        // Check if rule name already exists
        if (ruleDetails.contains(newRuleName)) {
            QMessageBox::warning(nullptr, "创建失败", "规则名称 '" + newRuleName + "' 已存在");
            return;
        }
        
        // Extract action
        QString action;
        for (const QString& detail : newDetails) {
            if (detail.startsWith("Action=")) {
                action = detail.mid(7);
                break;
            }
        }
        
        // Add to table
        int row = ui->ruleTable->rowCount();
        ui->ruleTable->insertRow(row);
        ui->ruleTable->setItem(row, 0, new QTableWidgetItem(newRuleName));
        ui->ruleTable->setItem(row, 1, new QTableWidgetItem(action));
        
        // Save details
        ruleDetails[newRuleName] = newDetails;
    }
}

// Delete button clicked handler
void RuleTab::onDeleteButtonClicked()
{
    // Get selected row
    QList<QTableWidgetItem*> selectedItems = ui->ruleTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(nullptr, "删除失败", "请先选择要删除的规则");
        return;
    }
    
    int row = selectedItems.first()->row();
    QString ruleName = ui->ruleTable->item(row, 0)->text();
    
    // Confirm deletion
    QMessageBox::StandardButton reply = QMessageBox::question(
        nullptr, "确认删除", "确定要删除规则 '" + ruleName + "' 吗？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        // Remove from details map
        ruleDetails.remove(ruleName);
        
        // Remove from table
        ui->ruleTable->removeRow(row);
    }
} 