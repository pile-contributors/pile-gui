#ifndef NEWPILEDLG_H
#define NEWPILEDLG_H

#include <QDialog>

namespace Ui {
class NewPileDlg;
}

class QTreeWidgetItem;

class NewPileDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewPileDlg(QWidget *parent = 0);
    ~NewPileDlg();

    bool doCreatePile();
private slots:
    void on_btn_template_path_clicked();

    void on_btn_output_path_clicked();

    void on_le_template_path_editingFinished();

    void on_le_pile_name_editingFinished();

private:
    Ui::NewPileDlg *ui;
    void addItemToTree(const QString &s_value, const QString &s_name, bool b_file);
    void reloadGenContent();
    QTreeWidgetItem *createItem(const QString &s_name, const QString & s_source, QTreeWidgetItem *crt_parent, bool b_file);
};

#endif // NEWPILEDLG_H
