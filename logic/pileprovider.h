#ifndef PILEPROVIDER_H
#define PILEPROVIDER_H

#include <QString>
#include <QStringList>
#include <QList>

class PileContainer;
class Pile;
class QSettings;

enum ProviderKind {
    PROV_TXT,
    PROV_XML,
    PROV_REMOTE,
    PROV_DIR,
    PROV_USER
};

class PileProvider
{
public:

    PileProvider (ProviderKind kind);

    virtual ~PileProvider();

    virtual QString
    toString () = 0;

    ProviderKind
    providerKind () const
    { return kind_; }

    virtual bool
    rescan () = 0;

    virtual bool
    save (
            QSettings & s) = 0;


    const QList<Pile*> &
    piles() const
    { return piles_; }

    Pile*
    pile (
            int i) const
    { return piles_[i]; }

    int
    pileCount() const
    { return piles_.count (); }

    int
    index(
            Pile* value) const
    { return piles_.indexOf (value); }

    void
    addPile(
           Pile* value)
    { piles_.append (value); }

    void
    remPile(
           Pile* value)
    { piles_.removeOne (value); }

    void
    clearPiles();

protected:

    bool
    rescanTextFile (
            const QString & source);

    bool
    rescanXMLFile (
            const QString & source);

private:
    ProviderKind kind_;
    QList<Pile*> piles_;
};

class PileProviderText : public PileProvider
{
public:
    PileProviderText (const QString & s_path):
        PileProvider (PROV_TXT),
        source_  (s_path)
    {}

    virtual QString
    toString () {
        return source_;
    }

    virtual bool
    rescan ();

    virtual bool
    save (
            QSettings & s);

    static PileProvider *
    fromSettings (
            PileContainer * container,
            QSettings & s);

private:
    QString source_;
};

class PileProviderXml : public PileProvider
{
public:
    PileProviderXml (const QString & s_path):
        PileProvider (PROV_XML),
        source_  (s_path)
    {}

    virtual QString
    toString () {
        return source_;
    }

    virtual bool
    rescan ();

    virtual bool
    save (
            QSettings & s);

    static PileProvider *
    fromSettings (
            PileContainer * container,
            QSettings & s);

private:
    QString source_;
};

class PileProviderDir : public PileProvider
{
public:
    PileProviderDir (const QString & s_path):
        PileProvider (PROV_DIR),
        source_  (s_path)
    {}

    virtual QString
    toString () {
        return source_;
    }

    virtual bool
    rescan ();

    virtual bool
    save (
            QSettings & s);

    static PileProvider *
    fromSettings (
            PileContainer * container,
            QSettings & s);

private:
    QString source_;
};

class PileProviderRemote : public PileProvider
{
public:
    PileProviderRemote (const QString & s_url):
        PileProvider (PROV_REMOTE),
        source_  (s_url)
    {}

    virtual QString
    toString () {
        return source_;
    }

    virtual bool
    rescan ();

    virtual bool
    save (
            QSettings & s);

    static PileProvider *
    fromSettings (
            PileContainer * container,
            QSettings & s);
private:

    static void
    rescanDone (
            const QString &s_downloaded_file,
            void * user);

private:
    QString source_;
};

class PileProviderUser : public PileProvider
{
public:
    PileProviderUser () :
        PileProvider (PROV_USER)
    {}

    virtual QString
    toString () {
        return QObject::tr ("User Provided");
    }

    virtual bool
    rescan ();

    virtual bool
    save (
            QSettings & s);

    static PileProvider *
    fromSettings (
            PileContainer * container,
            QSettings & s);


private:
    QStringList sl_piles_;
};

#endif // PILEPROVIDER_H
