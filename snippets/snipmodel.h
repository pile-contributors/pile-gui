#ifndef SNIPMODEL_H
#define SNIPMODEL_H

#include "snipitem.h"

class SnipItem;
class QDomDocument;
class QDomElement;

class SnipModel
{
public:

    SnipModel (QTreeWidget * tv);
    SnipModel (QTreeWidget * tv, QDomDocument & document);
    virtual ~SnipModel ();

    bool
    addGroup (
            const QString & s_name,
            SnipGroup * parent);

    bool
    addRootGroup (
            const QString & s_name);

    bool
    loadXML (
            QDomDocument & document);

    bool
    saveXML (
            QDomDocument & document);

    bool
    addSnip (
            const QString & s_name,
            SnipGroup * parent);

    SnipGroup * root_;
    QTreeWidget * tv_;
    SnipItem *currentItem();
    SnipGroup *currentGroup();
    bool removeItem(SnipItem *msel);
    void saveToItem(SnipItem *item, const QString &s_name, const QString &s_icon, const QString &s_content);
    void getFromItem(SnipItem *item, QString &s_name, QString &s_icon, QString &s_content);
private:
    bool loadXMLitem(const QDomElement &el, SnipItem *it);
    bool loadXMLSnip(const QDomElement &el, SnipGroup *parent);
    bool loadXMLGroup(const QDomElement &el_grp, SnipGroup *parent);
    bool saveXMLGroup(QDomElement &el_parent, SnipGroup *grp);
    bool saveXMLSnip(QDomElement &el_parent, SnipSnip *snip);
    QDomElement saveXMLItem(QDomElement &el_parent, SnipItem *item);
};

#endif // SNIPMODEL_H
