#include "envdlg.h"
#include "ui_envdlg.h"
#include "setting_names.h"
#include "config.h"
#include "pilesgui.h"

#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QMenu>
#include <QMessageBox>
#include <QProcessEnvironment>

class EnvItem : public QTreeWidgetItem
{
public:
    EnvItem() : QTreeWidgetItem(), is_new_(false) {
        setFlags (Qt::ItemIsSelectable |
                  Qt::ItemIsEnabled);
    }

    virtual ~EnvItem() {
        QTreeWidgetItem * tvi;
        int i_max = childCount ();
        for (int i = 0; i < i_max; ++i) {
            tvi = takeChild (0);
            delete tvi;
        }
    }

    void
    setName (
            const QString & s_value) {
        setText (0, s_value);
    }

    QString
    name () {
        return text (0);
    }

    void
    setValue (
            const QString & s_value) {
        setText (1, s_value);
    }

    QString
    value () {
        return text (1);
    }

    bool
    newlyCreated () {
        return is_new_;
    }

    void
    setNewlyCreated (
            bool value) {
        is_new_ = value;
    }

    bool is_new_;
};



EnvDlg::EnvDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnvDlg),
    to_delete_()
{
    ui->setupUi(this);


    QSettings stg;

    QString s_path = stg.value (STG_ENVAR_DB).toString ();
    if (s_path.isEmpty ()) {
        s_path = QStandardPaths::standardLocations (
                    QStandardPaths::DataLocation).at (0);
        QDir d(s_path);
        if (!d.exists ()) {
            d.mkpath (".");
        }
        s_path = d.absoluteFilePath ("environment.xml");
        QFile file (s_path);
        if (!file.exists ()) {
            if (file.open (QIODevice::WriteOnly)) {
                QTextStream txs (&file);

                txs << "<?xml version='1.0' encoding='UTF-8'?>\n"
                    << "<environment>\n"
                    << "</environment>\n";
                file.close();
            }
        }
    }
    ui->database_path->setText (s_path);

    QSettings s;

    restoreGeometry (
                s.value(
                    STG_ENV_STATE).toByteArray());
    ui->tv_content->header()->restoreState (
                s.value(
                    STG_ENV_TV_STATE).toByteArray());

    loadXMLFile (s_path);
}

EnvDlg::~EnvDlg()
{
    clearAll ();
    clearToDelete ();


    delete ui;
}

void EnvDlg::clearToDelete()
{
    foreach(EnvItem * item, to_delete_) {
        delete item;
    }
    to_delete_.clear ();
}

void EnvDlg::loadEnvironment()
{
    QProcessEnvironment envp =
            PilesGui::processEnvironment ();

    foreach(const QString & s_name, envp.keys ()) {
        EnvItem * itm = new EnvItem ();
        itm->setName (s_name);
        itm->setValue (envp.value (s_name));
        ui->tv_content->addTopLevelItem (itm);
    }
}

void EnvDlg::clearAll()
{
    int i_max = ui->tv_content->topLevelItemCount ();
    for (int i = 0; i < i_max; ++i) {
        QTreeWidgetItem * tvi = ui->tv_content->takeTopLevelItem (0);
        delete tvi;
    }
}

