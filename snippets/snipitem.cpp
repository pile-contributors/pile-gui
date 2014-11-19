#include "snipitem.h"

#include "SnipItem.h"

#include <QtXml>


SnipItem::SnipItem() : QTreeWidgetItem()
{
    setFlags (Qt::ItemIsUserCheckable |
              Qt::ItemIsSelectable |
              Qt::ItemIsEnabled);
    //setDefaultIcon ();
}

SnipItem::~SnipItem()
{
    QTreeWidgetItem * tvi;
    int i_max = childCount ();
    for (int i = 0; i < i_max; ++i) {
        tvi = takeChild (0);
        delete tvi;
    }
}

SnipGroup * SnipItem::parentSnip()
{
    QTreeWidgetItem * tvi = parent();
    if (tvi == NULL) return NULL;
    return static_cast<SnipGroup *>(tvi);
}

void SnipItem::setDefaultIcon ()
{
    if (isGrup ()) {
        //QIcon ic_res ("G:/prog-archive/fatcow-hosting-icons-3800/FatCow_Icons16x16/folder.png");
        QIcon ic_res (":/icons/resources/icons/folder.png");
        if (!ic_res.isNull ()) {
            QTreeWidgetItem::setIcon (0, ic_res);
        }
    } else {
        //QIcon ic_res ("G:/prog-archive/fatcow-hosting-icons-3800/FatCow_Icons16x16/text.png");
        QIcon ic_res (":/icons/resources/icons/text.png");
        if (!ic_res.isNull ()) {
            QTreeWidgetItem::setIcon (0, ic_res);
        }
    }
}

void SnipItem::setIcon (const QString & s_value)
{
    if (s_value.isEmpty ()) setDefaultIcon ();
    if (s_icon_ == s_value) return;
    s_icon_ = s_value;
    QIcon ic (s_value);
    if (ic.isNull ()) {
        setDefaultIcon ();
    } else {
        QTreeWidgetItem::setIcon(0, ic);
    }
}
