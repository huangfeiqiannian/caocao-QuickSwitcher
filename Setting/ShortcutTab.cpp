#include "ShortcutTab.h"
#include "ui_ShortcutTab.h"
#include <QCoreApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QApplication>
#include <QProcess>
#include <QFile>
#include <QTableWidgetItem>

// 在 .cpp 文件中定义并初始化静态成员变量
HHOOK ShortcutTab::keyboardHook = nullptr;
bool ShortcutTab::isActive = false;

ShortcutTab::ShortcutTab(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ShortcutTabWidget),
      m_iniPath(QCoreApplication::applicationDirPath() + "/UiSetting.ini")
{
    ui->setupUi(this);
    
    // 初始化表格
    initTable();
    
    // 连接按钮信号
    connect(ui->addButton, &QPushButton::clicked, this, &ShortcutTab::onAddButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ShortcutTab::onDeleteButtonClicked);
    
    // 安装键盘钩子
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);
    if (!keyboardHook) {
        QMessageBox::warning(nullptr, "错误", "无法安装键盘钩子");
    }
}

ShortcutTab::~ShortcutTab()
{
    delete ui;
    if (keyboardHook) {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = nullptr;
    }
}

void ShortcutTab::initTable()
{
    // 设置表格水平头部拉伸最后一列
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionsClickable(false);
    
    // 连接单元格编辑完成信号
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, &ShortcutTab::onCellChanged);
    
    // 加载设置
    loadSettings(m_iniPath);
}

void ShortcutTab::loadSettings(const QString& iniPath)
{
    m_iniPath = iniPath.isEmpty() ? QCoreApplication::applicationDirPath() + "/UiSetting.ini" : iniPath;
    
    // 创建 QSettings 对象
    m_settings = new QSettings(m_iniPath, QSettings::IniFormat, this);
    
    if (m_settings->status() != QSettings::NoError) {
        QMessageBox::warning(nullptr, "错误", "无法读取配置文件");
        return;
    }

    // 过滤出以 "HotKey." 开头的组
    QStringList allGroups = m_settings->childGroups();
    QStringList hotKeyGroups;
    for (const QString& group : allGroups) {
        if (group.startsWith("HotKey.")) {
            hotKeyGroups.append(group);
        }
    }

    // 清空表格并设置行数
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(hotKeyGroups.size());

    for (int i = 0; i < hotKeyGroups.size(); ++i) {
        QString section = hotKeyGroups[i].mid(7);  // 去掉 "HotKey." 前缀
        updateTable(section, i);
    }
}

void ShortcutTab::updateTable(const QString& section, int row)
{
    auto shortcutInfo = getShortcut(section);
    setupTableRow(row, section, shortcutInfo);
}

void ShortcutTab::setupTableRow(int row, const QString& section, const ShortcutInfo& info)
{
    // 设置配置名称
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(section));

    // 设置是否跟随鼠标
    QComboBox* followMouseComboBox = new QComboBox();
    followMouseComboBox->addItem("是");
    followMouseComboBox->addItem("否");
    followMouseComboBox->setCurrentIndex(info.followMouse ? 0 : 1);
    ui->tableWidget->setCellWidget(row, 1, followMouseComboBox);

    // 设置视图风格
    QComboBox* listTypeComboBox = new QComboBox();
    listTypeComboBox->addItem("经典菜单");
    listTypeComboBox->addItem("树状视图");
    listTypeComboBox->addItem("类Mac");
    listTypeComboBox->setCurrentIndex(info.listType);
    ui->tableWidget->setCellWidget(row, 2, listTypeComboBox);

    // 设置快捷键
    QString shortcut;
    if (info.ctrl) shortcut += "Ctrl+";
    if (info.shift) shortcut += "Shift+";
    if (info.alt) shortcut += "Alt+";
    if (info.win) shortcut += "Win+";
    shortcut += info.key;
    
    ShortcutLineEdit* shortcutEdit = new ShortcutLineEdit();
    shortcutEdit->setText(shortcut);
    ui->tableWidget->setCellWidget(row, 3, shortcutEdit);
}

