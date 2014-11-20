#include "snippetsdlg.h"
#include "ui_snippetsdlg.h"
#include "setting_names.h"
#include "config.h"
#include "pilesgui.h"

#include "snippets/snipitem.h"
#include "snippets/snipmodel.h"

#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QMenu>
#include <QMessageBox>


QDomDocument createNewXml()
{
    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
                "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild (header);

    QDomElement root = document.createElement ("snippets");
    document.appendChild(root);

    return document;
}

SnippetsDlg::SnippetsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnippetsDlg),
    model_(NULL)
{

    ui->setupUi(this);
    model_ = new SnipModel (ui->tv_content);

    QSettings stg;

    QString s_path = stg.value (STG_SNIPP_DB).toString ();
    if (s_path.isEmpty ()) {
        s_path = QStandardPaths::standardLocations (
                    QStandardPaths::DataLocation).at (0);
        QDir d(s_path);
        if (!d.exists ()) {
            d.mkpath (".");
        }
        s_path = d.absoluteFilePath ("snippets.xml");
        QFile file (s_path);
        if (!file.exists ()) {
            if (file.open (QIODevice::WriteOnly)) {
                QTextStream txs (&file);

                txs << "<?xml version='1.0' encoding='UTF-8'?>\n"
                    << "<snippets>\n"
                    << "</snippets>\n";
                file.close();
            }
        }
    }
    ui->database_path->setText (s_path);

    QSettings s;

    restoreGeometry (
                s.value(
                    STG_SNIPP_GEOMETRY).toByteArray());
    ui->tv_content->header()->restoreState (
                s.value(
                    STG_SNIPP_TV_STATE).toByteArray());

    on_tv_content_currentItemChanged (NULL, NULL);

    loadXMLFile (s_path);
}

SnippetsDlg::~SnippetsDlg()
{
    delete ui;
}

void SnippetsDlg::closeEvent(QCloseEvent *)
{
    saveXMLFile (ui->database_path->text ());

    QSettings s;
    s.setValue (STG_SNIPP_GEOMETRY, saveGeometry ());
    s.setValue (STG_SNIPP_TV_STATE, ui->tv_content->header()->saveState ());
}

void SnippetsDlg::on_database_browse_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                this, tr("Open Database"),
                ui->database_path->text (), tr("XML files (*.xml)"));

    if (!filePath.isEmpty()) {
        ui->database_path->setText (filePath);
        loadXMLFile (ui->database_path->text());
    }
}

void SnippetsDlg::on_database_path_editingFinished()
{
}

bool SnippetsDlg::loadXMLFile(const QString & s_file)
{
    bool b_ret = false;

    for (;;) {


        // open the file
        QFile file (s_file);
        if (!file.open(QIODevice::ReadOnly)) {
            PilesGui::showError (
                        tr("Failed to open file %1\n%2")
                        .arg(s_file)
                        .arg(file.errorString ()));
            break;
        }

        QSettings stg;
        stg.setValue (STG_SNIPP_DB, s_file);

        // get the tree parsed
        QDomDocument document;
        if (!document.setContent (&file)) {
            PilesGui::showError (
                        tr ("The document %1 is not a valid XML file")
                        .arg(s_file));
            break;
        }

        // build the model
        if (model_ != NULL) delete model_;
        model_ = NULL;
        SnipModel *newModel = new SnipModel (ui->tv_content, document);
        model_ = newModel;

        // close the source file
        file.close();

        b_ret = true;
        break;
    }

    return b_ret;
}


bool SnippetsDlg::saveXMLFile (const QString & s_file)
{
    bool b_ret = false;

    for (;;) {

        if (model_ == NULL) {
            PilesGui::showError ("No document to save");
            break;
        }

        QTreeWidgetItem * tvi = ui->tv_content->currentItem ();
        if (tvi != NULL)
            on_tv_content_currentItemChanged (tvi, tvi);

        // open the file
        QFile file (s_file);
        if (!file.open(QIODevice::WriteOnly)) {
            PilesGui::showError (
                        tr("Failed to open file %1\n%2")
                        .arg (s_file)
                        .arg (file.errorString ()));
            break;
        }

        QTextStream txs (&file);
        QDomDocument doc ("snippets");
        model_->saveXML (doc);
        doc.save (txs, 2);

        // close the source file
        file.close();

        b_ret = true;
        break;
    }

    return b_ret;
}

