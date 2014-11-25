#include "newpiledlg.h"
#include "ui_newpiledlg.h"
#include "setting_names.h"
#include "pilesgui.h"
#include "config.h"
#include "mainwindow.h"

#include "logic/pile.h"

#include <QDir>
#include <QSettings>
#include <QFileDialog>
#include <QStack>


enum UserDataTree {
    UD_SOURCE = Qt::UserRole + 1,
    UD_ORIGINAL_NAME
};

NewPileDlg::NewPileDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPileDlg)
{
    ui->setupUi(this);

    QSettings stg;

    ui->le_template_path->setText (
                stg.value (STG_TEMPLATE_PATH).toString ());
    ui->le_output_path->setText (
                stg.value (STG_OUTPUT_PATH).toString ());

    ui->tv_expected_content->setEnabled (true);
}

NewPileDlg::~NewPileDlg()
{
    delete ui;
}

void NewPileDlg::on_btn_template_path_clicked()
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

void NewPileDlg::on_btn_output_path_clicked()
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

static void createPileStatus (const QString & s_message)
{
    PilesGui::mainWindow ()->showStatusMessage (s_message);
}

bool NewPileDlg::doCreatePile()
{
    bool b_ret = false;

    QString s_name = ui->le_pile_name->text ().trimmed ();
    //QString s_output_path;
    QString s_path = ui->le_output_path->text ().trimmed ();
    QString s_template = ui->le_template_path->text ().trimmed ();
    QString s_error;
    for (;;){

        if (s_name.isEmpty ()) {
            s_error.append (tr("A pile name must be provided\n"));
        } else if (s_name.contains (' ')) {
            s_error.append (tr("A pile name can't contain white spacesd\n"));
        }
#if 0
        if (s_path.isEmpty ()) {
            s_error.append (tr("A pile output path must be provided\n"));
        } else {
            QDir d_path (s_path);
            if (!d_path.exists ()) {
                if (!d_path.mkpath (".")) {
                    s_error.append (
                                tr(
                                    "Output path does not exists "
                                    "and could not be created\n"));
                    break;
                }
            }

            if (d_path.exists (s_name)) {
                s_error.append (
                            tr(
                                "A file or directory with the same name "
                                "already exists in output directory\n"));
            } else {
                s_output_path = d_path.absoluteFilePath (s_name);
            }
        }

        if (s_template.isEmpty ()) {
            s_error.append (tr("A pile template must be provided\n"));
        } else {
            QDir d_path (s_template);
            if (!d_path.exists ()) {
                s_error.append (
                            tr("Template path does not exists\n"));
                break;
            } else {
                s_template = d_path.absolutePath ();
            }
        }
#endif
        if (!s_error.isEmpty ()) break;

        QStringList sl_errors;
        b_ret = Pile::createAllNew (
                    s_name,
                    s_path,
                    s_template,
                    sl_errors,
                    createPileStatus);
        if (!b_ret) {
            s_error = sl_errors.join ("\n");
        }
        break;
    }

    if (!s_error.isEmpty ()) {
        PilesGui::showError (s_error);
    }

    return b_ret;
}

QTreeWidgetItem * NewPileDlg::createItem (
        const QString & s_name, const QString & s_source,
        QTreeWidgetItem * crt_parent, bool b_file)
{
    QStringList col_values;
    col_values << s_name << s_source;

    QTreeWidgetItem * crt_item;
    crt_item = new QTreeWidgetItem (col_values);
    crt_item->setFlags (
                Qt::ItemIsUserCheckable |
                Qt::ItemIsSelectable |
                Qt::ItemIsEnabled);

    if (b_file) {
        QIcon icn (":/icons/resources/icons/text.png");
        DBG_ASSERT (!icn.isNull ());
        crt_item->setIcon (0, icn);
        crt_item->setIcon (1, icn);
        crt_item->setIcon (2, icn);
    } else {
        QIcon icn (":/icons/resources/icons/folder.png");
        DBG_ASSERT (!icn.isNull ());
        crt_item->setIcon (0, icn);
    }

    crt_item->setCheckState(0, Qt::Checked);

    if (crt_parent == NULL) {
        ui->tv_expected_content->addTopLevelItem (crt_item);
    } else {
        crt_parent->addChild (crt_item);
    }

    return crt_item;
}

