#include "config.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setting_names.h"
#include "pilesgui.h"
#include "pileslist.h"
#include "settingsdlg.h"
#include "newpiledlg.h"
#include "snippetsdlg.h"
#include "custcmddlg.h"
#include "copyreplacedlg.h"
#include "envdlg.h"
#include "support/programrunner.h"

#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QClipboard>
#include <QInputDialog>
#include <QToolButton>
#include <QProcess>
#include <QMimeData>


#define NAVIGATE_BACK true
#define NAVIGATE_FORW false
#define GET_NAV_DIR(__act__) __act__->data ().toBool ()
#define MANDATORY_QDIR_FILTER (QDir::AllDirs | QDir::NoDotAndDotDot)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new FsModelEnhanced),
    my_comp_index(),
    forward_menu(new QMenu(this)),
    backward_menu(new QMenu(this)),
    navigation_lock_(false)
{
    ui->setupUi(this);
    model->setRootPath (QDir::rootPath ());



    model->setFilter (MANDATORY_QDIR_FILTER |
                      QDir::Files |
                      QDir::Drives |
                      QDir::NoSymLinks |

                      QDir::Readable |
                      QDir::Writable |
                      QDir::Executable |

                      QDir::Modified |
                      QDir::Hidden |
                      QDir::System);

    ui->treeView->setModel (model);
    ui->treeView->setSortingEnabled (true);
    my_comp_index = ui->treeView->rootIndex ();

#if 0
    connect (model, SIGNAL(fsmeLoadStart()),
             this, SLOT(fsmeLoadStart()));
    connect (model, SIGNAL(fsmeLoadEnd()),
             this, SLOT(fsmeLoadEnd()));
#endif

    connect (ui->treeView->selectionModel(),
             SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
             this,
             SLOT(fileListSelectionChanged(const QModelIndex &, const QModelIndex &)));

    ui->menuToolbars->clear ();
    ui->menuToolbars->addAction (
                ui->mainToolBar->toggleViewAction ());

    loadSettings();

}

void MainWindow::fsmeLoadStart()
{
    ui->treeView->setCursor (Qt::WaitCursor);
}

void MainWindow::fsmeLoadEnd()
{
    ui->treeView->setCursor (Qt::ArrowCursor);
}


