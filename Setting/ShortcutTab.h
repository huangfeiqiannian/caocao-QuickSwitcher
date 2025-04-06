#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QSettings>
#include <QComboBox>
#include <QLineEdit>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QEvent>
#include <Windows.h>
#include <QCoreApplication>
#include <QInputDialog>

namespace Ui {
    class ShortcutTabWidget;
}

class ShortcutTab : public QWidget
{
    Q_OBJECT

public:
    explicit ShortcutTab(QWidget *parent = nullptr);
    ~ShortcutTab();
    
    // 与 RuleTab 相同的接口
    void loadSettings(const QString& iniPath);
    void saveSettings();
    void applySettings() { /* 可以为空或实现具体功能 */ }

    // 获取标准化的 INI 文件路径
    static QString getIniFilePath(const QString& fileName) {
        return QCoreApplication::applicationDirPath() + "/" + fileName;
    }

private slots:
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onCellChanged(int row, int column);

private:
    struct ShortcutInfo {
        QString key;
        bool ctrl = false;
        bool shift = false;
        bool alt = false;
        bool win = false;
        bool followMouse = false;
        int listType = 0;
    };

    class ShortcutLineEdit : public QLineEdit
    {
    public:
        ShortcutLineEdit(QWidget* parent = nullptr);
    protected:
        bool eventFilter(QObject* obj, QEvent* event) override;
    };

    void initTable();
    void loadDataFromSettings();
    void updateTable(const QString& section, int row);
    ShortcutInfo getShortcut(const QString& section);
    void saveShortcut(const QString& section, const ShortcutInfo& info);
    void addNewRow(const QString& section);
    void setupTableRow(int row, const QString& section, const ShortcutInfo& info);

    static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static void setActive(bool active);

    Ui::ShortcutTabWidget *ui;
    QSettings* m_settings;
    QString m_iniPath;

    static HHOOK keyboardHook;
    static bool isActive;
}; 