#include "pilecontainer.h"
#include "pileprovider.h"
#include "pile.h"
#include "../setting_names.h"

#include <QSettings>

PileContainer::PileContainer()
{
}

PileContainer::~PileContainer()
{
    foreach(PileProvider * iter, l_) {
        delete iter;
    }
}

void PileContainer::loadFromSettings()
{
    QSettings s;
    int i_max = s.beginReadArray (STG_PILE_ARRAY);
    for (int i = 0; i < i_max; ++i) {
        s.setArrayIndex (i);
        ProviderKind k = (ProviderKind)s.value ("kind").toInt ();
        PileProvider * pprov;

        switch (k) {
        case PROV_TXT: {
            pprov = PileProviderText::fromSettings (this, s);
            break; }
        case PROV_XML: {
            pprov = PileProviderXml::fromSettings (this, s);
            break; }
        case PROV_REMOTE: {
            pprov = PileProviderRemote::fromSettings (this, s);
            break; }
        case PROV_DIR: {
            pprov = PileProviderDir::fromSettings (this, s);
            break; }
        case PROV_USER: {
            pprov = PileProviderUser::fromSettings (this, s);
            break; }
        default:
            pprov = NULL;
            // TODO error
            break;
        }

        if (pprov != NULL) {
            pprov->rescan ();
        }
    }
    s.endArray ();
}

void PileContainer::saveToSettings() const
{
    QSettings s;
    s.beginWriteArray (STG_PILE_ARRAY, l_.size ());
    foreach(PileProvider * iter, l_) {
        s.setValue ("kind", iter->providerKind ());
        iter->save (s);
    }
    s.endArray ();
}
