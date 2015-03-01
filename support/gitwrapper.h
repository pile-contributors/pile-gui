#ifndef GITWRAPPER_H
#define GITWRAPPER_H

#include <QString>

class GitWrapper
{

    int process_exit_code_;
    QString s_output_;
    QString s_error_;
    QString s_combined_;
    bool b_ready;
    QString s_error_message_;

    bool hub_mode_;

    GitWrapper();
    virtual ~GitWrapper();

public:

    //! shows an error message if empty
    static QString
    getGit();

    //! shows an error message if empty
    static QString
    getHub ();

    //! Tell if hub was found
    static bool
    hasHub ();

    //! location of backups
    static QString
    getGitBackup();



    static bool
    commit (
            const QString & s_path,
            const QString & s_message,
            bool b_add_before = false);

    static bool
    push (
            const QString & s_path,
            const QString & s_remote = QString(),
            const QString & s_branch = "master");

    static bool
    init(
            const QString &s_path,
            bool b_bare = false);

    static bool
    addRemote (
            const QString & s_path,
            const QString & s_name,
            const QString & s_remote_path);

    static bool
    setUpstream (
            const QString & s_path,
            const QString & s_name,
            const QString & s_ref);

    static bool
    addSubmodule (
            const QString & s_path,
            const QString & s_subm_path,
            const QString & s_dir_name,
            const QString & s_message);

    static bool
    addAll(
            const QString &s_path);

    static const QString &
    gitHubOrg ();

    static const QString &
    urlOrg ();

    static bool
    createGitHubRepo (
            const QString & s_path,
            const QString & s_name,
            const QString & s_org = QString(),
            const QString & s_description = QString(),
            const QString & s_info_url = QString());

private:

    static void
    afterGeneric (
            int process_exit_code,
            const QString &s_output,
            const QString &s_error,
            const QString &s_combined,
            void * kb_data);

private:

    bool
    doAjob (
            const QString & s_path,
            const QStringList & args);

};

#endif // GITWRAPPER_H
