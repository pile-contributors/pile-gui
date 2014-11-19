#ifndef SNIPMODEL_H
#define SNIPMODEL_H

#include <QAbstractItemModel>
#include <QDomDocument>
#include <QModelIndex>

class SnipItem;

class SnipModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SnipModel(QDomDocument document, QObject *parent = 0);
    ~SnipModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void Remove(const QModelIndex &index);
    bool IsUnicName(QDomElement node, QString name);
    void AddItm(const QModelIndex &index, const QString & s_name);
    bool AddGroup(const QModelIndex &index, const QString & s_name);
    bool AddRootGroup (const QString & s_name);

    QDomDocument &
    domDocument () {
        return dom_document_;
    }

protected:
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    Qt::DropActions supportedDragActions() const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
private:
    QDomDocument dom_document_;
    SnipItem *rootItem;
};

#endif // SNIPMODEL_H
