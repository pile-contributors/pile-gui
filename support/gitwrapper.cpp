#include "../config.h"

#include "../support/simplecrypt.h"

#include "../pilesgui.h"
#include "../setting_names.h"

#include "gitwrapper.h"
#include "programrunner.h"
#include "waiter.h"

#include <QSettings>
#include <QDebug>
#include <QFileInfo>

const QString s_github_org = "pile-contributors";
const QString s_url_org    = "http://pile-contributors.github.io/";

#if DEBUG_ON
#    define STGS_DEBUGM printf
#else
#    define STGS_DEBUGM black_hole
#endif

#if DEBUG_ON
#    define STGS_TRACE_ENTRY printf("GITW ENTRY %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#    define STGS_TRACE_ENTRY
#endif

#if DEBUG_ON
#    define STGS_TRACE_EXIT printf("GITW EXIT %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#    define STGS_TRACE_EXIT
#endif


GitWrapper::GitWrapper() :
    process_exit_code_(-999),
    s_output_(),
    s_error_(),
    s_combined_(),
    b_ready(false),
    s_error_message_(),
    hub_mode_(false)
{
    STGS_TRACE_ENTRY;
    STGS_TRACE_EXIT;
}

GitWrapper::~GitWrapper()
{
    STGS_TRACE_ENTRY;
    STGS_TRACE_EXIT;
}

QString GitWrapper::getGit()
{
    STGS_TRACE_ENTRY;
    QSettings stg;
    QString s_git = stg.value (STG_APPS_GIT).toString ();
    if (s_git.isEmpty ()) {
        PilesGui::showError (
                    QObject::tr("Git executable not set"));
    }
    STGS_TRACE_EXIT;
    return s_git;
}

QString GitWrapper::getHub()
{
    STGS_TRACE_ENTRY;
    QSettings stg;
    QString s_git = stg.value (STG_APPS_HUB).toString ();
    if (s_git.isEmpty ()) {
        PilesGui::showError (
                    QObject::tr("Hub executable not set"));
    }
    STGS_TRACE_EXIT;
    return s_git;
}

bool GitWrapper::hasHub()
{
    QSettings stg;
    QString s_git = stg.value (STG_APPS_HUB).toString ();
    if (s_git.isEmpty ()) return false;
    QFileInfo fi (s_git);
    return (fi.exists () && fi.isExecutable ());
}

QString GitWrapper::getGitBackup()
{
    QSettings stg;
    QString s_git = stg.value (STG_BACKUP_PATH).toString ();
    return s_git;
}

bool GitWrapper::addAll(
        const QString & s_path)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {

        GitWrapper wrap_git;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to add files to the Git stage\n");

        QStringList args;
        args << "add"
             << "*"
             << "--all";

        b_ret = wrap_git.doAjob (s_path, args);
        break;
    }
    STGS_TRACE_EXIT;
    return b_ret;
}

const QString & GitWrapper::gitHubOrg ()
{
    return s_github_org;
}

const QString & GitWrapper::urlOrg ()
{
    return s_github_org;
}

bool GitWrapper::createGitHubRepo (
        const QString & s_path,
        const QString &s_name, const QString &s_org,
        const QString &s_description, const QString &s_info_url)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {

        GitWrapper wrap_git;
        wrap_git.hub_mode_ = true;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to create Github repository\n");
        QStringList args;
        args << "create";

        if (s_org.isEmpty ()) {
            args << s_name;
        } else {
            args << QString("%1/%2").arg (s_org).arg(s_name);
        }

        if (!s_description.isEmpty ()) {
            args << "-d" << s_description;
        }

        if (!s_info_url.isEmpty ()) {
            args << "-h" << s_info_url;
        }

        b_ret = wrap_git.doAjob (s_path, args);
        b_ret = true;
        break;
    }

    STGS_TRACE_EXIT;
    return b_ret;
}

bool GitWrapper::commit(
        const QString & s_path, const QString &s_message,
        bool b_add_before)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {

        if (b_add_before) {
            if (!addAll (s_path)) break;
        }

        GitWrapper wrap_git;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to commit\n");

        QStringList args;
        args << "commit"
                    << "-m"
                    << QString("\"%1\"").arg (s_message);

        b_ret = wrap_git.doAjob (s_path, args);
        break;
    }

    STGS_TRACE_EXIT;
    return b_ret;
}

bool GitWrapper::push (
        const QString & s_path, const QString & s_remote,
        const QString & s_branch)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {

        GitWrapper wrap_git;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to push\n");

        QStringList args;
        args << "push";
        if (s_remote.isEmpty ()) {
            args << "--all";
        } else {
            args << s_remote;
            args << s_branch;
        }

        b_ret = wrap_git.doAjob (s_path, args);
        break;
    }

    STGS_TRACE_EXIT;
    return b_ret;
}

