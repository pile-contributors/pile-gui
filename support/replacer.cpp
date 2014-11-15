#include "replacer.h"
#include "../pilesgui.h"
#include "../config.h"

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QStack>
#include <QFileInfo>
#include <QDir>

#include <iostream>

#if DEBUG_THISFILE_ON
#define REPLACER_DBG DBG_PMESSAGE
#else
#define REPLACER_DBG black_hole
#endif

static QString replaceInString (
        const QString & s, const QStringList & sl, const QStringList & sl_repl)
{
    QString ret = s;
    int i_max = sl.count();
    for (int i = 0; i < i_max; ++i) {
        ret.replace (sl.at (i), sl_repl.at (i));
    }
    return ret;
}

bool Replacer::inFiles(
        const QString & s_path_in, const QString & s_path_out,
        bool recursive,
        const QStringList &s_to_search, const QStringList &s_to_replace,
        Qt::CaseSensitivity cs, const QStringList &s_filters)
{
    REPLACER_DBG("Replacing and copying at the same time\n");
    REPLACER_DBG(" - s_path_in: %s\n", TMP_A(s_path_in));
    REPLACER_DBG(" - s_path_out: %s\n", TMP_A(s_path_out));
    REPLACER_DBG(" - recursive: %s\n", recursive ? "true" : "false");
    REPLACER_DBG(" - case sensitive: %s\n", cs == Qt::CaseSensitive ? "true" : "false");
    REPLACER_DBG(" - s_to_search: %d\n", s_to_search.count ());
    REPLACER_DBG(" - s_to_replace: %d\n", s_to_replace.count ());
    REPLACER_DBG(" - s_filters: %d\n", s_filters.count ());

    int i_pattern_count = s_to_search.count ();
    if (i_pattern_count != s_to_replace.count ()) {
        PilesGui::showError (
                    QObject::tr(
                        "Bad replacement (mismatched count)"));
        return false;
    }
    if (i_pattern_count == 0) {
        PilesGui::showError (
                    QObject::tr(
                        "Bad replacement (nothng to replace)"));
        return false;
    }

    QDir d_out (s_path_out);
    if (!d_out.exists ()) {
        d_out.mkpath ("."); // result is not reliable for some reason
        if (!d_out.exists ()) {
            PilesGui::showError (
                        QObject::tr(
                            "Unable to create output directory: %1")
                        .arg (d_out.absolutePath ()));
            return false;
        }
    }

    QStringList filters (s_filters);
    if(filters.count () == 0){
        filters << "*.*";
        REPLACER_DBG("   uses default filter\n");
    }

    QStack<QString> dirs;
    dirs.push (s_path_in);

    QString s_relative;
    QString s_relative_out;

    // continue if there are sub directory
    while (dirs.size() > 0) {

        QDir currentDir (dirs.pop());
        REPLACER_DBG("   directory: %s\n", TMP_A(currentDir.path ()));

        s_relative = currentDir.path ();
        s_relative.replace (s_path_in, "");
        if (s_relative.startsWith ("/")) {
            s_relative = s_relative.mid (1);
        }
        REPLACER_DBG("   relative: %s\n", TMP_A(s_relative));

        if (!s_relative.isEmpty ()) {
            s_relative_out = replaceInString (
                        s_relative, s_to_search, s_to_replace);
            if (!d_out.exists (s_relative_out)) {
                d_out.mkdir (s_relative_out);
                if (!d_out.exists (s_relative_out)) {
                    PilesGui::showError (
                                QObject::tr(
                                    "Unable to create output directory (relative): %1")
                                .arg (d_out.absoluteFilePath (s_relative_out)));
                    return false;
                }
            }
        }

        QStringList child = currentDir.entryList (
                    filters, QDir::NoDotAndDotDot | QDir::Files);
        REPLACER_DBG("   files found: %d\n", child.count ());

        QStringList::const_iterator itbegin = child.begin();
        QStringList::const_iterator itend = child.end();
        for(; itbegin != itend ; ++itbegin) {

            if (*itbegin == ".git") continue;


            const QString nextFile (currentDir.path() + "/" + (*itbegin));
            REPLACER_DBG("   file: %s\n", TMP_A(nextFile));

            s_relative = nextFile;
            s_relative.replace (s_path_in, "");
            if (s_relative.startsWith ("/")) {
                s_relative = s_relative.mid (1);
            }
            REPLACER_DBG("     relative: %s\n", TMP_A(s_relative));

            s_relative_out = replaceInString (
                        s_relative, s_to_search, s_to_replace);

            QString oldContent;
            {
                QFile inFile (nextFile);
                if(!inFile.open (QIODevice::ReadOnly)){
                    PilesGui::showError (
                                QObject::tr(
                                    "Unable to open input file: %1")
                                .arg (nextFile));
                    return false;
                }
                QTextStream inStream (&inFile);
                oldContent = inStream.readAll();
                inFile.close();
            }
            REPLACER_DBG("     content size: %d\n", oldContent.count ());

            QString newContent = oldContent;
            for (int i = 0; i < i_pattern_count; ++i ) {
                newContent = newContent.replace (
                            s_to_search.at(i),
                            s_to_replace.at(i),
                            cs);
            }
            REPLACER_DBG("     after replacement: %d\n", newContent.count ());

            {

                QFile inFile (d_out.absoluteFilePath (s_relative_out));
                if(!inFile.open (QIODevice::WriteOnly)) {
                    PilesGui::showError (
                                QObject::tr(
                                    "Unable to write output file: %1")
                                .arg (inFile.fileName ()));
                    return false;
                }
                QTextStream outStream (&inFile);
                outStream << newContent;
                inFile.close();
            }
            REPLACER_DBG("     done with this file\n");
        }

        if (recursive) {
            QStringList childdir =  currentDir.entryList (
                        QDir::NoDotAndDotDot | QDir::Dirs);
            REPLACER_DBG("   sub-directories found: %d\n", childdir.count ());

            QStringList::const_iterator itbegindir = childdir.begin();
            QStringList::const_iterator itenddir = childdir.end();
            for(; itbegindir != itenddir ; ++itbegindir){
                if (*itbegindir == ".git") continue;
                if (*itbegindir == ".svn") continue;
                dirs.push (currentDir.path() + "/" + (*itbegindir));
                REPLACER_DBG("     adding %s to the stack\n", TMP_A(dirs.first ()));
            }
        }
    }

    REPLACER_DBG("Replacing and copying at the same time was succesfull");
    return true;
}

