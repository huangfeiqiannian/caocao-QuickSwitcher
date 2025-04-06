#pragma once

#include <QWidget>
#include <QTableWidgetItem>
#include <QMap>
#include <QSettings>

namespace Ui {
    class RuleTabWidget;
}

class RuleDetailDialog;

class RuleTab : public QWidget
{
    Q_OBJECT

public:
    explicit RuleTab(QWidget *parent = nullptr);
    ~RuleTab();
    
    void loadSettings(const QString& iniPath);
    void saveSettings();
    void applySettings() { /* No specific apply action needed */ }

private slots:
    void onItemDoubleClicked(QTableWidgetItem *item);
    void onNewButtonClicked();
    void onDeleteButtonClicked();

private:
    Ui::RuleTabWidget *ui;
    QMap<QString, QStringList> ruleDetails;  // 存储规则详细信息
    RuleDetailDialog *detailDialog;  // 详细信息对话框
    QString m_iniPath;  // Store the ini file path
};
