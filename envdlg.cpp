#include "envdlg.h"
#include "ui_envdlg.h"

EnvDlg::EnvDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnvDlg)
{
    ui->setupUi(this);
}

EnvDlg::~EnvDlg()
{
    delete ui;
}

void EnvDlg::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{

}

void EnvDlg::on_textEdit_customContextMenuRequested(const QPoint &pos)
{

}
