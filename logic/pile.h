#ifndef PILE_H
#define PILE_H

#include <QUrl>

class PileProvider;

//! Represents a pile in our program
class Pile
{
public:

    typedef void (*kbStatus) (const QString & s_message);

private:

    QUrl url_; /**< the address where we can find this pile */
    PileProvider * parent_; /**< the provider */

    static void
    reportStat(
            kbStatus kb_sts,
            const QString &s_msg);

public:

    //! constructor
    Pile (
            PileProvider * parent,
            const QString & s_url);

    //! constructor
    Pile (
            PileProvider * parent,
            const QUrl & url);

    //! destructor
    virtual ~Pile();

    //! get the parent provider
    PileProvider *
    parent () const {
        return parent_;
    }


    static bool
    createAllNew (
            const QString & s_name,
            const QString & s_out_path,
            const QString & s_template_path,
            QStringList & errors,
            kbStatus kb_sts);


};

#endif // PILE_H
