#include "snipitem.h"

#include "SnipItem.h"

#include <QtXml>


SnipItem::SnipItem(QDomElement &node, int row, SnipItem *parent)
{
    domNode = node;

    // Record the item's location within its parent.

    rowNumber = row;
    parentItem = parent;
}



SnipItem::~SnipItem()
{
    QHash<int,SnipItem*>::iterator it;
    for (it = childItems.begin(); it != childItems.end(); ++it)
        delete it.value();
}



QDomElement SnipItem::node() const
{
    return domNode;
}



SnipItem *SnipItem::parent()
{
    return parentItem;
}



SnipItem *SnipItem::child (int i)
{
    if (childItems.contains(i))
        return childItems[i];

    if (i >= 0 && i < domNode.childNodes().count()) {
        QDomNode childNode = domNode.childNodes().item(i);
        if (childNode.isElement ()) {
            QDomElement child_element = childNode.toElement ();

            SnipItem *childItem = new SnipItem (child_element, i, this);
            childItems[i] = childItem;
            return childItem;
        }
    }
    return 0;
}



int SnipItem::row()
{
    return rowNumber;
}

void SnipItem::RemoveChildren(SnipItem *node)
{
    this->node().removeChild(node->node());
    foreach(int i, childItems.keys ()) {
        if (childItems.value (i) == node) {
            childItems.remove (i);
        }
    }
    //    childItems.clear();
}


void SnipItem::SetNodeName(QString name)
{
    if(name.isEmpty()==false)
    {
        node().toElement ().setAttribute("name",name);
    }
}



void SnipItem::SetNodeCost(QString cost)
{
    if(cost.isEmpty()==false)
    {
        node().toElement ().setAttribute("cost",cost);
    }else
        node().toElement ().setAttribute("cost",0);
}


QString SnipItem::NodeName(){
    return node().toElement ().attribute("name");
}

QString SnipItem::NodeCost(){
    return node().toElement ().attribute("cost");
}
