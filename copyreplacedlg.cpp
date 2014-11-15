#include "copyreplacedlg.h"
#include "ui_copyreplacedlg.h"
#include "pilesgui.h"
#include "config.h"

#include "support/replacer.h"

#include <QDir>
#include <QSettings>
#include <QFileDialog>
#include <QStack>
#include <QKeyEvent>

CopyReplaceDlg::CopyReplaceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyReplaceDlg),
    editor_lock_(false)
{
    ui->setupUi(this);
    ui->tv_filter->installEventFilter(this);
    ui->tv_replace->installEventFilter(this);
}

CopyReplaceDlg::~CopyReplaceDlg()
{
    delete ui;
}

void CopyReplaceDlg::setSuggestions (const QStringList & sl)
{
    ui->le_source->clear ();
    ui->le_source->addItems (sl);
    ui->le_destination->clear ();
    ui->le_destination->addItems (sl);
}

void CopyReplaceDlg::setSource (const QString & s_value)
{
    ui->le_source->setCurrentText (s_value);
}

void CopyReplaceDlg::setDestination (const QString & s_value)
{
    ui->le_destination->setCurrentText (s_value);
}

void CopyReplaceDlg::on_b_source_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getExistingDirectory (
                    this,
                    tr("Select the source directory"),
                    ui->le_source->currentText ());
        if (fileName.isEmpty ()) break;
        ui->le_source->setCurrentText (fileName);
        break;
    }
}

void CopyReplaceDlg::on_b_destination_clicked()
{
    for (;;) {
        QString fileName =
                QFileDialog::getExistingDirectory (
                    this,
                    tr("Select the destination directory"),
                    ui->le_destination->currentText ());
        if (fileName.isEmpty ()) break;
        ui->le_destination->setCurrentText (fileName);
        break;
    }
}

bool CopyReplaceDlg::isNewCommandEntry (QTreeWidgetItem * tvi)
{
    return !tvi->icon (0).isNull ();
}

bool CopyReplaceDlg::perform()
{
    bool b_ret = false;
    for (;;) {

        QString s_source = ui->le_source->currentText ().trimmed ();
        if (s_source.isEmpty ()) {
            PilesGui::showError (tr("A source must be provided"));
            break;
        }

        QString s_dest = ui->le_destination->currentText ().trimmed ();
        if (s_dest.isEmpty ()) {
            PilesGui::showError (tr("A destination must be provided"));
            break;
        }

        int i_max;

        QStringList sl_find;
        QStringList sl_replace;
        i_max = ui->tv_replace->topLevelItemCount ();
        for (int i = 0; i < i_max; ++i) {
            QTreeWidgetItem * current =
                    ui->tv_replace->topLevelItem (i);
            if (isNewCommandEntry (current)) continue;

            sl_find.append (current->text (0));
            sl_replace.append (current->text (1));
        }

        bool b_indep =
                ui->ck_case_independent->checkState () ==
                Qt::Checked;

        bool b_recursive =
                ui->ck_recursive->checkState () ==
                Qt::Checked;

        QStringList sl_filter;
        i_max = ui->tv_filter->topLevelItemCount ();
        for (int i = 0; i < i_max; ++i) {
            QTreeWidgetItem * current =
                    ui->tv_filter->topLevelItem (i);
            if (isNewCommandEntry (current)) continue;

            sl_filter.append (current->text (0));
        }

        b_ret = Replacer::inFiles (
                    s_source,
                    s_dest,
                    b_recursive,
                    sl_find,
                    sl_replace,
                    b_indep ? Qt::CaseInsensitive : Qt::CaseSensitive,
                    sl_filter);

        break;
    }

    return b_ret;
}

void CopyReplaceDlg::on_tv_replace_itemChanged(
        QTreeWidgetItem *item, int )
{
    if (editor_lock_) return;
    editor_lock_ = true;
    if (isNewCommandEntry (item)) {
        QStringList sl;
        sl << item->text (0) << item->text (1);
        item->setText (0, tr("New item"));
        item->setText (1, tr("New item"));
        QTreeWidgetItem * tvi = new QTreeWidgetItem (sl);
        tvi->setFlags (
                    Qt::ItemIsEnabled |
                    Qt::ItemIsEditable |
                    Qt::ItemIsSelectable);
        ui->tv_replace->insertTopLevelItem (1, tvi);
        ui->tv_replace->setCurrentItem (tvi);
    }
    editor_lock_ = false;
}

void CopyReplaceDlg::on_tv_filter_itemChanged(
        QTreeWidgetItem *item, int )
{
    if (editor_lock_) return;
    editor_lock_ = true;
    if (isNewCommandEntry (item)) {
        QStringList sl;
        sl << item->text (0);
        item->setText (0, tr("New item"));
        QTreeWidgetItem * tvi = new QTreeWidgetItem (sl);
        tvi->setFlags (
                    Qt::ItemIsEnabled |
                    Qt::ItemIsEditable |
                    Qt::ItemIsSelectable);
        ui->tv_filter->insertTopLevelItem (1, tvi);
        ui->tv_filter->setCurrentItem (tvi);
    }
    editor_lock_ = false;
}

bool CopyReplaceDlg::eventFilter (QObject *target, QEvent *event)
{
    QTreeWidget * tv = qobject_cast<QTreeWidget*>(target);
    if (tv != NULL) {
        if( event->type() == QEvent::KeyPress ){
            QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Delete) {

                QTreeWidgetItem * crt = tv->currentItem ();
                if (crt != NULL) {
                    if (!isNewCommandEntry(crt)) {
                        int i = tv->indexOfTopLevelItem (crt);
                        tv->takeTopLevelItem (i);
                        delete crt;
                        return true;
                    }
                }
            }
        }
    }
    return QDialog::eventFilter (target, event);
}
