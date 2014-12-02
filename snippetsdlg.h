#ifndef SNIPPETSDLG_H
#define SNIPPETSDLG_H

#include <QDialog>

namespace Ui {
class SnippetsDlg;
}

class SnipModel;
class QTreeWidgetItem;

class SnippetsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SnippetsDlg(QWidget *parent = 0);
    ~SnippetsDlg();

public slots:
    bool loadXMLFile(const QString &s_file);
    bool saveXMLFile(const QString &s_file);
private slots:
    void on_database_browse_clicked();

    void on_database_path_editingFinished();

    void on_database_path_returnPressed();

    void on_tv_content_customContextMenuRequested(const QPoint &pos);

    void on_actionAdd_new_group_triggered();

    void on_actionAdd_new_snipped_triggered();

    void on_actionRemove_triggered();

    void on_actionSave_database_triggered();

    void on_actionReload_database_triggered();

    void on_tv_content_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_b_icon_clicked();

    void on_le_name_returnPressed();

protected:

    void
    closeEvent (
            QCloseEvent *);

private:
    Ui::SnippetsDlg *ui;
    SnipModel *model_;
};

#endif // SNIPPETSDLG_H
