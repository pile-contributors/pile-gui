#include "custcmddlg.h"
#include "ui_custcmddlg.h"
#include "setting_names.h"

#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTreeWidgetItem>
#include <QKeyEvent>

CustCmdDlg::CustCmdDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustCmdDlg)
{
    ui->setupUi(this);
    ui->treeWidget->installEventFilter(this);

    loadSettings ();
}

CustCmdDlg::~CustCmdDlg()
{
    delete ui;
}


void CustCmdDlg::on_b_program_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getOpenFileName(
                    this,
                    tr("Select the executable"),
                    ui->le_program->text (),
                    tr("All Files (*.*)"));
        if (fileName.isEmpty ()) break;
        ui->le_program->setText (fileName);
        break;
    }
}

bool CustCmdDlg::isNewCommandItem (QTreeWidgetItem *it)
{
    return !it->data (0, Qt::UserRole + 5).toBool ();
}

bool CustCmdDlg::loadSettings ()
{
    bool b_ret = false;
    for (;;) {

        QSettings stg;
        int array_index = 0;
        int i_max = stg.beginReadArray (STG_CUSTOM_COMMANDS);
        for (int i = 0; i < i_max; ++i) {
            stg.setArrayIndex (array_index++);

            QStringList sl;
            sl << stg.value (STG_CUSTOM_CM_NAME).toString ()
               << stg.value (STG_CUSTOM_CM_PROG).toString ()
               << stg.value (STG_CUSTOM_CM_ARGS).toString ()
               << stg.value (STG_CUSTOM_CM_CRTDIR).toString ();
            QTreeWidgetItem *current = new QTreeWidgetItem (sl);
            current->setData (0, Qt::UserRole + 5, true);
            ui->treeWidget->insertTopLevelItem (i, current);
        }
        stg.endArray ();

        b_ret = true;
        break;
    }
    return b_ret;
}

bool CustCmdDlg::saveSettings ()
{
    saveToItem (ui->treeWidget->currentItem ());

    QSettings stg;
    bool b_ret = false;
    for (;;) {

        int i_max = ui->treeWidget->topLevelItemCount ();
        stg.beginWriteArray (STG_CUSTOM_COMMANDS, i_max);
        int array_index = 0;
        for (int i = 0; i < i_max; ++i) {
            QTreeWidgetItem *current =
                    ui->treeWidget->topLevelItem (i);
            if (!isNewCommandItem (current)) {
                stg.setArrayIndex (array_index++);
                stg.setValue (STG_CUSTOM_CM_NAME, current->text (0));
                stg.setValue (STG_CUSTOM_CM_PROG, current->text (1));
                stg.setValue (STG_CUSTOM_CM_ARGS, current->text (2));
                stg.setValue (STG_CUSTOM_CM_CRTDIR, current->text (3));
            }
        }
        stg.endArray ();
        b_ret = true;
        break;
    }
    return b_ret;
}

void CustCmdDlg::saveToItem (QTreeWidgetItem * previous)
{
    if (previous != NULL) {
        if (isNewCommandItem (previous)) {
            QStringList sl_content;
            QTreeWidgetItem * new_v =
                    new QTreeWidgetItem (sl_content);
            new_v->setData (0, Qt::UserRole + 5, true);
            ui->treeWidget->insertTopLevelItem (0, new_v);

        } else {
            previous->setText (0, ui->le_custom_name->text ());
            previous->setText (1, ui->le_program->text ());
            previous->setText (2, ui->le_arguments->text ());
            previous->setText (3, ui->le_crtdir->text ());
        }
    }
}

void CustCmdDlg::on_treeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem * previous)
{
    saveToItem (previous);

    bool b = current != NULL;
    if (b) {
        ui->le_custom_name->setText (current->text (0));
        ui->le_program->setText (current->text (1));
        ui->le_arguments->setText (current->text (2));
        ui->le_crtdir->setText (current->text (3));
    } else {
        ui->le_custom_name->clear ();
        ui->le_program->clear ();
        ui->le_arguments->clear ();
        ui->le_crtdir->clear ();
    }

    ui->le_custom_name->setEnabled (b);
    ui->le_arguments->setEnabled (b);
    ui->le_crtdir->setEnabled (b);
    ui->le_program->setEnabled (b);
    ui->b_program->setEnabled (b);
}

void CustCmdDlg::on_b_new_command_clicked()
{
    QStringList sl_content;
    sl_content << "command" << "arguments" << "directory" << "name";
    QTreeWidgetItem * new_v =
            new QTreeWidgetItem (sl_content);
    new_v->setData (0, Qt::UserRole + 5, true);
    ui->treeWidget->insertTopLevelItem (0, new_v);
    ui->treeWidget->setCurrentItem (new_v);
}

bool CustCmdDlg::eventFilter (QObject *target, QEvent *event)
{
    if( event->type() == QEvent::KeyPress ){
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Delete) {
            QTreeWidgetItem * crt = ui->treeWidget->currentItem ();
            if (crt != NULL) {
                int i = ui->treeWidget->indexOfTopLevelItem (crt);
                ui->treeWidget->takeTopLevelItem (i);
                delete crt;
                return true;
            }
        }
    }

    return QDialog::eventFilter (target, event);
}

QTreeWidgetItem * CustCmdDlg::addCommand (const QStringList & sl_content)
{
    QTreeWidgetItem * new_v =
            new QTreeWidgetItem (sl_content);
    new_v->setData (0, Qt::UserRole + 5, true);
    ui->treeWidget->insertTopLevelItem (0, new_v);
    return new_v;
}

void CustCmdDlg::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->buttonRole (button) == QDialogButtonBox::ResetRole) {
        ui->treeWidget->clear ();

        QString s_exe;
        QString s_exe_git = QStandardPaths::findExecutable ("git");
        QString s_exe_git_path;
        if (!s_exe_git.isEmpty ()) {
            QFileInfo fi (s_exe_git);
            QDir abs = fi.dir ();
            if (0 == QString::compare (abs.dirName (), "cmd", Qt::CaseInsensitive)) {
                abs.cdUp ();
            }
            if (0 == QString::compare (abs.dirName (), "bin", Qt::CaseInsensitive)) {
                abs.cdUp ();
            }
            s_exe_git_path = abs.absolutePath ();
        }

        s_exe = QStandardPaths::findExecutable ("wish");
        if (!s_exe.isEmpty () && !s_exe_git_path.isEmpty ()) {
            QStringList sl_content;
            sl_content << "explore git"
                       << QString("\"%1/cmd/gitk.cmd\" -- ").arg(s_exe_git_path)
                       << ""
                       << "${CRT_DIR}";
            addCommand (sl_content);
        }
        if (!s_exe.isEmpty () && !s_exe_git_path.isEmpty ()) {
            QStringList sl_content;
            sl_content << "commit git"
                       << s_exe
                       << QString("\"%1/libexec/git-core/git-gui\" -- ").arg(s_exe_git_path)
                       << "${CRT_DIR}";
            addCommand (sl_content);
        }

    }
}
