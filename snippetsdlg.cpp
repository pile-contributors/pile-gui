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

QDomDocument createNewXml()
{
    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
                "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild (header);

    QDomElement root = document.createElement("snippets");
    document.appendChild(root);

    return document;
}


SnippetsDlg::SnippetsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnippetsDlg),
    model_(NULL)
{
    QSettings stg;
    ui->setupUi(this);

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
            QTextStream txs (&file);

            txs << "<?xml version='1.0' encoding='UTF-8'?>\n"
                << "<snippets>\n"
                << "</snippets>\n";
            file.close();
        }
    }
    ui->database_path->setText (s_path);

    loadXMLFile (s_path);
}

SnippetsDlg::~SnippetsDlg()
{
    delete ui;
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

        QSettings stg;
        stg.setValue (STG_SNIPP_DB, s_file);

        // open the file
        QFile file (s_file);
        if (!file.open(QIODevice::ReadOnly)) {
            PilesGui::showError (
                        tr("Failed to open file %1\n%2")
                        .arg(s_file)
                        .arg(file.errorString ()));
            break;
        }

        // get the tree parsed
        QDomDocument document;
        if (!document.setContent (&file)) {
            PilesGui::showError (
                        tr("The document %1 is not a valid XML file")
                        .arg(s_file));
            break;
        }

        // build the model
        SnipModel *newModel = new SnipModel (document, this);
        ui->tv_content->setModel (newModel);
        if (model_ != NULL) delete model_;
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
        model_->domDocument().save (txs, 2);

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

        QModelIndex msel = ui->tv_content->currentIndex ();

        if (msel.isValid ()) {
            model_->AddGroup (msel, tr("New group"));
            break;
        }

        model_->AddRootGroup (tr("New group"));
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

        QModelIndex msel = ui->tv_content->currentIndex ();
        if (msel.isValid ()) {
            model_->AddItm (msel, tr("New snippet"));
        } else {
            PilesGui::showError (tr ("Please select the location, first"));
        }
        break;
    }

}

void SnippetsDlg::on_actionRemove_triggered()
{
    QModelIndex msel = ui->tv_content->currentIndex ();
    if (msel.isValid ()) {

    } else {
        PilesGui::showError (tr ("Nothing selected"));
    }
}

void SnippetsDlg::on_actionSave_database_triggered()
{
    saveXMLFile (ui->database_path->text());
}

void SnippetsDlg::on_actionReload_database_triggered()
{
    loadXMLFile (ui->database_path->text());
}
