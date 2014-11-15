#ifndef PILECONTAINER_H
#define PILECONTAINER_H

#include <QList>

class PileProvider;

class PileContainer
{
public:

    PileContainer();

    virtual ~PileContainer();

    void
    loadFromSettings();

    void
    saveToSettings() const;

    const QList<PileProvider*> &
    providers() const
    { return l_; }

    PileProvider*
    provider(
            int i) const
    { return l_[i]; }

    int
    providerCount() const
    { return l_.count (); }

    int
    index(
            PileProvider* value) const
    { return l_.indexOf (value); }

    void
    addProvider(
           PileProvider* value)
    { l_.append (value); }

    void
    remProvider(
           PileProvider* value)
    { l_.removeOne (value); }

private:
    QList<PileProvider*> l_;
};

#endif // PILECONTAINER_H
