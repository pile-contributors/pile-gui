#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>

namespace Ui {
class MainWindow;
}

class QToolButton;
class QToolBar;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString currentFile();
    QString currentDirectory();
    QString currentPath();
    QStringList favouritePaths() const;
    void afterShown();
public slots:
    void saveSettings();
    void loadSettings();
    void addToFavouritePaths(const QString &s_value);
    void setFavouritePaths(const QStringList &sl);
    bool selectPath(
            const QString &s_path,
            bool b_verbose = false);
    void navigationUser(const QString &s_path);
    bool execUserCommand(const QStringList &sl);
    void showStatusMessage(const QString &s_path);

protected:
    void closeEvent (QCloseEvent *);
protected slots:
    void fileListSelectionChanged(
            const QModelIndex &current,
            const QModelIndex &previous);
    void gotoFromAction();

private slots:
    void on_pushButton_clicked();

    void on_btn_open_clicked();

    void on_btn_new_pile_clicked();

    void on_btn_add_pile_clicked();

    void on_actionSet_root_here_triggered();

    void on_actionClear_virtual_root_triggered();

    void on_actionAdd_to_favorites_triggered();

    void on_treeView_customContextMenuRequested(const QPoint &pos);

    void on_actionGo_to_path_triggered();

    void on_actionGo_to_clipboard_path_triggered();

    void on_actionRemove_all_favorites_triggered();

    void on_btn_settings_clicked();

    void navigationAction();

    void on_actionCopy_triggered();

    void on_actionCut_triggered();

    void on_actionPaste_triggered();

    void userCommandFromAction();
    void on_actionAdd_custom_command_triggered();

    void on_actionCopy_and_replace_triggered();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    QModelIndex my_comp_index;
    QMenu * forward_menu;
    QMenu * backward_menu;
    bool navigation_lock_;

    QToolButton * toolBtnForAction(QToolBar *toolBar, QAction *action);
    void navigationTrim(QMenu *mnu, const QList<QAction *> &all_acts);
};

#endif // MAINWINDOW_H
