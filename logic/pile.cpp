#include "pile.h"
#include "../support/replacer.h"
#include "../support/gitwrapper.h"

#include <QDir>

Pile::Pile(PileProvider * p, const QString & s_url) :
    url_(s_url),
    parent_(p)
{
}

Pile::Pile(
        PileProvider * p, const QUrl & url) :
    url_(url),
    parent_(p)
{
}

Pile::~Pile()
{
}

void Pile::reportStat (kbStatus kb_sts, const QString & s_msg)
{
    if (kb_sts != NULL) {
        kb_sts (s_msg);
    }
}


bool Pile::createAllNew(
        const QString &s_name, const QString &s_out_path,
        const QString &s_template_path, QStringList & errors, kbStatus kb_sts)
{
    bool b_ret = false;
    bool b_ret_tmp = false;
    bool b_stage_1 = true;
    for (;;){
#if 0
        QString s_tmpl ("TemPile");
        QStringList sl_keys;
        QStringList sl_repl;

        sl_keys << s_tmpl << s_tmpl.toUpper () << s_tmpl.toLower ();
        sl_repl << s_name << s_name.toUpper () << s_name.toLower ();

        b_ret = Replacer::inFiles (s_template_path, s_out_path, true, sl_keys, sl_repl);
#else
        reportStat (kb_sts, QObject::tr("Checking input ..."));

        // a pile name must be provided
        QString pile_name = s_name.trimmed ();
        QString pile_name_l = pile_name.toLower ();
        QString pile_name_u = pile_name.toUpper ();
        QString pile_helpers_name_l =
                QString("%1_helpers")
                .arg(pile_name_l);
        if (pile_name.isEmpty ()) {
            errors.append (
                        QObject::tr (
                            "A pile name was not provided"));
            b_stage_1 = false;
        }

        // make sure the input directory exists
        QDir d_template (s_template_path);
        if (!d_template.exists()) {
            errors.append (
                        QObject::tr (
                            "Template directory %1 does not exists")
                        .arg (s_template_path));
            b_stage_1 = false;
        }

        // the output directory must exists or must be created
        QDir d_output (s_out_path);
        if (!d_output.exists ()) {
            d_output.mkpath (".");
            if (!d_output.exists ()) {
                errors.append (
                            QObject::tr (
                                "Output directory %1 could not be created")
                            .arg (s_out_path));
                b_stage_1 = false;
            }
        }

        // backup if we have a backup path
        QString s_backup_path = GitWrapper::getGitBackup();
        QDir d_backup (s_backup_path);
        if (!s_backup_path.isEmpty ()) {
            if (!d_backup.exists ()) {
                d_backup.mkpath (".");
                if (!d_backup.exists ()) {
                    errors.append (
                                QObject::tr (
                                    "Backup directory %1 could not be created")
                                .arg (s_backup_path));
                    b_stage_1 = false;
                }
            }
        }

        if (!b_stage_1) break;


        // the template must contain pile-helpers
        QDir d_template_helpers (d_template);
        if (!d_template_helpers.cd ("pile-helpers")) {
            errors.append (
                        QObject::tr (
                            "Template directory %1 must contain a pile-helpers directory")
                        .arg (s_template_path));
            break;
        }

        // the template must contain a pile
        QDir d_template_pile (d_template);
        if (!d_template_pile.cd ("pile")) {
            errors.append (
                        QObject::tr (
                            "Template directory %1 must contain a pile directory")
                        .arg (s_template_path));
            break;
        }

        // we are going to fail if the directories already exist
        if (d_output.exists (pile_name_l) ||
            d_output.exists (pile_helpers_name_l)) {

            errors.append (
                        QObject::tr (
                            "Output directory %1 must not contain the pile directories")
                        .arg (s_out_path));
            break;
        }

        reportStat (kb_sts, QObject::tr("Creating directories ..."));

        // create one directory for the pile
        if (!d_output.mkdir (pile_name_l)) {
            errors.append (
                        QObject::tr (
                            "Pile directory could not be created in %1")
                        .arg (s_out_path));
            break;
        }
        d_output.cd (pile_name_l);

        // create one directory for helper and one for the pile
        if (!d_output.mkdir (pile_name_l) ||
            !d_output.mkdir (pile_helpers_name_l)) {
            errors.append (
                        QObject::tr (
                            "Pile directory could not be created in %1")
                        .arg (s_out_path));
            break;
        }

        reportStat (kb_sts, QObject::tr("Copy & replace in templates ..."));

        // compute the things to replace
        QString s_tmpl ("TemPile");
        QStringList sl_keys;
        QStringList sl_repl;
        sl_keys << s_tmpl << "TEMPILE"   << "tempile";
        sl_repl << s_name << pile_name_u << pile_name_l;

        // full path to output
        QString s_full_path_helpers = d_output.absoluteFilePath (pile_helpers_name_l);

        // replace in helpers
        b_ret_tmp = Replacer::inFiles (
                    d_template_helpers.absolutePath (),
                    s_full_path_helpers,
                    true,
                    sl_keys,
                    sl_repl);
        if (!b_ret_tmp) {
            errors.append (
                        QObject::tr (
                            "Can't generate pile helpers from template"));
            break;
        }

        // replace in pile
        QString s_full_path_pile = d_output.absoluteFilePath (pile_name_l);
        b_ret_tmp = Replacer::inFiles (
                    d_template_pile.absolutePath (),
                    s_full_path_pile,
                    true,
                    sl_keys,
                    sl_repl);
        if (!b_ret_tmp) {
            errors.append (
                        QObject::tr (
                            "Can't generate pile helpers from template"));
            break;
        }

        // initialize empty repository for helpers and pile
        reportStat (kb_sts, QObject::tr("Initialize empty repository ..."));
        if (!GitWrapper::init (s_full_path_helpers)) break;
        if (!GitWrapper::init (s_full_path_pile)) break;

        // add everything in first commit
        reportStat (kb_sts, QObject::tr("First commit ..."));
        if (!GitWrapper::commit (
                    s_full_path_helpers,
                    QObject::tr("Initial commit"),
                    true)) break;
        if (!GitWrapper::commit (
                    s_full_path_pile,
                    QObject::tr("Initial commit"),
                    true)) break;

        // if we have a backup path create backups
        if (!s_backup_path.isEmpty ()) {
            reportStat (kb_sts, QObject::tr("Backup directories ..."));
            if (!d_backup.mkdir (pile_name_l) ||
                !d_backup.mkdir (pile_helpers_name_l)) {
                errors.append (
                            QObject::tr (
                                "Backup directories can't be created"));
                break;
            }
            QString s_backup_pile_full =
                    d_backup.absoluteFilePath (pile_name_l);
            QString s_backup_helpers_full =
                    d_backup.absoluteFilePath (pile_helpers_name_l);

            reportStat (kb_sts, QObject::tr("Initialize backup repositories ..."));
            if (!GitWrapper::init (
                        s_backup_pile_full,
                        true))
                break;
            if (!GitWrapper::init (
                        s_backup_helpers_full,
                        true))
                break;

            reportStat (kb_sts, QObject::tr("Adding remotes ..."));
            if (!GitWrapper::addRemote (
                        s_full_path_helpers,
                        "backup",
                        s_backup_helpers_full))
                break;
            if (!GitWrapper::addRemote (
                        s_full_path_pile,
                        "backup",
                        s_backup_pile_full))
                break;

            reportStat (kb_sts, QObject::tr("Setting upstream ..."));
            if (!GitWrapper::setUpstream (
                        s_full_path_helpers,
                        "backup",
                        "master"))
                break;
            if (!GitWrapper::setUpstream (
                        s_full_path_pile,
                        "backup",
                        "master"))
                break;

            reportStat (kb_sts, QObject::tr("Adding submodules ..."));
            if (!GitWrapper::addSubmodule (
                        s_full_path_helpers,
                        s_backup_pile_full,
                        pile_name_l,
                        QObject::tr("Imported upstream source code")))
                break;

            reportStat (kb_sts, QObject::tr("Publishing changes ..."));
            if (!GitWrapper::push (
                        s_full_path_helpers,
                        "backup"))
                break;
        }

/*
        echo "Adding files in pile source to git..."
        pushd "$FULL_PILE_DIR" > /dev/null
            git add .
            git ci -am "Initial commit"
            git remote add backup "$FULL_BACKUP_PILE_DIR"
            git push --set-upstream backup master
        popd > /dev/null
        echo "... done"

        echo "Adding files in pile helpers source to git..."
        pushd "$FULL_HELPERS_PILE_DIR" > /dev/null
            git add .
            git ci -am "Initial commit"
            git remote add backup "$FULL_BACKUP_HELPERS_PILE_DIR"
            git push --set-upstream backup master
            git submodule add "$FULL_BACKUP_PILE_DIR" "$PILE_NAME-src"
            git add .
            git ci -am "Imported upstream source code"
            git push backup
        popd > /dev/null
        echo "... done"
*/
#endif

        reportStat (kb_sts, QObject::tr("New pile is ready to be used ..."));
        b_ret = true;
        break;
    }

    return b_ret;
}