void ShortcutTab::saveSettings()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString section = ui->tableWidget->item(row, 0)->text();

        // 获取是否跟随鼠标的值
        QComboBox* followMouseComboBox = (QComboBox*)(ui->tableWidget->cellWidget(row, 1));
        bool followMouse = (followMouseComboBox->currentIndex() == 0);

        // 获取视图风格的值
        QComboBox* listTypeComboBox = (QComboBox*)(ui->tableWidget->cellWidget(row, 2));
        int listType = listTypeComboBox->currentIndex();

        // 获取快捷键信息
        ShortcutLineEdit* shortcutEdit = (ShortcutLineEdit*)(ui->tableWidget->cellWidget(row, 3));
        QString shortcut = shortcutEdit->text();
        
        // 解析快捷键
        ShortcutInfo info;
        info.followMouse = followMouse;
        info.listType = listType;
        
        // 解析快捷键字符串
        QStringList keys = shortcut.split("+");
        for (const QString& key : keys) {
            if (key.compare("Ctrl", Qt::CaseInsensitive) == 0) {
                info.ctrl = true;
            } else if (key.compare("Shift", Qt::CaseInsensitive) == 0) {
                info.shift = true;
            } else if (key.compare("Alt", Qt::CaseInsensitive) == 0) {
                info.alt = true;
            } else if (key.compare("Win", Qt::CaseInsensitive) == 0) {
                info.win = true;
            } else {
                info.key = key;
            }
        }

        // 保存到 INI 文件
        saveShortcut(section, info);
    }
}

ShortcutTab::ShortcutInfo ShortcutTab::getShortcut(const QString& section)
{
    ShortcutInfo info;
    m_settings->beginGroup("HotKey." + section);
    info.key = m_settings->value("key").toString();
    info.ctrl = m_settings->value("ctrl", false).toBool();
    info.shift = m_settings->value("shift", false).toBool();
    info.alt = m_settings->value("alt", false).toBool();
    info.win = m_settings->value("win", false).toBool();
    info.followMouse = m_settings->value("followMouse", false).toBool();
    info.listType = m_settings->value("ListType", 0).toInt();
    m_settings->endGroup();
    return info;
}

void ShortcutTab::saveShortcut(const QString& section, const ShortcutTab::ShortcutInfo& info)
{
    m_settings->beginGroup("HotKey." + section);
    m_settings->setValue("key", info.key);
    m_settings->setValue("ctrl", info.ctrl);
    m_settings->setValue("shift", info.shift);
    m_settings->setValue("alt", info.alt);
    m_settings->setValue("win", info.win);
    m_settings->setValue("followMouse", info.followMouse);
    m_settings->setValue("ListType", info.listType);
    m_settings->endGroup();
}


// ShortcutLineEdit 实现
ShortcutTab::ShortcutLineEdit::ShortcutLineEdit(QWidget* parent) : QLineEdit(parent)
{
    setReadOnly(true);
    installEventFilter(this);  // 安装事件过滤器
}

bool ShortcutTab::ShortcutLineEdit::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::FocusIn) {
        ShortcutTab::setActive(true);
    } else if (event->type() == QEvent::FocusOut) {
        ShortcutTab::setActive(false);
    }
    return QLineEdit::eventFilter(obj, event);
}

void ShortcutTab::setActive(bool active)
{
    isActive = active;
}

std::vector<std::wstring> keys;

