#include "snipmodel.h"
#include "snipitem.h"

#include "../pilesgui.h"
#include <QDomDocument>

SnipModel::SnipModel (QTreeWidget * tv) :
    tv_(tv)
{
    root_ = new SnipGroup ();
    root_->setText (0, QObject::tr ("(no database)"));
    tv->addTopLevelItem (root_);
    root_->setDefaultIcon ();
}

SnipModel::SnipModel (QTreeWidget * tv, QDomDocument & document) :
    tv_(tv)
{
    root_ = new SnipGroup ();
    root_->setText (0, QObject::tr ("(no database)"));
    tv->addTopLevelItem (root_);
    root_->setDefaultIcon ();

    loadXML (document);
}

SnipModel::~SnipModel ()
{
    QTreeWidgetItem * tvi;
    int i_max = tv_->topLevelItemCount ();
    for (int i = 0; i < i_max; ++i) {
        tvi = tv_->takeTopLevelItem (0);
        delete tvi;
    }
}


QDomElement SnipModel::saveXMLItem (
        QDomElement & el_parent, SnipItem * item)
{
    QDomElement el_ret;
    for (;;) {

        el_ret = el_parent.ownerDocument ().createElement ("entry");
        el_ret.setAttribute ("name", item->name ());
        el_ret.setAttribute ("icon", item->iconString ());

        el_parent.appendChild (el_ret);
        break;
    }

    return el_ret;
}

bool SnipModel::loadXMLitem (const QDomElement & el, SnipItem * it)
{
    bool b_ret = false;
    for (;;) {

        it->setName (el.attribute ("name"));
        it->setIcon (el.attribute ("icon"));

        b_ret = true;
        break;
    }

    return b_ret;
}

bool SnipModel::saveXMLSnip (QDomElement & el_parent, SnipSnip * snip)
{
    bool b_ret = false;
    for (;;) {

        QDomElement el = saveXMLItem (el_parent, snip);
        el.setAttribute ("type", "snippet");
        el.setAttribute ("content", snip->content ());

        b_ret = true;
        break;
    }

    return b_ret;
}

bool SnipModel::loadXMLSnip (const QDomElement & el, SnipGroup * parent)
{
    bool b_ret = false;
    for (;;) {
        SnipSnip * snp = new SnipSnip();
        parent->addChild (snp);

        if (!loadXMLitem (el, snp)) {
            PilesGui::showError (
                        QObject::tr (
                            "The xml document failed to load due to %1")
                        .arg (el.text ()));
            break;
        }

        snp->setContent  (el.attribute ("content"));

        b_ret = true;
        break;
    }

    return b_ret;
}

bool SnipModel::saveXMLGroup (QDomElement & el_parent, SnipGroup * grp)
{
    bool b_ret = false;
    for (;;) {

        QDomElement el = saveXMLItem (el_parent, grp);
        el.setAttribute ("type", "group");
        el.setAttribute ("expanded", grp->isExpanded () ? "true" : "false");

        QTreeWidgetItem * tvi;
        int i_max = grp->childCount ();
        bool b_intermed = true;
        for (int i = 0; i < i_max; ++i) {
            tvi = grp->child (i);
            SnipItem * item = static_cast<SnipItem *>(tvi);
            if (item->isGrup ()) {
                SnipGroup * grp = static_cast<SnipGroup *>(item);
                b_intermed = saveXMLGroup (el, grp);
            } else {
                SnipSnip * snip = static_cast<SnipSnip *>(item);
                b_intermed = saveXMLSnip (el, snip);
            }
            if (!b_intermed) break;
        }
        if (!b_intermed) break;

        b_ret = true;
        break;
    }

    return b_ret;
}

bool SnipModel::loadXMLGroup (const QDomElement & el_grp, SnipGroup * parent)
{
    bool b_ret = false;
    for (;;) {

        SnipGroup * grp = new SnipGroup();
        parent->addChild (grp);

        if (!loadXMLitem (el_grp, grp)) {
            PilesGui::showError (
                        QObject::tr (
                            "The xml document failed to load due to %1")
                        .arg (el_grp.text ()));
            break;
        }

        bool b_subitems = true;
        QDomNodeList nl = el_grp.childNodes ();
        int i_max = nl.count ();
        for (int i = 0; i < i_max; ++i) {

            const QDomNode & e (nl.at (i));
            if (e.isElement ()) {

                const QDomElement el = e.toElement ();
                QString s_type = el.attribute ("type");

                if (s_type == "group") {
                    b_subitems = loadXMLGroup (el, grp);
                } else if (s_type == "snippet") {
                    b_subitems = loadXMLSnip (el, grp);
                } else {
                    if (s_type.isEmpty ()) {
                        s_type = QObject::tr("(empty)");
                    }
                    PilesGui::showError (
                                QObject::tr (
                                    "The xml document is invalid;\ninvalid type %1")
                                .arg (s_type));
                    b_subitems = false;
                    break;
                }
                if (!b_subitems) break;
            }
        }
        if (!b_subitems) break;

        QString s_expanded = el_grp.attribute ("expanded");
        grp->setExpanded (s_expanded == "true");

        b_ret = true;
        break;
    }

    return b_ret;
}

