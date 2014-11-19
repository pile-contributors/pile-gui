#ifndef ENVDLG_H
#define ENVDLG_H

#include <QDialog>

namespace Ui {
class EnvDlg;
}

class EnvDlg : public QDialog
{
    Q_OBJECT

public:
    explicit EnvDlg(QWidget *parent = 0);
    ~EnvDlg();

private slots:
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_textEdit_customContextMenuRequested(const QPoint &pos);

private:
    Ui::EnvDlg *ui;
};

#endif // ENVDLG_H