void MainWindow::afterShown()
{
    // toolbar must be shown to get position
    if (!ui->mainToolBar->toggleViewAction ()->isChecked ()) {
        ui->mainToolBar->toggleViewAction ()->setChecked (true);
    }

    // set the back/forward menus
    QToolButton* btn_back =
            toolBtnForAction (ui->mainToolBar, ui->actionBack);
    if (btn_back != NULL) {
        btn_back->setMenu (backward_menu);
        connect(ui->actionBack, SIGNAL(triggered()),
                btn_back, SLOT(showMenu()));
    }
    QToolButton* btn_forw =
            toolBtnForAction (ui->mainToolBar, ui->actionForward);
    if (btn_forw != NULL) {
        btn_forw->setMenu (forward_menu);
        connect(ui->actionForward, SIGNAL(triggered()),
                btn_forw, SLOT(showMenu()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QToolButton* MainWindow::toolBtnForAction (QToolBar * toolBar, QAction* action)
{
    QRect actionRect = toolBar->actionGeometry (action);
    QWidget * w = toolBar->childAt (actionRect.center());
    QToolButton *toolButton =
            qobject_cast<QToolButton *>(w);
    return toolButton;
}

void MainWindow::closeEvent (QCloseEvent *)
{
    saveSettings();
}

void MainWindow::fileListSelectionChanged (
        const QModelIndex &, const QModelIndex &)
{
    QString s_crt_path = currentPath ();
    ui->statusBar->showMessage (s_crt_path);
    navigationUser (s_crt_path);
}

void MainWindow::navigationAction ()
{

    QAction * act = qobject_cast<QAction*>(sender ());
    if (act == NULL) return;

    if (navigation_lock_) return;
    navigation_lock_ = true;

    selectPath (act->text ());

    QList<QAction*> all_acts_back = backward_menu->actions ();
    QList<QAction*> all_acts_forw = forward_menu->actions ();
    QAction* insert_before = NULL;

    if (GET_NAV_DIR(act) == NAVIGATE_BACK) {
        if (all_acts_forw.count () > 0) {
            insert_before = all_acts_forw.at (0);
        }
        foreach(QAction * act_iter, all_acts_back) {
            if (act_iter == act) break;

            backward_menu->removeAction (act_iter);
            if (insert_before == NULL) {
                forward_menu->addAction (act_iter);
            } else {
                forward_menu->insertAction (insert_before, act_iter);
            }
            act_iter->setData (NAVIGATE_FORW);
            insert_before = act_iter;
        }
        navigationTrim (forward_menu, forward_menu->actions ());
    } else {
        if (all_acts_back.count () > 0) {
            insert_before = all_acts_back.at (0);
        }
        foreach(QAction * act_iter, all_acts_forw) {

            forward_menu->removeAction (act_iter);
            if (insert_before == NULL) {
                backward_menu->addAction (act_iter);
            } else {
                backward_menu->insertAction (insert_before, act_iter);
            }
            act_iter->setData (NAVIGATE_BACK);
            insert_before = act_iter;

            if (act_iter == act) break;
        }
        navigationTrim (backward_menu, backward_menu->actions ());
    }

    navigation_lock_ = false;
}

void MainWindow::navigationTrim (
        QMenu * mnu, const QList<QAction*> & all_acts)
{
    // get maximum number of paths to keep
    QSettings s;
    int max_to_keep = s.value (STG_NAV_MAX, 20).toInt ();

    // make sure we're inside the bounds
    if (all_acts.count () > max_to_keep) {
        for (int i = all_acts.count () - 1; i >= max_to_keep; --i) {
            mnu->removeAction (all_acts.at (i));
            all_acts.at (i)->deleteLater ();
        }
    }
}

void MainWindow::showStatusMessage (const QString & s_path)
{
    ui->statusBar->showMessage (s_path, SEC_TO_MSEC(40));
}

void MainWindow::navigationUser (const QString & s_path)
{

    if (s_path.isEmpty ()) return;

    if (navigation_lock_) return;
    navigation_lock_ = true;

    for (;;) {

        // clear forward menu
        foreach(QAction * act, forward_menu->actions ()) {
            act->deleteLater ();
        }

        // create a new action for this item
        QAction * new_act = new QAction (s_path, this);
        new_act->setData (NAVIGATE_BACK);
        connect (new_act, SIGNAL(triggered()),
                 this, SLOT(navigationAction()));

        // prepend to backward menu
        QList<QAction*> all_acts = backward_menu->actions ();
        if (all_acts.count () == 0) {
            backward_menu->addAction (new_act);
            break;
        } else {
            // make this one the first one
            backward_menu->insertAction (all_acts.at (0), new_act);
        }

        // make sure we're inside the bounds
        navigationTrim (backward_menu, all_acts);

        break;
    }

    navigation_lock_ = false;

}


void MainWindow::loadSettings ()
{
    QSettings s;
    QString s_crt_path = s.value (STG_CRT_PATH).toString ();
    selectPath (s_crt_path);

    restoreGeometry (
                s.value(
                    STG_MW_GEOMETRY).toByteArray());
    restoreState (
                s.value(
                    STG_MW_STATE).toByteArray());
    ui->treeView->header()->restoreState (
                s.value(
                    STG_TV_STATE).toByteArray());

    QDir::Filters filfiltt = (QDir::Filters) (MANDATORY_QDIR_FILTER | s.value(
                                                  STG_TV_FILTER, (int)model->filter ()).toInt ());
    model->setFilter (filfiltt);

    ui->actionFiles->setChecked ((filfiltt & QDir::Files) != 0);
    ui->actionDrives->setChecked ((filfiltt & QDir::Drives) != 0);
    ui->actionSymbolic_links->setChecked ((filfiltt & QDir::NoSymLinks) != 0);
    ui->actionReadable->setChecked ((filfiltt & QDir::Readable) != 0);
    ui->actionWritable->setChecked ((filfiltt & QDir::Writable) != 0);
    ui->actionExecutable->setChecked ((filfiltt & QDir::Executable) != 0);
    ui->actionModified->setChecked ((filfiltt & QDir::Modified) != 0);
    ui->actionHidden->setChecked ((filfiltt & QDir::Hidden) != 0);
    ui->actionSystem->setChecked ((filfiltt & QDir::System) != 0);
    ui->actionCase_sensitive->setChecked ((filfiltt & QDir::CaseSensitive) != 0);

}

void MainWindow::saveSettings ()
{
    QSettings s;
    s.setValue (STG_CRT_PATH, currentPath());
    s.setValue (STG_MW_GEOMETRY, saveGeometry ());
    s.setValue (STG_MW_STATE, saveState ());

    s.setValue (STG_TV_STATE, ui->treeView->header()->saveState ());
    s.setValue (STG_TV_FILTER, (int)model->filter());

}

QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;
    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i)
    {
        if (program.at(i) == QLatin1Char('"'))
        {
            ++quoteCount;
            if (quoteCount == 3)
            {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount)
        {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace())
        {
            if (!tmp.isEmpty())
            {
                args += tmp;
                tmp.clear();
            }
        }
        else
        {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;
    return args;
}

bool MainWindow::execUserCommand (const QStringList & sl)
{
    QString s_crt_dir = currentDirectory ();
    QString s_crt_file = currentFile ();
    QString s_crt_item = currentPath ();
    QString s_crt_sel;

    foreach(QModelIndex mi, ui->treeView->selectionModel ()->selectedIndexes ()) {
        if (mi.column () == 0) {
            QString s_path = model->filePath (mi);
            if (s_crt_sel.isEmpty ()) {
                s_crt_sel = s_path;
            } else {
                s_crt_sel.append (" ");
                s_crt_sel.append (s_path);
            }
        }
    }

    QString s_app = sl.at (0);
    s_app.replace ("${CRT_DIR}", s_crt_dir);
    s_app.replace ("${CRT_FILE}", s_crt_file);
    s_app.replace ("${CRT_ITEM}", s_crt_item);
    s_app.replace ("${CRT_SEL}", s_crt_sel);

    QString s_args = sl.at (1);
    s_args.replace ("${CRT_DIR}", s_crt_dir);
    s_args.replace ("${CRT_FILE}", s_crt_file);
    s_args.replace ("${CRT_ITEM}", s_crt_item);
    s_args.replace ("${CRT_SEL}", s_crt_sel);

    QString s_dir = sl.at (2);
    s_dir.replace ("${CRT_DIR}", s_crt_dir);
    s_dir.replace ("${CRT_FILE}", s_crt_file);
    s_dir.replace ("${CRT_ITEM}", s_crt_item);
    s_dir.replace ("${CRT_SEL}", s_crt_sel);

    QStringList arguments = parseCombinedArgString (s_args);

    QProcess *myProcess = new QProcess ();
    myProcess->setProgram (s_app);
    myProcess->setArguments (arguments);

    //connect(myProcess, SIGNAL(readyReadStdout()),
    //        this, SLOT(readFromStdout()) );
    connect(myProcess, SIGNAL(processExited()),
            myProcess, SLOT(deleteLater()) );
    myProcess->setWorkingDirectory (s_dir);

    myProcess->start();
    //    PilesGui::showError (
    //                QObject::tr("Could not start %1")
    //                .arg(s_app));
    return true;
}

bool MainWindow::selectPath (const QString & s_path, bool b_verbose)
{
    if (!s_path.isEmpty ()) {
        QModelIndex mx = model->index (s_path);
        if (mx.isValid ()) {
            ui->treeView->setCurrentIndex (mx);
            ui->treeView->scrollTo (mx);
            return true;
        }
    }

    if (b_verbose) {
        PilesGui::showError (
                    tr("Could not find path %1")
                    .arg(s_path));
    }
    return false;
}

QStringList MainWindow::favouritePaths () const
{
    QSettings stg;
    QStringList sl_fav =
            stg.value (STG_FAVOURITES_PATHS)
            .toStringList ();
    return sl_fav;
}


void MainWindow::addToFavouritePaths (
        const QString & s_value)
{
    QSettings stg;
    QStringList sl_fav =
            stg.value (STG_FAVOURITES_PATHS)
            .toStringList ();
    sl_fav.prepend (s_value);
    sl_fav.removeDuplicates ();
    stg.setValue (STG_FAVOURITES_PATHS, sl_fav);
}

void MainWindow::setFavouritePaths (
        const QStringList & sl)
{
    QSettings stg;
    stg.setValue (STG_FAVOURITES_PATHS, sl);
}

QString MainWindow::currentDirectory ()
{
    QString s_ret;
    for (;;) {
        QModelIndex mx = ui->treeView->currentIndex ();
        if (!mx.isValid ()) break;
        s_ret = model->filePath (mx);
        if (s_ret.isEmpty ()) break;
        QFileInfo  fi(s_ret);
        if (fi.isDir ()) break;
        if (!fi.isFile ()) {
            s_ret.clear ();
            break;
        }
        s_ret = fi.path ();
        break;
    }
    return s_ret;
}

QString MainWindow::currentFile ()
{
    QString s_ret;
    for (;;) {
        QModelIndex mx = ui->treeView->currentIndex ();
        if (!mx.isValid ()) break;
        s_ret = model->filePath (mx);
        if (s_ret.isEmpty ()) break;
        QFileInfo fi(s_ret);
        if (fi.isDir ()) {
            s_ret.clear ();
            break;
        }
        if (!fi.isFile ()) {
            s_ret.clear ();
            break;
        }
        break;
    }
    return s_ret;
}

QString MainWindow::currentPath ()
{
    QString s_ret;
    for (;;) {
        QModelIndex mx = ui->treeView->currentIndex ();
        if (!mx.isValid ()) break;
        s_ret = model->filePath (mx);
        break;
    }
    return s_ret;
}

void MainWindow::on_pushButton_clicked()
{
    QString directory = currentDirectory ();
    if (directory.isEmpty ()) {
        PilesGui::showError (tr("No selection"));
    } else {
        QDesktopServices::openUrl(
                    QUrl (
                        QString("file:///%1")
                        .arg(directory)));
    }
}

void MainWindow::on_btn_open_clicked()
{
    QString selected_entry = currentFile ();
    if (selected_entry.isEmpty ()) {
        PilesGui::showError (tr("No file selected"));
    } else {
        ProgramRunner::startProgramOrOpenFile (selected_entry);
    }
}

void MainWindow::on_btn_new_pile_clicked()
{
    NewPileDlg dlg;
    while (dlg.exec () == QDialog::Accepted) {
        if (dlg.doCreatePile ()) break;
    }
}

void MainWindow::on_btn_add_pile_clicked()
{
    PilesList dlg;
    dlg.exec ();
}

void MainWindow::on_actionSet_root_here_triggered()
{
    QString scrt = currentDirectory ();
    if (scrt.isEmpty ()) {
        PilesGui::showError (tr("No directory selected"));
    } else {
        QModelIndex mi = model->setRootPath (scrt);
        if (!mi.isValid ()) {
            PilesGui::showError (
                        tr("Unable to set root path to\n%1")
                        .arg(scrt));
        } else {
            ui->treeView->setRootIndex (mi);
        }
    }
}

void MainWindow::on_actionClear_virtual_root_triggered()
{
    /*QModelIndex mi =*/ model->setRootPath (QDir::rootPath ());
    ui->treeView->setRootIndex (my_comp_index);
}

void MainWindow::on_actionAdd_to_favorites_triggered()
{
    QString scrt = currentPath ();
    if (scrt.isEmpty ()) {
        PilesGui::showError (tr("Nothing selected"));
        return;
    }
    addToFavouritePaths (scrt);
}

void MainWindow::on_actionGo_to_path_triggered()
{
    QString s_path;
    s_path = QInputDialog::getText (
                this, tr("Select path"),
                tr("Please enter the path to select:"));
    if (s_path.isEmpty ()) {
        return;
    }
    selectPath (s_path, true);
}

void MainWindow::on_actionGo_to_clipboard_path_triggered()
{
    QString s_path;
    QClipboard *clipboard = QApplication::clipboard();

    s_path = clipboard->text (QClipboard::Clipboard);
    if (s_path.isEmpty ()) {
        s_path = clipboard->text (QClipboard::Selection);
        if (s_path.isEmpty ()) {
            PilesGui::showError (tr("Nothing in clipboard"));
            return;
        }
    }

    selectPath (s_path, true);
}

void MainWindow::on_treeView_customContextMenuRequested(
        const QPoint &pos)
{
    QSettings stg;

    QMenu mnu;
    mnu.addAction (ui->actionSet_root_here);
    mnu.addAction (ui->actionClear_virtual_root);
    mnu.addSeparator ();
    mnu.addAction (ui->actionAdd_to_favorites);
    mnu.addSeparator ();
    mnu.addAction (ui->actionGo_to_clipboard_path);
    mnu.addAction (ui->actionGo_to_path);
    mnu.addAction (ui->actionCopy_path_to_clipboard);

    QMenu favs (tr("Favorites"));
    QStringList fav_list = favouritePaths ();
    QAction * crt_act;
    if (fav_list.isEmpty ()) {
        crt_act = favs.addAction (tr("(none)"));
        crt_act->setDisabled (true);
    } else {
        foreach (const QString & s, fav_list) {
            crt_act = favs.addAction (
                        s, this,
                        SLOT(gotoFromAction()));
        }
        favs.addSeparator ();
        favs.addAction (
                    ui->actionRemove_all_favorites);
    }
    mnu.addMenu (&favs);

    mnu.addSeparator ();

    QMenu cmnds (tr("User Commands"));
    int i_max = stg.beginReadArray (STG_CUSTOM_COMMANDS);

    for (int i = 0; i < i_max; ++i) {
        stg.setArrayIndex (i);

        QStringList sl;
        sl << stg.value (STG_CUSTOM_CM_PROG).toString ()
           << stg.value (STG_CUSTOM_CM_ARGS).toString ()
           << stg.value (STG_CUSTOM_CM_CRTDIR).toString ()
           << stg.value (STG_CUSTOM_CM_NAME).toString ();

        crt_act = cmnds.addAction (
                    QString ("%1 (%2 %3)")
                    .arg(sl.at (3))
                    .arg(sl.at (0))
                    .arg(sl.at (1)),
                    this,
                    SLOT(userCommandFromAction()));
        crt_act->setData (sl);
    }
    stg.endArray ();

    if (i_max > 0) {
        cmnds.addSeparator ();
    }
    cmnds.addAction (
                ui->actionAdd_custom_command);
    mnu.addMenu (&cmnds);


    mnu.exec (mapToGlobal (pos));
}

void MainWindow::gotoFromAction()
{
    QAction * act = qobject_cast<QAction*>(sender ());
    if (act == NULL) return;
    selectPath (act->text (), true);
}

void MainWindow::userCommandFromAction()
{
    QAction * act = qobject_cast<QAction*>(sender ());
    if (act == NULL) return;
    execUserCommand (act->data ().toStringList ());
}

void MainWindow::on_actionRemove_all_favorites_triggered()
{
    int relpy =
            QMessageBox::question(
                this,
                tr("Are you sure?"),
                tr("You are about to remove all favorite paths.\n"
                   "\n"
                   "Are you sure?"),
                QMessageBox::Yes, QMessageBox::No);
    if (relpy == QMessageBox::Yes) {
        QStringList sl;
        setFavouritePaths (sl);
    }
}

void MainWindow::on_btn_settings_clicked()
{
    SettingsDlg dlg;
    if (dlg.exec () == QDialog::Accepted) {
        dlg.saveContent ();
    }
}

bool MainWindow::on_actionCopy_triggered()
{
    // get a list of files
    QList<QUrl> urls;
    foreach(const QModelIndex & mi, ui->treeView->selectionModel ()->selectedIndexes ()) {
        if (mi.column () == 0) {
            urls << QUrl::fromLocalFile (model->filePath (mi));
        }
    }

    if (urls.count () == 0) {
        PilesGui::showError (tr("Nothing selected"));
        return false;
    }

    QMimeData* mm_data = new QMimeData();
    mm_data->setUrls (urls);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData (mm_data);

    return true;
}

void MainWindow::on_actionCut_triggered()
{
    if (on_actionCopy_triggered()) {
        // TODO
    }
}

//https://qt.gitorious.org/qt-creator/qt-creator/source/1a37da73abb60ad06b7e33983ca51b266be5910e:src/app/main.cpp#L13-189
// taken from utils/fileutils.cpp.
static bool copyRecursively(const QString &srcFilePath,
                            const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
            return false;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

void MainWindow::on_actionPaste_triggered()
{

    QString s_path = currentDirectory ();
    if (s_path.isEmpty ()) {
        PilesGui::showError (
                    tr("No destination path selected"));
    } else {
        QClipboard *clipboard = QApplication::clipboard();
        const QMimeData* data = clipboard->mimeData ();

        if (!data->formats().contains ("text/uri-list")) {
            PilesGui::showError (
                        tr("No files in the clipboard"));
        } else {
            QList<QUrl> urls = data->urls ();
            if (urls.isEmpty ()) {
                PilesGui::showError (
                            tr("No files in the clipboard"));
            } else {
                int i_non_file = 0;
                foreach(const QUrl & u, urls) {
                    if (u.isLocalFile ()) {
                        QString s_path_source = u.toLocalFile ();
                        QFileInfo srcFileInfo (s_path_source);
                        QString s_path_target = QString("%1%2%3")
                                .arg(s_path)
                                .arg(QDir::separator ())
                                .arg(srcFileInfo.fileName ());

                        if (!copyRecursively (s_path_source, s_path_target)) {
                            PilesGui::showError (
                                        tr("Could not copy %1\nto %2")
                                        .arg(s_path_source).arg(s_path_target));
                            break;
                        }
                    } else {
                        ++i_non_file;
                    }
                }
                if (i_non_file > 0) {
                    if (i_non_file < urls.count ()) {
                        PilesGui::showError (
                                    tr("A number of %1 entries in the\n"
                                       "clipboard were not identified\n"
                                       "as local paths and could not be copied")
                                    .arg(i_non_file));
                    } else {
                        PilesGui::showError (
                                    tr("The clipboard does not contain local files"));
                    }
                }
            }
        }
    }
}

void MainWindow::on_actionAdd_custom_command_triggered()
{
    CustCmdDlg dlg;
    if (dlg.exec () == QDialog::Accepted) {
        dlg.saveSettings ();
    }
}

void MainWindow::on_actionCopy_and_replace_triggered()
{
    CopyReplaceDlg dlg;

    QStringList sl;
    foreach(const QModelIndex & mi, ui->treeView->selectionModel ()->selectedIndexes ()) {
        if (mi.column () == 0) {
            sl << model->filePath (mi);
        }
    }
    dlg.setSuggestions (sl);

    if (sl.count () == 1) {
        dlg.setDestination (sl.at(0));
        dlg.setSource (sl.at(0));
    } else if (sl.count () >= 2) {
        dlg.setDestination (sl.at(0));
        dlg.setSource (sl.at(1));
    }

    while (dlg.exec () == QDialog::Accepted) {
        if (dlg.perform ()) break;
    }

}

void MainWindow::on_actionSnippets_triggered()
{
    SnippetsDlg * dlg = new SnippetsDlg (this);
    dlg->show ();
    dlg->setAttribute (Qt::WA_DeleteOnClose);
}

void MainWindow::on_actionEnvironment_Variables_triggered()
{
    EnvDlg dlg;
    dlg.exec ();
}

void MainWindow::on_actionFiles_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::Files :
                    model->filter () & (~QDir::Files));
}

void MainWindow::on_actionDrives_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::Drives :
                    model->filter () & (~QDir::Drives));
}

void MainWindow::on_actionSymbolic_links_triggered(bool checked)
{
    model->setFilter (
                !checked ?
                    model->filter () | QDir::NoSymLinks :
                    model->filter () & (~QDir::NoSymLinks));
}

void MainWindow::on_actionReadable_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::Readable :
                    model->filter () & (~QDir::Readable));
}

void MainWindow::on_actionWritable_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::Writable :
                    model->filter () & (~QDir::Writable));
}

void MainWindow::on_actionExecutable_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::Executable :
                    model->filter () & (~QDir::Executable));
}

void MainWindow::on_actionModified_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::Modified :
                    model->filter () & (~QDir::Modified));
}

void MainWindow::on_actionHidden_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::Hidden :
                    model->filter () & (~QDir::Hidden));
}

void MainWindow::on_actionSystem_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::System :
                    model->filter () & (~QDir::System));
}

void MainWindow::on_actionCase_sensitive_triggered(bool checked)
{
    model->setFilter (
                checked ?
                    model->filter () | QDir::CaseSensitive :
                    model->filter () & (~QDir::CaseSensitive));
}

void MainWindow::on_actionCopy_path_to_clipboard_triggered()
{
    QString s_path = currentPath ();
    if (s_path.isEmpty ()) {
        PilesGui::showError (tr("Nothing selected"));
    } else {
        s_path = QDir::toNativeSeparators (s_path);
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText (s_path);
    }
}
