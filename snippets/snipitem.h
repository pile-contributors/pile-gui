#ifndef SNIPITEM_H
#define SNIPITEM_H

#include <QDomNode>
#include <QDomElement>
#include <QHash>

class SnipItem
{
public:
    SnipItem(QDomElement &node, int row, SnipItem *parent = 0);
    ~SnipItem();
    SnipItem *child(int i);
    SnipItem *parent();
    QDomElement node() const;
    int row();


    void SetNodeName(QString name);
    void SetNodeCost(QString cost);
    QString NodeName();
    QString NodeCost();
    void RemoveChildren(SnipItem *node);



private:
    QDomElement domNode;
    QHash<int,SnipItem*> childItems;
    SnipItem *parentItem;
    int rowNumber;
};
#endif // SNIPITEM_H