bool GitWrapper::init (
        const QString & s_path, bool b_bare)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {

        GitWrapper wrap_git;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to initialize new repository\n");

        QStringList args;
        args << "init";
        if (b_bare) {
            args << "--bare";
        }
        args << ".";

        b_ret = wrap_git.doAjob (s_path, args);
        break;
    }

    STGS_TRACE_EXIT;
    return b_ret;
}

bool GitWrapper::addRemote (
        const QString & s_path, const QString & s_name,
        const QString & s_remote_path)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {
        GitWrapper wrap_git;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to add remote\n");

        QStringList args;
        args << "remote"
             << "add"
             << s_name
             << s_remote_path;

        b_ret = wrap_git.doAjob (s_path, args);
        break;
    }

    STGS_TRACE_EXIT;
    return b_ret;
}

bool GitWrapper::setUpstream (
        const QString & s_path, const QString & s_name,
        const QString & s_ref)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {
        GitWrapper wrap_git;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to set upstream\n");

        QStringList args;
        args << "push"
             << "--set-upstream"
             << s_name
             << s_ref;

        b_ret = wrap_git.doAjob (s_path, args);
        break;
    }

    STGS_TRACE_EXIT;
    return b_ret;
}


bool GitWrapper::addSubmodule (
        const QString & s_path, const QString & s_subm_path,
        const QString & s_dir_name, const QString & s_message)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    for (;;) {
        GitWrapper wrap_git;
        wrap_git.s_error_message_ = QObject::tr(
                    "Failed to add submodule\n");

        QStringList args;
        args << "submodule"
             << "--quiet"
             << "add"
             << "--force"
             << s_subm_path
             << s_dir_name;

        b_ret = wrap_git.doAjob (s_path, args);
        if (!b_ret) break;

        b_ret = commit (s_path, s_message, true);
        break;
    }

    STGS_TRACE_EXIT;
    return b_ret;
}

void GitWrapper::afterGeneric (
        int process_exit_code, const QString & s_output,
        const QString & s_error, const QString & s_combined,
        void * kb_data)
{
    STGS_TRACE_ENTRY;
    GitWrapper * gw = (GitWrapper *)kb_data;
    gw->process_exit_code_ = process_exit_code;
    gw->s_output_ = s_output;
    gw->s_error_ = s_error;
    gw->s_combined_ = s_combined;

    if (process_exit_code != 0) {
        PilesGui::showError (
                    QString("%1\n%2")
                    .arg(gw->s_error_message_)
                    .arg(s_error));
    }
    qDebug () << gw->s_combined_;
    gw->b_ready = true;
    STGS_TRACE_EXIT;
}

bool GitWrapper::doAjob (
        const QString & s_path, const QStringList & args)
{
    STGS_TRACE_ENTRY;
    bool b_ret = false;
    ProgramRunner * prog_run;
    for (;;) {

        QString s_git;
        QMap<QString,QString> env;

        if (hub_mode_) {
            QSettings stg;
            SimpleCrypt crypto (SIMPLE_CRYPT_KEY);
            QString s_pass = crypto.decryptToString (
                        stg.value (STG_GITHUB_PASS).toString ());
            s_git  = getHub ();
            env.insert ("GITHUB_USER", stg.value (STG_GITHUB_USER).toString ());
            env.insert ("GITHUB_PASSWORD", s_pass);
        } else {
            s_git  = getGit ();
        }
        STGS_DEBUGM("  app: %s\n", TMP_A(s_git));
        STGS_DEBUGM("  args: %s\n", TMP_A(args.join (" ")));

        if (s_git.isEmpty ()) break;

        prog_run = ProgramRunner::startProgram(
                    s_git, args, s_path,
                    afterGeneric, this);
        if (prog_run == NULL) {
            PilesGui::showError (
                        QObject::tr ("Unable to start Git"));
            break;
        }

        if (!Waiter::waitForBoolean (SEC_TO_MSEC(48),
                                     &b_ready)) {
            STGS_DEBUGM("  waitForBoolean fails\n");

            prog_run->terminate ();
            PilesGui::showError (
                        QObject::tr ("%1\nProgram timed out in command\n%2")
                        .arg(s_error_message_)
                        .arg (args.join (" ")));
            break;
        }

        b_ret = b_ready;
        break;
    }
    //    if (prog_run != NULL) {
    //        prog_run->terminate ();
    //    }
    STGS_TRACE_EXIT;
    return b_ret;

}
