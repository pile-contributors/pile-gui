#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>

namespace Ui {
class SettingsDlg;
}

class SettingsDlg : public QDialog
{
    Q_OBJECT

public:

    explicit SettingsDlg(QWidget *parent = 0);

    ~SettingsDlg();

    void saveContent ();

private slots:

    void on_btn_cmake_exe_clicked();

    void on_btn_git_exe_clicked();

    void on_btn_template_path_clicked();

    void on_btn_output_path_clicked();

    void on_btn_backup_path_clicked();

    void on_btn_hub_exe_clicked();

private:
    Ui::SettingsDlg *ui;
    QString getCMakeExe();
    QString getGitExe();
    QString getHubExe();

    static QString
    encrypt (
            const QString & value);

    static QString
    decrypt (
            const QString & value);

};

#endif // SETTINGSDLG_H
