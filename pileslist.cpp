#include "pileslist.h"
#include "ui_pileslist.h"
#include "pilesgui.h"
#include "logic/pile.h"
#include "logic/pilecontainer.h"
#include "logic/pileprovider.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QIcon>


PilesList::PilesList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PilesList)
{
    ui->setupUi (this);
    PileContainer * pc = PilesGui::pileCont();
    foreach(PileProvider * prov, pc->providers()) {
        insertProvider (prov);
    }
}

PilesList::~PilesList()
{
    delete ui;
}

QTreeWidgetItem * PilesList::insertProvider (
        PileProvider * prov)
{
    QStringList sl;
    sl.append (prov->toString());
    QTreeWidgetItem * tvi = new QTreeWidgetItem (sl);
    switch (prov->providerKind ()) {
    case PROV_TXT: {
        tvi->setIcon (
                    0,
                    QIcon(
                        ":/resources/icons/text.png"));
        break;}
    case PROV_XML: {
        tvi->setIcon (
                    0,
                    QIcon(
                        ":/resources/icons/xml_imports.png"));
        break;}
    case PROV_REMOTE: {
        tvi->setIcon (
                    0,
                    QIcon(
                        ":/resources/icons/link.png"));
        break;}
    case PROV_DIR: {
        tvi->setIcon (
                    0,
                    QIcon(
                        ":/resources/icons/folder.png"));
        break;}
    case PROV_USER: {
        tvi->setIcon (
                    0,
                    QIcon(
                        ":/resources/icons/user_red.png"));
        break;}
    default: {
        // TODO debug assert
        break;}
    }

    ui->treeWidget->addTopLevelItem (tvi);

    return tvi;
}


void PilesList::loadTextFile (const QString & fileName)
{
    PileContainer * container = PilesGui::pileCont();
    PileProviderText * ret = NULL;
    ret = new PileProviderText (fileName);
    if (ret->rescan()) {
        container->addProvider (ret);
        insertProvider (ret);
    } else {
        PilesGui::showError (tr("Could not load file"));
        delete ret;
    }
}

void PilesList::loadDirectory (const QString & fileName)
{
    PileContainer * container = PilesGui::pileCont();
    PileProviderDir * ret = NULL;
    ret = new PileProviderDir (fileName);
    if (ret->rescan()) {
        container->addProvider (ret);
        insertProvider (ret);
    } else {
        PilesGui::showError (tr("Could not load directory"));
        delete ret;
    }
}

void PilesList::loadXMLFile (const QString & fileName)
{
    PileContainer * container = PilesGui::pileCont();
    PileProviderXml * ret = NULL;
    ret = new PileProviderXml (fileName);
    if (ret->rescan()) {
        container->addProvider (ret);
        insertProvider (ret);
    } else {
        PilesGui::showError (tr("Could not load file"));
        delete ret;
    }
}

void PilesList::on_actionAdd_source_text_triggered()
{
    for (;;) {
        QString fileName =
                QFileDialog::getOpenFileName(
                    this,
                    tr("Import piles from a text file"),
                    QString(),
                    tr("Text Files (*.txt)"));
        if (fileName.isEmpty ()) break;
        loadTextFile (fileName);
        break;
    }
}

void PilesList::on_actionAdd_source_directory_triggered()
{
    for (;;) {
        QString fileName =
                QFileDialog::getExistingDirectory (
                    this,
                    tr("Select the directory to import"),
                    QString());
        if (fileName.isEmpty ()) break;
        loadDirectory (fileName);
        break;
    }
}

void PilesList::on_actionAdd_source_XML_File_triggered()
{
    for (;;) {
        QString fileName =
                QFileDialog::getOpenFileName(
                    this,
                    tr("Import piles from an XML file"),
                    QString(),
                    tr("XML Files (*.xml)"));
        if (fileName.isEmpty ()) break;
        loadXMLFile (fileName);
        break;
    }
}

void PilesList::on_actionAdd_source_URL_triggered()
{

    for (;;) {
        QString s_value = QInputDialog::getText (
                    this,
                    tr("Import piles from a remote text or XML file"),
                    tr("The url:"));
        if (s_value.isEmpty ()) break;

        break;
    }
}

void PilesList::on_actionAdd_user_pile_triggered()
{
    for (;;) {
        QString fileName =
                QFileDialog::getExistingDirectory (
                    this,
                    tr("Select the directory that contains the pile's .git directory"),
                    QString());
        if (fileName.isEmpty ()) break;
        loadDirectory (fileName);
        break;
    }
}

void PilesList::on_actionRemove_triggered()
{

}

void PilesList::on_treeWidget_customContextMenuRequested(
        const QPoint & pt)
{
    QMenu mnu;
    mnu.addAction (ui->actionAdd_source_text);
    mnu.addAction (ui->actionAdd_source_XML_File);
    mnu.addAction (ui->actionAdd_source_directory);
    mnu.addAction (ui->actionAdd_source_URL);
    mnu.addAction (ui->actionAdd_source_directory);
    mnu.addAction (ui->actionAdd_user_pile);
    mnu.addSeparator ();
    mnu.addAction (ui->actionRemove);
    mnu.exec (mapToGlobal (pt));
}

