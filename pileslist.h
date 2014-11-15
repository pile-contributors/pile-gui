#ifndef PILESLIST_H
#define PILESLIST_H

#include <QDialog>

class QTreeWidgetItem;
class PileProvider;

namespace Ui {
class PilesList;
}

class PilesList : public QDialog
{
    Q_OBJECT

public:
    explicit PilesList(QWidget *parent = 0);
    ~PilesList();

public slots:

    void loadTextFile(const QString &fileName);

    void loadDirectory(const QString &fileName);

    void loadXMLFile(const QString &fileName);

private slots:

    void on_actionAdd_source_text_triggered();

    void on_actionAdd_source_directory_triggered();

    void on_actionAdd_source_XML_File_triggered();

    void on_actionAdd_source_URL_triggered();

    void on_actionAdd_user_pile_triggered();

    void on_actionRemove_triggered();

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

private:

    QTreeWidgetItem *insertProvider(
            PileProvider *prov);

private:
    Ui::PilesList *ui;
};

#endif // PILESLIST_H
