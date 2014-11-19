#ifndef ENVDLG_H
#define ENVDLG_H

#include <QDialog>

namespace Ui {
class EnvDlg;
}

class QTreeWidgetItem;
class EnvItem;

class EnvDlg : public QDialog
{
    Q_OBJECT

public:
    explicit EnvDlg(QWidget *parent = 0);
    ~EnvDlg();

    bool saveXMLFile(const QString &s_file);
    bool loadXMLFile(const QString &s_file);
protected:
    void closeEvent(QCloseEvent *);
protected slots:
    void on_tv_content_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_actionReload_database_triggered();
    void on_actionSave_database_triggered();
    void on_tv_content_customContextMenuRequested(const QPoint &pos);
    void on_database_path_returnPressed();
private slots:

    void on_database_browse_clicked();

    void on_actionCreate_new_variable_triggered();

    void on_actionRemove_variable_triggered();

private:
    void loadEnvironment();
    void clearAll();
    bool hasName(const QString &value);

private:
    Ui::EnvDlg *ui;
    QList<EnvItem*> to_delete_;
};

#endif // ENVDLG_H