void SnippetsDlg::on_database_path_returnPressed()
{
    loadXMLFile (ui->database_path->text());
}

void SnippetsDlg::on_tv_content_customContextMenuRequested(
        const QPoint &pos)
{
    QMenu mnu;
    mnu.addAction (ui->actionAdd_new_group);
    mnu.addAction (ui->actionAdd_new_snipped);
    mnu.addSeparator ();
    mnu.addAction (ui->actionRemove);
    mnu.addSeparator ();
    mnu.addAction (ui->actionReload_database);
    mnu.addAction (ui->actionSave_database);

    mnu.exec (mapToGlobal (pos));
}

void SnippetsDlg::on_actionAdd_new_group_triggered()
{
    for (;;) {
        if (model_ == NULL) {
            PilesGui::showError ("Please select a file, first");
            break;
        }

        SnipGroup * msel = model_->currentGroup();
        if (msel != NULL) {
            model_->addGroup (tr("New group"), msel);
            break;
        }

        model_->addRootGroup (tr("New group"));
        break;
    }

}

void SnippetsDlg::on_actionAdd_new_snipped_triggered()
{
    for (;;) {
        if (model_ == NULL) {
            PilesGui::showError ("Please select a file, first");
            break;
        }

        SnipGroup * msel = model_->currentGroup();
        if (msel != NULL) {
            model_->addSnip (tr("New snippet"), msel);
        } else {
            PilesGui::showError (tr ("Please select the location, first"));
        }
        break;
    }

}

void SnippetsDlg::on_actionRemove_triggered()
{
    SnipItem * msel = model_->currentItem ();
    model_->removeItem (msel);
}

void SnippetsDlg::on_actionSave_database_triggered()
{
    saveXMLFile (ui->database_path->text());
}

void SnippetsDlg::on_actionReload_database_triggered()
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

void SnippetsDlg::on_tv_content_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    model_->saveToItem(
                static_cast<SnipItem*>(previous),
                ui->le_name->text (),
                ui->le_icon->currentText (),
                ui->tx_content->toPlainText ());

    QString s_name;
    QString s_icon;
    QString s_content;

    model_->getFromItem(
                static_cast<SnipItem*>(current),
                s_name,
                s_icon,
                s_content);
    ui->le_name->setText (s_name),
            ui->le_icon->setCurrentText (s_icon),
            ui->tx_content->setPlainText (s_content);

    if (current == NULL) {
        ui->le_name->setEnabled (false);
        ui->le_icon->setEnabled (false);
        ui->tx_content->setEnabled (false);
        ui->icon_sample->setPixmap (QPixmap());
    } else {
        SnipItem * sp = static_cast<SnipItem*>(current);
        QIcon ic = sp->icon();
        ui->icon_sample->setPixmap (ic.pixmap (16, 16));
        if (sp->isGrup () || (sp->parent() == NULL)) {
            ui->le_name->setEnabled (true);
            ui->le_icon->setEnabled (true);
            ui->tx_content->setEnabled (false);
        } else {
            ui->le_name->setEnabled (true);
            ui->le_icon->setEnabled (true);
            ui->tx_content->setEnabled (true);
        }
    }
}

void SnippetsDlg::on_b_icon_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
                this, tr("Open Icon"),
                ui->le_icon->currentText (), tr("Image files (*.png)"));

    if (!filePath.isEmpty()) {
        ui->le_icon->setCurrentText (filePath);
        QIcon ic (filePath);
        if (ic.isNull ()) {
            ui->icon_sample->setPixmap (QPixmap());
        } else {
            ui->icon_sample->setPixmap (ic.pixmap (16, 16));
        }
    }
}
