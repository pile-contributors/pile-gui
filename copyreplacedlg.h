#ifndef COPYREPLACEDLG_H
#define COPYREPLACEDLG_H

#include <QDialog>

namespace Ui {
class CopyReplaceDlg;
}
class QTreeWidgetItem;

class CopyReplaceDlg : public QDialog
{
    Q_OBJECT

public:

    explicit CopyReplaceDlg(QWidget *parent = 0);

    ~CopyReplaceDlg();

public slots:

    void setSuggestions(const QStringList &sl);

    void setSource (const QString & s_value);

    void setDestination (const QString & s_value);

    bool perform();

private slots:

    void on_b_source_clicked();

    void on_b_destination_clicked();

    void on_tv_replace_itemChanged(QTreeWidgetItem *item, int column);

    void on_tv_filter_itemChanged(QTreeWidgetItem *item, int column);
private:

    bool isNewCommandEntry(QTreeWidgetItem *tvi);

private:
    Ui::CopyReplaceDlg *ui;
    bool editor_lock_;
    bool eventFilter(QObject *target, QEvent *event);
};

#endif // COPYREPLACEDLG_H