bool SnipModel::loadXML (QDomDocument &document)
{
    bool b_ret = false;
    for (;;) {
        if (document.isNull ()) {
            PilesGui::showError (
                        QObject::tr ("The xml document is invalid"));
            break;
        }

        QDomElement root = document.documentElement ();
        if (root.isNull ()) {
            PilesGui::showError (
                        QObject::tr ("The xml document is invalid"));
            break;
        }

        bool b_subitems = true;

        QDomNodeList nl = root.childNodes ();
        int i_max = nl.count ();
        for (int i = 0; i < i_max; ++i) {
            const QDomNode & e (nl.at (i));
            if (e.isElement ()) {
                const QDomElement el = e.toElement ();
                QString s_type = el.attribute ("type");
                if (s_type == "group") {
                    b_subitems = loadXMLGroup (el, root_);
                } else if (s_type == "snippet") {
                    b_subitems = loadXMLSnip (el, root_);
                } else {
                    if (s_type.isEmpty ()) {
                        s_type = QObject::tr("(empty)");
                    }
                    PilesGui::showError (
                                QObject::tr (
                                    "The xml document is invalid;\ninvalid type %1")
                                .arg (s_type));
                    b_subitems = false;
                    break;
                }
                if (!b_subitems) break;
            }
        }
        if (!b_subitems) break;
        root_->setExpanded (true);

        b_ret = true;
        break;
    }

    return b_ret;
}

bool SnipModel::saveXML (QDomDocument &document)
{
    bool b_ret = false;
    for (;;) {

        if (document.isNull ()) {
            PilesGui::showError (
                        QObject::tr ("The xml document is invalid"));
            break;
        }

        QDomProcessingInstruction header =
                document.createProcessingInstruction(
                    "xml", "version=\"1.0\" encoding=\"UTF-8\"");
        document.appendChild (header);

        QDomElement root = document.createElement ("snippets");
        document.appendChild(root);

        QTreeWidgetItem * tvi;
        int i_max = root_->childCount ();
        bool b_intermed = true;
        for (int i = 0; i < i_max; ++i) {
            tvi = root_->child (i);
            SnipItem * item = static_cast<SnipItem *>(tvi);
            if (item->isGrup ()) {
                SnipGroup * grp = static_cast<SnipGroup *>(item);
                b_intermed = saveXMLGroup (root, grp);
            } else {
                SnipSnip * snip = static_cast<SnipSnip *>(item);
                b_intermed = saveXMLSnip (root, snip);
            }
            if (!b_intermed) break;
        }
        if (!b_intermed) break;

        b_ret = true;
        break;
    }

    return b_ret;
}

void SnipModel::saveToItem (
        SnipItem * item, const QString & s_name,
        const QString & s_icon, const QString & s_content )
{
    if (item == NULL) return;
    if (item->parent () == NULL) return;

    item->setName (s_name);
    item->setIcon (s_icon);
    if (item->isGrup ()) {
        //SnipGroup * grp = static_cast<SnipGroup *>(item);
    } else {
        SnipSnip * snp = static_cast<SnipSnip *>(item);
        snp->setContent (s_content);
    }
}


void SnipModel::getFromItem (
        SnipItem * item, QString & s_name,
        QString & s_icon, QString & s_content )
{
    s_name.clear ();
    s_icon.clear ();
    s_content.clear ();

    if (item == NULL) return;
    if (item->parent () == NULL) return;

    s_name = item->text (0);
    s_icon = item->iconString ();

    if (item->isGrup ()) {
        //SnipGroup * grp = static_cast<SnipGroup *>(item);
    } else {
        SnipSnip * snp = static_cast<SnipSnip *>(item);
        s_content = snp->content ();
    }
}

bool SnipModel::addGroup (
        const QString & s_name, SnipGroup * parent)
{
    bool b_ret = false;
    for (;;) {
        if (parent == NULL) break;
        if (!parent->isGrup ()) break;

        SnipGroup * tvi = new SnipGroup ();
        tvi->setText (0, s_name);
        parent->insertChild (parent->childCount (), tvi);

        tv_->setCurrentItem (tvi);
        tv_->scrollToItem (tvi);
        b_ret = true;
        break;
    }

    return b_ret;
}

bool SnipModel::addRootGroup (
        const QString & s_name)
{
    return addGroup (s_name, root_);
}

bool SnipModel::addSnip (
        const QString & s_name, SnipGroup * parent)
{
    bool b_ret = false;
    for (;;) {
        if (parent == NULL) break;
        if (!parent->isGrup ()) break;

        SnipSnip * tvi = new SnipSnip ();
        tvi->setText (0, s_name);
        parent->insertChild (parent->childCount (), tvi);

        tv_->setCurrentItem (tvi);
        tv_->scrollToItem (tvi);
        b_ret = true;
        break;
    }

    return b_ret;
}

SnipItem * SnipModel::currentItem ()
{
    QTreeWidgetItem * tvi = tv_->currentItem ();
    if (tvi == NULL) return NULL;
    return static_cast<SnipItem *>(tvi);
}

SnipGroup * SnipModel::currentGroup ()
{
    QTreeWidgetItem * tvi = tv_->currentItem ();
    if (tvi == NULL) return NULL;
    SnipItem * si = static_cast<SnipItem *>(tvi);
    if (si->isGrup ()) return static_cast<SnipGroup *>(si);
    return si->parentSnip ();
}

bool SnipModel::removeItem (SnipItem * msel)
{
    bool b_ret = false;
    for (;;) {
        if (msel == NULL) {
            PilesGui::showError (
                        QObject::tr (
                            "Nothing selected"));
            break;
        }

        if (msel->parent () == NULL) {
            PilesGui::showError (
                        QObject::tr (
                            "Can't delete the root group"));
            break;
        }

        int i = msel->parent ()->indexOfChild (msel);
        msel->parent ()->takeChild (i);
        delete msel;

        b_ret = true;
        break;
    }

    return b_ret;
}

