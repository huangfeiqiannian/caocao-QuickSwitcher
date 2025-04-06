#pragma once

#include <QWidget>
#include <QMap>
#include <QSettings>

namespace Ui {
    class MiscTabWidget;
}

class MiscTab : public QWidget
{
    Q_OBJECT

public:
    explicit MiscTab(QWidget *parent = nullptr);
    ~MiscTab();
    
    void loadSettings(const QString& iniPath);
    void saveSettings();
    void applySettings() { /* No specific apply action needed */ }

private slots:
    void applyTheme(const QString& theme);

private:
    Ui::MiscTabWidget *ui;
    QString m_iniPath;
}; 