LRESULT CALLBACK ShortcutTab::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && isActive) {
        // 检查当前窗口是否为前台窗口或活动窗口
        HWND foregroundWindow = GetForegroundWindow();
        HWND activeWindow = GetActiveWindow();
        HWND ourWindow = QApplication::activeWindow() ? (HWND)QApplication::activeWindow()->winId() : nullptr;

        if (foregroundWindow != ourWindow || activeWindow != ourWindow) {
            return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
        } 

        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            wchar_t buffer[128];
            UINT sc = MapVirtualKey(pKeyInfo->vkCode, MAPVK_VK_TO_VSC_EX) << 16;
            int len = GetKeyNameTextW(sc, buffer, 128);
            std::wstring key(buffer, len);

            bool found = false;
            for (const auto& k : keys)
                if (k == key) {
                    found = true;
                    break;
                }
            if (!found)
                keys.push_back(key);

            std::wstring result;
            for (const auto& k : keys) {
                if (!result.empty())
                    result += L"+";
                result += k;
            }

            QLineEdit* edit = qobject_cast<QLineEdit*>(QApplication::focusWidget());

            if (edit) 
                edit->setText(QString::fromStdWString(result));
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            keys.clear();
        }
        return 1;
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void ShortcutTab::onCellChanged(int row, int column)
{
    // 只处理第一列（配置名称）的变更
    if (column == 0) {
        QString newName = ui->tableWidget->item(row, column)->text();
        
        // 检查是否有重名
        bool nameExists = false;
        
        // 检查表格中是否已有此名称
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
            if (i != row && ui->tableWidget->item(i, 0) && 
                ui->tableWidget->item(i, 0)->text() == newName) {
                nameExists = true;
                break;
            }
        }
        
        if (nameExists) {
            // 断开信号连接，避免递归调用
            disconnect(ui->tableWidget, &QTableWidget::cellChanged, this, &ShortcutTab::onCellChanged);
            
            // 显示警告
            QMessageBox::warning(this, "名称重复", 
                                "配置名称 \"" + newName + "\" 已存在，请使用其他名称。");
            
            // 生成一个新的唯一名称
            int n = 1;
            QString baseName = newName;
            QString uniqueName;
            
            do {
                uniqueName = baseName + " - " + QString::number(n++);
                nameExists = false;
                
                for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
                    if (i != row && ui->tableWidget->item(i, 0) && 
                        ui->tableWidget->item(i, 0)->text() == uniqueName) {
                        nameExists = true;
                        break;
                    }
                }
            } while (nameExists);
            
            // 设置新的唯一名称
            ui->tableWidget->item(row, column)->setText(uniqueName);
            
            // 重新连接信号
            connect(ui->tableWidget, &QTableWidget::cellChanged, this, &ShortcutTab::onCellChanged);
        }
    }
}

void ShortcutTab::onAddButtonClicked()
{
    // 查找未使用的第一个数字
    int n = 1;
    QString baseName = "未命名";
    QString section;
    bool nameExists;
    
    do {
        nameExists = false;
        section = baseName + " - " + QString::number(n);
        
        /*
        // 检查是否已存在同名配置
        QStringList allGroups = m_settings->childGroups();
        for (const QString& group : allGroups) 
            if (group == "HotKey." + section) {
                nameExists = true;
                n++;
                break;
            }
        */
        
        // 同时检查表格中是否已有此名称
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) 
            if (ui->tableWidget->item(row, 0)->text() == section) {
                nameExists = true;
                n++;
                break;
            }
        
    } while (nameExists);
    
    // 创建默认的快捷键信息
    ShortcutInfo info;
    info.key = "A";
    info.ctrl = true;
    
    // 暂时断开cellChanged信号，避免触发验证
    disconnect(ui->tableWidget, &QTableWidget::cellChanged, this, &ShortcutTab::onCellChanged);
    
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    setupTableRow(row, section, info);
    ui->tableWidget->selectRow(row);
    
    // 重新连接信号
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, &ShortcutTab::onCellChanged);
}

void ShortcutTab::onDeleteButtonClicked()
{
    int row = ui->tableWidget->currentRow();
    if (row >= 0) {
        QString section = ui->tableWidget->item(row, 0)->text();
        
        // 确认删除
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认删除", 
                                     "确定要删除配置 \"" + section + "\" 吗?",
                                     QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            // 只从表格中删除，不从INI文件中删除
            ui->tableWidget->removeRow(row);
        }
    }
} 