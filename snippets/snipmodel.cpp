#include "snipmodel.h"
#include "snipitem.h"


#include <QtXml>
#include <QDomNode>
#include <QDomElement>
#include <QDomDocument>


SnipModel::SnipModel(QDomDocument document, QObject *parent)
    : QAbstractItemModel(parent), dom_document_(document)
{
    QDomElement del = dom_document_.documentElement ();
    rootItem = new SnipItem (del, 0);
}



SnipModel::~SnipModel()
{
    delete rootItem;
}



int SnipModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 3;
}



QVariant SnipModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((role != Qt::DisplayRole) && (role !=Qt::EditRole))
        return QVariant();

    SnipItem *item = static_cast<SnipItem*>(index.internalPointer());

    QDomNode node = item->node();

    QStringList attributes;
    QDomNamedNodeMap attributeMap = node.attributes();

    switch (index.column()) {
        case 0:
            return node.nodeName();
        case 1:
            for (int i = 0; i < attributeMap.count(); ++i) {
                QDomNode attribute = attributeMap.item(i);
                attributes << attribute.nodeName() + "=\""
                              +attribute.nodeValue() + "\"";
            }
            return attributes.join(' ');
        case 2:
            return node.nodeValue().split("\n").join(' ');
        default:
            return QVariant();
    }
}



Qt::ItemFlags SnipModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    SnipItem *item = static_cast<SnipItem*>(index.internalPointer());

    return QAbstractItemModel::flags(index);

    if (item->node().toElement ().tagName()=="group")
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable|
                Qt::ItemIsEditable ;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable|
                Qt::ItemIsEditable |Qt::ItemIsDragEnabled;
}



QVariant SnipModel::headerData(int section, Qt::Orientation orientation,
                              int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Attributes");
            case 2:
                return tr("Value");
            default:
                return QVariant();
        }
    }

    return QVariant();
}



QModelIndex SnipModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SnipItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SnipItem*>(parent.internalPointer());



    SnipItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}



QModelIndex SnipModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    SnipItem *childItem = static_cast<SnipItem*>(child.internalPointer());
    SnipItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SnipModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    SnipItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SnipItem*>(parent.internalPointer());

    return parentItem->node().childNodes().count();
}


bool SnipModel::setData( const QModelIndex &index, const QVariant &value,
                        int role  )
{
    SnipItem *item = static_cast< SnipItem*>(index.internalPointer());
    switch( role )
    {
    case Qt::EditRole:
    {
        switch( index.column() )
        {
        case 0:{
            //item->SetNodeName(value.toString());
            break;}
        case 1:
        {
            //item->SetNodeCost(value.toString());
            break;
        }
        case 2:
        {
            //qDebug()<< value.toStringList();
            // item->SetBold(value.toBool());
            break;
        }
        case 3:
        {
            //item->SetHidden(value.toBool());
            break;
        }
        default:
            break;
        }
    }
    }
    emit dataChanged( index, index );
    return true;
}

bool  SnipModel::AddRootGroup(
        const QString & s_name)
{
    bool b_ret = false;
    for (;;) {
        QDomElement newSection =
                rootItem->node().ownerDocument().createElement("group");
        newSection.setAttribute ("name", s_name);

        beginInsertRows (QModelIndex(),1,1);
        rootItem->node().appendChild (newSection);
        endInsertRows();
        emit dataChanged( QModelIndex(),  QModelIndex());

        b_ret = true;
        break;
    }

    return b_ret;
}

bool  SnipModel::AddGroup(const QModelIndex &index, const QString & s_name)
{
    bool b_ret = false;
    for (;;) {


        b_ret = true;
        break;
    }

    return b_ret;

    QDomElement newSection=rootItem->node().ownerDocument().createElement("group");
    beginInsertRows(QModelIndex(),1,1);
    int i=0;
    while(IsUnicName(rootItem->node().toElement (),tr("Group%1").arg(i))==false)
    {
        i++;
    }


    newSection.setAttribute("name",tr("Group%1").arg(i));
    rootItem->node().appendChild(newSection);
    endInsertRows();
    emit dataChanged( index,  index);
}


void  SnipModel::AddItm(const QModelIndex &index, const QString & s_name)
{
    if (index.isValid()==true)
    {
        SnipItem *item = static_cast< SnipItem*>(index.internalPointer());
        if(item->node().toElement ().tagName()=="group")
        {
            QDomElement newSection=item->node().ownerDocument().createElement("itm");
            beginInsertRows(index,1,1);
            int i=0;
            while(IsUnicName(item->node().toElement (),tr("item%1").arg(i))==false)
            {
                i++;
            }
            newSection.setAttribute("name",tr("item%1").arg(i));
            item->node().appendChild(newSection);
            endInsertRows();
            emit dataChanged( index,  index);
        }
    }
}


bool SnipModel::IsUnicName(QDomElement node,QString name)
{
    for(int i=0;i<node.childNodes().count();i++)
    {
        if (node.childNodes().at(i).toElement().attribute("name")==name)
            return false;
        if(node.childNodes().at(i).hasChildNodes()==true)
            if( IsUnicName(node.childNodes().at(i).toElement(),name)==false)
                return false;
    }
    return true;
}


void SnipModel::Remove(const QModelIndex &index)
{
    if (index.isValid()==true)
    {
        SnipItem *item = static_cast< SnipItem*>(index.internalPointer());
        if(item->node().childNodes().count()>0)
        {
            //emit this->CannotRemove(tr("remove All children first"));
            return;
        }
        else
        {
            SnipItem *parent=item->parent();
            beginRemoveRows(index,index.row(),index.row());
            parent->RemoveChildren(item);
            removeRow(index.row(),index);
            endRemoveRows();
            emit dataChanged( QModelIndex(), QModelIndex());

        }
    }
}


Qt::DropActions SnipModel::supportedDragActions () const
{
    return Qt::CopyAction;
}

QStringList SnipModel::mimeTypes() const
{
    QStringList types;
    types << "application/itm";
    return types;
}

QMimeData *SnipModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            SnipItem *item = static_cast< SnipItem*>(index.internalPointer());
            stream <<item->parent()->NodeName() ;
            stream <<item->NodeName() ;
            stream <<item->NodeCost() ;
        }
    }
    mimeData->setData("application/itm", encodedData);
    return mimeData;
}

