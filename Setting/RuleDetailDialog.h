#ifndef RULEDETAILDIALOG_H
#define RULEDETAILDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QFileDialog>
#include "ui_RuleDetailDialog.h"

namespace Ui {
class RuleDetailDialog;
}

class RuleDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RuleDetailDialog(QWidget *parent = nullptr);
    ~RuleDetailDialog();

    void setDetails(const QString& ruleName, const QStringList& details);
    QString getRuleName() const;
    QString getTitle() const;
    QString getClassName() const;
    QString getExePath() const;
    bool isIgnore() const;
    bool isTopMost() const;
    QString getHotkey() const;
    QStringList getDetails() const;

private slots:
    void onIgnoreRadioButtonToggled(bool checked);
    void onTopMostRadioButtonToggled(bool checked);
    void onHotkeyRadioButtonToggled(bool checked);
    void onBrowseButtonClicked();

private: 
    Ui::RuleDetailDialog *ui;
};

#endif // RULEDETAILDIALOG_H