bool Replacer::inFiles(
        const QString &s_path, bool recursive,
        const QStringList &s_to_search, const QStringList &s_to_replace,
        Qt::CaseSensitivity cs, const QStringList &s_filters)
{
    int i_pattern_count = s_to_search.count ();
    if (i_pattern_count != s_to_replace.count ()) {
        //
        return false;
    }
    if (i_pattern_count == 0) {
        //
        return false;
    }

    QStringList filters (s_filters);
    if(filters.count () == 0){
        filters << "*.*";
    }

    QStack<QString> dirs;
    dirs.push (s_path);

    // continue if there are sub directory
    while (dirs.size() > 0) {

        QDir currentDir (dirs.pop());
        QStringList child =  currentDir.entryList (filters);

        QStringList::const_iterator itbegin = child.begin();
        QStringList::const_iterator itend = child.end();
        for(; itbegin != itend ; ++itbegin){

            const QString nextFile (currentDir.path() + "/" + (*itbegin));

            QString oldContent;
            {
                QFile inFile (nextFile);
                if(!inFile.open (QIODevice::ReadOnly)){
                    //std::cout << "Cannot open " << (const char*)nextFile.toAscii() << "\n";
                    return false;
                }
                QTextStream inStream (&inFile);
                oldContent = inStream.readAll();
                inFile.close();
            }

            QString newContent = oldContent;
            for (int i = 0; i < i_pattern_count; ++i ) {
                newContent = newContent.replace (
                            s_to_search.at(i),
                            s_to_replace.at(i),
                            cs);
            }

            {
                QFile inFile (nextFile);
                if(!inFile.open (QIODevice::WriteOnly)) {
                    // std::cout << "Cannot open to write " << (const char*)nextFile.toAscii() << "\n";
                    return false;
                }
                QTextStream outStream (&inFile);
                outStream << newContent;
                inFile.close();
            }
        }

        if (recursive) {
            QStringList childdir =  currentDir.entryList (
                        QDir::NoDotAndDotDot | QDir::Dirs);
            QStringList::const_iterator itbegindir = childdir.begin();
            QStringList::const_iterator itenddir = childdir.end();
            for(; itbegindir != itenddir ; ++itbegindir){
                if (*itbegindir == ".git") continue;
                if (*itbegindir == ".svn") continue;
                dirs.push (currentDir.path() + "/" + (*itbegindir));
                // std::cout << "Add " << (const char*)(currentDir.path() + "/" + (*itbegindir)).toAscii() << std::endl;
            }
        }
    }

    return true;
}