bool EnvDlg::hasName (const QString & value)
{
    int i_max = ui->tv_content->topLevelItemCount ();
    for (int i = 0; i < i_max; ++i) {
        QTreeWidgetItem * tvi = ui->tv_content->topLevelItem (1);
        EnvItem * envv = static_cast<EnvItem *>(tvi);
        if (0 == QString::compare (
                    envv->name (),
                    value,
                    Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

void EnvDlg::on_database_browse_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                this, tr("Open Database"),
                ui->database_path->text (), tr("XML files (*.xml)"));

    if (!filePath.isEmpty()) {
        ui->database_path->setText (filePath);
        loadXMLFile (ui->database_path->text());
    }
}

bool EnvDlg::loadXMLFile(const QString & s_file)
{
    bool b_ret = false;
    for (;;) {
        clearAll ();
        loadEnvironment ();



        b_ret = true;
        break;
    }

    return b_ret;
}

bool EnvDlg::saveXMLFile(const QString & s_file)
{
    bool b_ret = false;
    for (;;) {

        QTreeWidgetItem * tvi = ui->tv_content->currentItem ();
        on_tv_content_currentItemChanged (tvi, tvi);


        b_ret = true;
        break;
    }

    return b_ret;
}

void EnvDlg::on_database_path_returnPressed()
{
    loadXMLFile (ui->database_path->text());
}

void EnvDlg::on_tv_content_customContextMenuRequested(
        const QPoint &pos)
{
    QMenu mnu;
    mnu.addAction (ui->actionCreate_new_variable);
    mnu.addAction (ui->actionRemove_variable);
    mnu.addSeparator ();
    mnu.addAction (ui->actionReload_database);
    mnu.addAction (ui->actionSave_database);

    mnu.exec (mapToGlobal (pos));
}

void EnvDlg::on_actionSave_database_triggered()
{
    saveXMLFile (ui->database_path->text());
}

void EnvDlg::on_actionReload_database_triggered()
{
    int res = QMessageBox::question (
                this, tr("Reload content"),
                tr("You are about to reload the content "
                   "and discard all changes made to this file\n\n"
                   "Are you sure?"),
                QMessageBox::Yes, QMessageBox::No);
    if (res == QMessageBox::No) return;
    loadXMLFile (ui->database_path->text());
}

void EnvDlg::on_tv_content_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (previous != NULL) {
        EnvItem * sp = static_cast<EnvItem*>(previous);
        QString s_value = ui->te_value->toPlainText ();
        s_value.replace ("\n", ";");
        sp->setValue (s_value);
        sp->setName (ui->variable_name->text ());
    }

    if (current == NULL) {
        ui->variable_name->clear ();
        ui->te_value->clear ();
        ui->variable_name->setEnabled (false);
        ui->te_value->setEnabled (false);

    } else {
        EnvItem * sp = static_cast<EnvItem*>(current);

        QString s_value = sp->value ();
        s_value.replace (";", "\n");
        ui->te_value->setPlainText (s_value);
        ui->te_value->setEnabled (true);
        ui->variable_name->setText (sp->name());
        ui->variable_name->setEnabled (true);
    }
}

void EnvDlg::closeEvent(QCloseEvent *)
{

    saveXMLFile (ui->database_path->text ());

    QSettings s;
    s.setValue (STG_ENV_GEOMETRY, saveGeometry ());
    s.setValue (STG_ENV_TV_STATE, ui->tv_content->header()->saveState ());
}

void EnvDlg::on_actionCreate_new_variable_triggered()
{
    EnvItem * itm = new EnvItem ();
    itm->setName (tr("NEW_VARIABLE"));
    itm->setValue (tr("VALUE"));
    itm->setNewlyCreated (true);
    ui->tv_content->addTopLevelItem (itm);

    ui->tv_content->scrollToItem (itm);
}

void EnvDlg::on_actionRemove_variable_triggered()
{
    QTreeWidgetItem * tvi = ui->tv_content->currentItem ();
    if (tvi == NULL) return;
    int i = ui->tv_content->indexOfTopLevelItem (tvi);
    ui->tv_content->takeTopLevelItem (i);

    EnvItem * sp = static_cast<EnvItem*>(tvi);
    to_delete_.append (sp);
}

void EnvDlg::on_buttonBox_accepted()
{
    //    QProcessEnvironment envp =
    //            PilesGui::processEnvironment ();
    QProcessEnvironment env;

    int i_max = ui->tv_content->topLevelItemCount ();
    for (int i = 0; i < i_max; ++i) {
        QTreeWidgetItem * tvi = ui->tv_content->topLevelItem (i);
        EnvItem * envv = static_cast<EnvItem *>(tvi);
        env.insert (envv->name (), envv->value ());
    }
    PilesGui::setProcessEnvironment (env);

    clearToDelete ();

    accept();
}
