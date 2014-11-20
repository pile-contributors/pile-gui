#ifndef SNIPITEM_H
#define SNIPITEM_H

#include <QTreeWidgetItem>

class SnipGroup;

class SnipItem : public QTreeWidgetItem
{
public:
    SnipItem();
    virtual ~SnipItem();

    virtual bool
    isGrup() = 0;

    void
    setName (
            const QString & s_value) {
        setText (0, s_value);
    }

    void
    setIcon (
            const QString & s_value);

    const QString &
    iconString () {
        return s_icon_;
    }

    QString
    name () {
        return text (0);
    }

    QIcon
    icon () {
        return QTreeWidgetItem::icon (0);
    }

    void
    setDefaultIcon();

    SnipGroup *parentSnip();
    QString s_icon_;
};

class SnipGroup : public SnipItem
{
public:
    SnipGroup() {
        setFlags (Qt::ItemIsSelectable |
                  Qt::ItemIsEnabled |
                  Qt::ItemIsDragEnabled |
                  Qt::ItemIsDropEnabled);
    }
    virtual ~SnipGroup() {}

    virtual bool
    isGrup() {return true; }
};

class SnipSnip : public SnipItem
{
public:
    SnipSnip() {
        setFlags (Qt::ItemIsSelectable |
                  Qt::ItemIsEnabled |
                  Qt::ItemIsDragEnabled);
    }
    virtual ~SnipSnip() {}

    void
    setContent(
            const QString &s_value) {
        setText (1, s_value);
    }

    QString
    content() const {
        return text (1);
    }

    virtual bool
    isGrup() {return false; }

};


#endif // SNIPITEM_H
