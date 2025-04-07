#include "RuleDetailDialog.h"
#include "ui_RuleDetailDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpressionValidator>

RuleDetailDialog::RuleDetailDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RuleDetailDialog)
{
    ui->setupUi(this);

    // 设置快捷键输入框的验证器
    ui->hotkeyEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-z]"), this));

    // 初始状态：隐藏快捷键输入框
    ui->hotkeyEdit->setVisible(false);

    // 连接信号槽
    connect(ui->ignoreRadioButton, &QRadioButton::toggled, this, &RuleDetailDialog::onIgnoreRadioButtonToggled);
    connect(ui->topMostRadioButton, &QRadioButton::toggled, this, &RuleDetailDialog::onTopMostRadioButtonToggled);
    connect(ui->hotkeyRadioButton, &QRadioButton::toggled, this, &RuleDetailDialog::onHotkeyRadioButtonToggled);
    connect(ui->saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->browseButton, &QPushButton::clicked, this, &RuleDetailDialog::onBrowseButtonClicked);
}

RuleDetailDialog::~RuleDetailDialog()
{
    delete ui;
}

void RuleDetailDialog::setDetails(const QString& ruleName, const QStringList& details)
{
    ui->ruleNameEdit->setText(ruleName);
    
    // 解析详细信息
    for (const QString& line : details) {
        if (line.startsWith("Title=")) {
            ui->titleEdit->setText(line.mid(6));
        }
        else if (line.startsWith("ClassName=")) {
            ui->classNameEdit->setText(line.mid(10));
        }
        else if (line.startsWith("ExePath=")) {
            ui->exePathEdit->setText(line.mid(8));
        }
        else if (line.startsWith("Action=")) {
            QString action = line.mid(7);
            if (action == "Ignore") {
                ui->ignoreRadioButton->setChecked(true);
            }
            else if (action == "TopMost") {
                ui->topMostRadioButton->setChecked(true);
            }
            else if (action == "Hotkey") {
                ui->hotkeyRadioButton->setChecked(true);
                // 显示快捷键相关控件
                ui->labelHotkey->setVisible(true);
                ui->hotkeyEdit->setVisible(true);
            }
        }
        else if (line.startsWith("Key=")) {
            ui->hotkeyEdit->setText(line.mid(4));  // 修改为 mid(4) 以正确获取键值
        }
    }
}

void RuleDetailDialog::onIgnoreRadioButtonToggled(bool checked)
{
    if (checked) {
        // 隐藏"快捷键："标签和输入框
        ui->labelHotkey->setVisible(false);
        ui->hotkeyEdit->setVisible(false);
    }
}

void RuleDetailDialog::onTopMostRadioButtonToggled(bool checked)
{
    if (checked) {
        // 隐藏"快捷键："标签和输入框
        ui->labelHotkey->setVisible(false);
        ui->hotkeyEdit->setVisible(false);
    }
}

void RuleDetailDialog::onHotkeyRadioButtonToggled(bool checked)
{
    // 显示或隐藏"快捷键："标签和输入框
    ui->labelHotkey->setVisible(checked);
    ui->hotkeyEdit->setVisible(checked);
}

void RuleDetailDialog::onBrowseButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择程序"), QString(), tr("可执行文件 (*.exe)"));
    if (!filePath.isEmpty()) {
        ui->exePathEdit->setText(filePath);
    }
}

QString RuleDetailDialog::getRuleName() const
{
    return ui->ruleNameEdit->text();
}

QString RuleDetailDialog::getTitle() const
{
    return ui->titleEdit->text();
}

QString RuleDetailDialog::getClassName() const
{
    return ui->classNameEdit->text();
}

QString RuleDetailDialog::getExePath() const
{
    return ui->exePathEdit->text();
}

bool RuleDetailDialog::isIgnore() const
{
    return ui->ignoreRadioButton->isChecked();
}

bool RuleDetailDialog::isTopMost() const
{
    return ui->topMostRadioButton->isChecked();
}

QString RuleDetailDialog::getHotkey() const
{
    return ui->hotkeyEdit->text();
}

QStringList RuleDetailDialog::getDetails() const
{
    QStringList details;
    
    // 添加匹配条件
    if (!ui->titleEdit->text().isEmpty())
        details.append("Title=" + ui->titleEdit->text());
    
    if (!ui->classNameEdit->text().isEmpty())
        details.append("ClassName=" + ui->classNameEdit->text());
    
    if (!ui->exePathEdit->text().isEmpty())
        details.append("ExePath=" + ui->exePathEdit->text());
    
    // 添加操作
    if (ui->ignoreRadioButton->isChecked())
        details.append("Action=Ignore");
    else if (ui->topMostRadioButton->isChecked())
        details.append("Action=TopMost");
    else if (ui->hotkeyRadioButton->isChecked()) {
        details.append("Action=Hotkey");
        if (!ui->hotkeyEdit->text().isEmpty())
            details.append("Key=" + ui->hotkeyEdit->text());
    }
    
    return details;
}