void NewPileDlg::addItemToTree (
        const QString & s_value,
        const QString & s_name, bool b_file)
{
    QString s_repl = s_value;
    s_repl.replace ("TemPile", s_name, Qt::CaseSensitive);
    s_repl.replace ("tempile", s_name.toLower (), Qt::CaseSensitive);
    s_repl.replace ("TEMPILE", s_name.toUpper (), Qt::CaseSensitive);

    QStringList sl_comps = s_repl.split ("/");
    QTreeWidgetItem * crt_parent = NULL;
    QTreeWidgetItem * prev_parent = NULL;
    QTreeWidgetItem * crt_item = NULL;
    int i, i_max;
    int comp_i = 0;
    int comp_i_last = sl_comps.count () - 1;
    foreach(const QString & s, sl_comps) {

        if (comp_i == comp_i_last) {
            crt_item = createItem (s, s_value, crt_parent, b_file);
            break;
        }

        if (crt_parent == NULL) {
            i_max = ui->tv_expected_content->topLevelItemCount ();
            for (i = 0; i < i_max; ++i) {
                crt_item = ui->tv_expected_content->topLevelItem (i);
                if (crt_item->text (0) == s) {
                    crt_parent = crt_item;
                    break;
                }
            }
            if (crt_parent == NULL) {
                crt_item = createItem (s, s_value, crt_parent, b_file);
                crt_parent = crt_item;
            }
        } else {
            i_max = crt_parent->childCount ();
            prev_parent = NULL;
            for (i = 0; i < i_max; ++i) {
                crt_item = crt_parent->child (i);
                if (crt_item->text (0) == s) {
                    prev_parent = crt_item;
                    break;
                }
            }
            if (prev_parent == NULL) {
                crt_item = createItem (s, s_value, crt_parent, b_file);
                prev_parent = crt_item;
            }
            crt_parent = prev_parent;
        }
        ++comp_i;
    }
}

void NewPileDlg::reloadGenContent()
{
    ui->tv_expected_content->clear ();

    QString s_name = ui->le_pile_name->text ().trimmed ();
    QString s_template = ui->le_template_path->text ().trimmed ();

    if (s_name.isEmpty ()) return;
    if (s_template.isEmpty ()) return;

    QDir d_tmpl (s_template);
    if (!d_tmpl.exists ()) return;

    QStack<QString> dirs;
    dirs.push (s_template);

    QStringList filters;
    filters << "*.*";

    while (dirs.size() > 0) {
        QDir currentDir (dirs.pop());
        QString s_relative;

        QStringList child = currentDir.entryList (
                    filters, QDir::NoDotAndDotDot | QDir::Files);

        QStringList::const_iterator itbegin = child.begin();
        QStringList::const_iterator itend = child.end();
        for(; itbegin != itend ; ++itbegin) {
            if (*itbegin == ".git") continue;
            const QString nextFile (currentDir.path() + "/" + (*itbegin));
            s_relative = d_tmpl.relativeFilePath (nextFile);
            addItemToTree (s_relative, s_name, true);
        }

        QStringList childdir =  currentDir.entryList (
                    QDir::NoDotAndDotDot | QDir::Dirs);
        QStringList::const_iterator itbegindir = childdir.begin();
        QStringList::const_iterator itenddir = childdir.end();
        for(; itbegindir != itenddir ; ++itbegindir){
            if (*itbegindir == ".git") continue;
            if (*itbegindir == ".svn") continue;

            const QString nextFile (
                        currentDir.path() + "/" + (*itbegindir));
            s_relative = d_tmpl.relativeFilePath (nextFile);

            dirs.push (nextFile);
            addItemToTree (s_relative, s_name, false);
        }
    }

    ui->tv_expected_content->expandAll ();
}

void NewPileDlg::on_le_template_path_editingFinished()
{
    reloadGenContent();
}

void NewPileDlg::on_le_pile_name_editingFinished()
{
    reloadGenContent();
}

