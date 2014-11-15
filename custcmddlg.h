#ifndef CUSTCMDDLG_H
#define CUSTCMDDLG_H

#include <QDialog>

class QTreeWidgetItem;
class QAbstractButton;

namespace Ui {
class CustCmdDlg;
}

class CustCmdDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CustCmdDlg(QWidget *parent = 0);
    ~CustCmdDlg();

public slots:
    bool loadSettings();
    bool saveSettings();
private slots:
    void on_b_program_clicked();

    void on_treeWidget_currentItemChanged(
            QTreeWidgetItem *current,
            QTreeWidgetItem *previous);

    void on_b_new_command_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::CustCmdDlg *ui;
    bool isNewCommandItem(QTreeWidgetItem *it);
    void saveToItem(QTreeWidgetItem *previous);
    bool eventFilter(QObject * target, QEvent * event);
    QTreeWidgetItem *addCommand(const QStringList &sl_content);
};

#endif // CUSTCMDDLG_H
