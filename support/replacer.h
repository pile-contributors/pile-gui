#ifndef REPLACER_H
#define REPLACER_H

#include <QString>
#include <QStringList>

class Replacer
{
public:
    Replacer();

    static bool
    inFiles (
            const QString & s_path,
            bool recursive,
            const QStringList & s_to_search,
            const QStringList & s_to_replace,
            Qt::CaseSensitivity cs = Qt::CaseSensitive,
            const QStringList & sl_filters = QStringList());

    static bool
    inFiles (
            const QString & s_path_in,
            const QString & s_path_out,
            bool recursive,
            const QStringList & s_to_search,
            const QStringList & s_to_replace,
            Qt::CaseSensitivity cs = Qt::CaseSensitive,
            const QStringList & sl_filters = QStringList());

};

#endif // REPLACER_H
