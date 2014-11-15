#include "pileprovider.h"
#include "pilecontainer.h"
#include "pile.h"
#include "../pilesgui.h"
#include "../downloader.h"

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QDir>

#define SETT_VALUE "value"
#define SETT_SUBCOMP "subcomp"

PileProvider::PileProvider (ProviderKind kind) :
    kind_(kind),
    piles_()
{
}

PileProvider::~PileProvider ()
{
    clearPiles ();
}

void PileProvider::clearPiles()
{
    foreach(Pile * iter, piles_) {
        delete iter;
    }
}


bool PileProvider::rescanXMLFile (const QString & source)
{
    bool b_ret = false;
    QFile* file = NULL;
    QXmlStreamReader* xml = NULL;
    for (;;) {

        // attempt to open it
        file = new QFile (source);
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
            // TODO message ?
            break;
        }

        xml = new QXmlStreamReader (file);
        while(!xml->atEnd() && !xml->hasError()) {

            /* Read next element.*/
            QXmlStreamReader::TokenType token = xml->readNext();

            /* If token is just StartDocument, we'll go to next.*/
            if (token == QXmlStreamReader::StartDocument) {
                continue;
            }

            /* If token is StartElement, we'll see if we can read it.*/
            if (token == QXmlStreamReader::StartElement) {
                /* If it's named persons, we'll go to the next.*/
                if (xml->name() == "piles") {
                    continue;
                }

                /* If it's named person, we'll dig the information from there.*/
                if (xml->name() == "pile") {

                    QXmlStreamAttributes attributes = xml->attributes();
                    if(attributes.hasAttribute("source")) {
                        Pile * pl = new Pile (
                                    this,
                                    attributes.value("source").toString());
                        addPile (pl);
                    } else {
                        // TODO message?
                    }
                }
            }
        }

        b_ret = xml->hasError();
        break;
    }

    if (xml != NULL) {
        if (xml->hasError()) {
            // TODO message
            // xml->errorString();
        }
        xml->clear();
        delete xml;
    }

    if (file != NULL) {
        file->close ();
        delete file;
    }

    return b_ret;
}

bool PileProvider::rescanTextFile (const QString & source)
{
    QFile inputFile (source);
    if (inputFile.open (QIODevice::ReadOnly))
    {
        QTextStream in (&inputFile);
        while ( !in.atEnd() )
        {
            QString line = in.readLine().trimmed ();
            if (!line.isEmpty () && !line.startsWith ("#")) {
                Pile * pl = new Pile (this, line);
                addPile (pl);
            }
        }
        inputFile.close();
        return true;
    } else {
        // TODO message?
        return false;
    }
}







bool PileProviderText::rescan()
{
    return rescanTextFile (source_);
}

bool PileProviderText::save(QSettings &s)
{
    s.value (SETT_VALUE, source_);
    return true;
}

PileProvider *PileProviderText::fromSettings(
        PileContainer *container, QSettings &s)
{
    PileProviderText * ret = NULL;
    for (;;) {
        QString s_value = s.value (SETT_VALUE).toString ();
        if (s_value.isEmpty ()) break;
        ret = new PileProviderText (s_value);
        container->addProvider (ret);
    }

    return ret;
}



bool PileProviderXml::rescan()
{
    return rescanXMLFile (source_);
}

bool PileProviderXml::save(QSettings &s)
{
    s.value (SETT_VALUE, source_);
    return true;
}

PileProvider *PileProviderXml::fromSettings(
        PileContainer *container, QSettings &s)
{
    PileProviderXml * ret = NULL;
    for (;;) {
        QString s_value = s.value (SETT_VALUE).toString ();
        if (s_value.isEmpty ()) break;
        ret = new PileProviderXml (s_value);
        container->addProvider (ret);
    }

    return ret;
}




bool PileProviderDir::rescan()
{
    QDir d (source_);
    d.setFilter (QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    foreach (const QString & s, d.entryList ()) {
        QString s_path = QString("%1/%2/.git").arg(source_).arg(s);
        if (d.exists (s_path )) {
            Pile * pl = new Pile (
                        this,
                        s_path);
            addPile (pl);
        }
    }
    return true;
}

bool PileProviderDir::save(QSettings &s)
{
    s.value (SETT_VALUE, source_);
    return true;
}

PileProvider *PileProviderDir::fromSettings(
        PileContainer *container, QSettings &s)
{
    PileProviderDir * ret = NULL;
    for (;;) {
        QString s_value = s.value (SETT_VALUE).toString ();
        if (s_value.isEmpty ()) break;
        ret = new PileProviderDir (s_value);
        container->addProvider (ret);
    }

    return ret;
}


bool PileProviderRemote::rescan()
{
    return PilesGui::downloader ()->doDownload (
                source_, QString(), rescanDone, this);
}

bool isXML (QFile * inputFile)
{
    QTextStream in (inputFile);
    QString line = in.readLine().trimmed ();
    if (line.startsWith ("<?xml")) {
        return true;
    } else {
        return false;
    }
}

void PileProviderRemote::rescanDone (
        const QString & s_downloaded_file, void * user)
{
    PileProviderRemote * trg = (PileProviderRemote*)user;
    bool b_ret = false;
    bool b_xml;

    QFile inputFile (s_downloaded_file);
    if (inputFile.open (QIODevice::ReadOnly))
    {
        b_xml = isXML (&inputFile);
        inputFile.close();
        if (b_xml) {
            b_ret = trg->rescanXMLFile (
                        s_downloaded_file);
        } else {
            b_ret = trg->rescanTextFile (
                        s_downloaded_file);
        }
    } else {
        // TODO message?
    }
    b_ret = !b_ret;
}

bool PileProviderRemote::save(QSettings &s)
{
    s.value (SETT_VALUE, source_);
    return true;
}

PileProvider *PileProviderRemote::fromSettings(
        PileContainer *container, QSettings &s)
{
    PileProviderRemote * ret = NULL;
    for (;;) {
        QString s_value = s.value (SETT_VALUE).toString ();
        if (s_value.isEmpty ()) break;
        ret = new PileProviderRemote (s_value);
        container->addProvider (ret);
    }

    return ret;
}



bool PileProviderUser::rescan()
{
    return true;
}

bool PileProviderUser::save(QSettings &s)
{
    s.beginWriteArray (SETT_SUBCOMP, sl_piles_.count ());

    int i = 0;
    foreach (const QString & iter, sl_piles_) {
        s.setArrayIndex (i);
        s.setValue (SETT_VALUE, iter);
        ++i;
    }

    s.endArray ();
    return true;
}

PileProvider *PileProviderUser::fromSettings(
        PileContainer *container, QSettings &s)
{
    PileProvider * ret = NULL;

    ret = new PileProviderUser ();
    container->addProvider (ret);

    int i_max =s.beginReadArray (SETT_SUBCOMP);
    for (int i = 0; i < i_max; ++i) {
        s.setArrayIndex (i);

        QString s_val = s.value (SETT_VALUE).toString();
        if (!s_val.isEmpty ()) {
            Pile * pl = new Pile (
                        ret,
                        s_val);
            ret->addPile (pl);
        }
    }
    s.endArray ();
    return ret;
}
