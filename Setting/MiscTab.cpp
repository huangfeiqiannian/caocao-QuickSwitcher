#include "MiscTab.h"
#include "ui_MiscTab.h"
#include <QSettings>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QStyleFactory>
#include <QOperatingSystemVersion>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

MiscTab::MiscTab(QWidget *parent) 
    : QWidget(parent),
      ui(new Ui::MiscTabWidget),
      m_iniPath(QCoreApplication::applicationDirPath() + "/UiSetting.ini")
{
    ui->setupUi(this);
    // 连接主题改变信号
    connect(ui->treeThemeComboBox, &QComboBox::currentTextChanged, this, &MiscTab::applyTheme);
}

MiscTab::~MiscTab()
{
    delete ui;
}

void MiscTab::loadSettings(const QString& iniPath)
{
    m_iniPath = iniPath.isEmpty() ? 
        QCoreApplication::applicationDirPath() + "/UiSetting.ini" : iniPath;

    QSettings settings(m_iniPath, QSettings::IniFormat);
    
    // Load MaxTitleLength
    ui->maxTitleLengthSpinBox->setValue(settings.value("Settings/MaxTitleLength", 51).toInt());
    
    // Load TreeTheme
    QString treeTheme = settings.value("Settings/TreeTheme", "Auto").toString();
    // 确保主题值有效
    if (treeTheme != "Dark" && treeTheme != "Light" && treeTheme != "Auto") {
        treeTheme = "Auto";
    }
    ui->treeThemeComboBox->setCurrentText(treeTheme);
    
    // Load HKType
    QString hkType = settings.value("HKType/Type", "HK_POLLING").toString();
    ui->hotkeyTypeComboBox->setCurrentText(hkType);

    // 应用加载的主题
    applyTheme(treeTheme);
}

bool isSystemDarkTheme()
{
#ifdef Q_OS_WIN
    // Windows 10及以上版本支持暗黑模式检测
    if (QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows10) {
        QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
        return registry.value("AppsUseLightTheme", 1).toInt() == 0;
    }
#endif
    // 其他系统或Windows 10以下版本默认使用浅色主题
    return false;
}

void MiscTab::applyTheme(const QString& theme)
{
    QString actualTheme = theme;
    if (theme == "Auto") {
        actualTheme = isSystemDarkTheme() ? "Dark" : "Light";
    }

    if (actualTheme == "Dark") {
        // 设置深色主题
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53,53,53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(35,35,35));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53,53,53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Highlight, QColor(42,130,218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        qApp->setPalette(darkPalette);
    } else {
        // 恢复默认主题
        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setPalette(qApp->style()->standardPalette());
    }
}

void MiscTab::saveSettings()
{
    QSettings settings(m_iniPath, QSettings::IniFormat);
    
    // Save MaxTitleLength
    settings.setValue("Settings/MaxTitleLength", ui->maxTitleLengthSpinBox->value());
    
    // Save TreeTheme
    settings.setValue("Settings/TreeTheme", ui->treeThemeComboBox->currentText());
    
    // Save HKType
    settings.setValue("HKType/Type", ui->hotkeyTypeComboBox->currentText());
    
    settings.sync();
    
    if (settings.status() != QSettings::NoError) {
        QMessageBox::warning(nullptr, "保存失败", "无法写入杂项配置文件: " + m_iniPath);
    }
} 