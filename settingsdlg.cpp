#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include "setting_names.h"

#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>

SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDlg)
{
    ui->setupUi(this);


    ui->le_cmake_exe->setText (getCMakeExe());
    ui->le_git_exe->setText (getGitExe());

    QSettings stg;

    ui->le_template_path->setText (
                stg.value (STG_TEMPLATE_PATH).toString ());
    ui->le_output_path->setText (
                stg.value (STG_OUTPUT_PATH).toString ());
    ui->le_backup_path->setText (
                stg.value (STG_BACKUP_PATH).toString ());

}

QString SettingsDlg::getCMakeExe()
{
    QSettings stg;
    QString s_value;
    for (;;) {
        // from settings
        s_value = stg.value (STG_APPS_CMAKE).toString ();
        if (!s_value.isEmpty ()) break;

        // from PATH
        s_value = QStandardPaths::findExecutable ("cmake");
        if (!s_value.isEmpty ()) break;

        // be smart
        QString s = QDir::homePath ();
        if (s.at (1) == ':') {

            // we're on windows
            QDir d (QString("%1:\\").arg(s.at (1)));

            if (d.exists("Program Files\\Cmake\\bin\\cmake.exe")) {
                s_value = d.absoluteFilePath (
                            "Program Files\\Cmake\\bin\\cmake.exe");
                break;
            }

            if (d.exists("Program Files (x86)\\Cmake\\bin\\cmake.exe")) {
                s_value = d.absoluteFilePath (
                            "Program Files (x86)\\Cmake\\bin\\cmake.exe");
                break;
            }
        }

        break;
    }
    return s_value;
}

QString SettingsDlg::getGitExe()
{
    QSettings stg;
    QString s_value;
    for (;;) {
        // from settings
        s_value = stg.value (STG_APPS_GIT).toString ();
        if (!s_value.isEmpty ()) break;

        // from PATH
        s_value = QStandardPaths::findExecutable ("git");
        if (!s_value.isEmpty ()) break;

        // be smart
        QString s = QDir::homePath ();
        if (s.at (1) == ':') {

            // we're on windows
            QDir d (QString("%1:\\").arg(s.at (1)));

            if (d.exists("Program Files\\Git\\bin\\git.exe")) {
                s_value = d.absoluteFilePath (
                            "Program Files\\Git\\bin\\git.exe");
                break;
            }

            if (d.exists("Program Files (x86)\\Git\\bin\\git.exe")) {
                s_value = d.absoluteFilePath (
                            "Program Files (x86)\\Git\\bin\\git.exe");
                break;
            }
        }

        break;
    }
    return s_value;
}


SettingsDlg::~SettingsDlg()
{
    delete ui;
}

void SettingsDlg::saveContent()
{
    QSettings stg;
    stg.setValue (STG_APPS_CMAKE, ui->le_cmake_exe->text ());
    stg.setValue (STG_APPS_GIT, ui->le_git_exe->text ());
    stg.setValue (STG_TEMPLATE_PATH, ui->le_template_path->text ());
    stg.setValue (STG_OUTPUT_PATH, ui->le_output_path->text ());
    stg.setValue (STG_BACKUP_PATH, ui->le_backup_path->text ());
}

void SettingsDlg::on_btn_cmake_exe_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getOpenFileName(
                    this,
                    tr("Select the CMake executable"),
                    ui->le_cmake_exe->text (),
                    tr("All Files (*.*)"));
        if (fileName.isEmpty ()) break;
        ui->le_cmake_exe->setText (fileName);
        break;
    }
}

void SettingsDlg::on_btn_git_exe_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getOpenFileName(
                    this,
                    tr("Select the git executable"),
                    ui->le_git_exe->text (),
                    tr("All Files (*.*)"));
        if (fileName.isEmpty ()) break;
        ui->le_git_exe->setText (fileName);
        break;
    }
}

void SettingsDlg::on_btn_template_path_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getExistingDirectory (
                    this,
                    tr("Select the template directory"),
                    ui->le_template_path->text ());
        if (fileName.isEmpty ()) break;
        ui->le_template_path->setText (fileName);
        break;
    }
}

void SettingsDlg::on_btn_output_path_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getExistingDirectory (
                    this,
                    tr("Select the output directory"),
                    ui->le_output_path->text ());
        if (fileName.isEmpty ()) break;
        ui->le_output_path->setText (fileName);
        break;
    }
}

void SettingsDlg::on_btn_backup_path_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getExistingDirectory (
                    this,
                    tr("Select the output directory"),
                    ui->le_backup_path->text ());
        if (fileName.isEmpty ()) break;
        ui->le_backup_path->setText (fileName);
        break;
    }
}
