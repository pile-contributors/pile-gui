#include "../config.h"
#include "../pilesgui.h"
#include "../setting_names.h"

#include "gitwrapper.h"
#include "programrunner.h"
#include "waiter.h"

#include <QSettings>
#include <QDebug>


GitWrapper::GitWrapper() :
    process_exit_code_(-999),
    s_output_(),
    s_error_(),
    s_combined_(),
    b_ready(false),
    s_error_message_()
{
    qDebug() << "Creating GitWrapper " << (unsigned long long)this << "\n";
}

GitWrapper::~GitWrapper()
{
    qDebug() << "Destroying GitWrapper " << (unsigned long long)this << "\n";
}

QString GitWrapper::getGit()
{
    QSettings stg;
    QString s_git = stg.value (STG_APPS_GIT).toString ();
    if (s_git.isEmpty ()) {
        PilesGui::showError (
                    QObject::tr("Git executable not set"));
    }
    return s_git;
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
    return b_ret;
}

bool GitWrapper::commit(
        const QString & s_path, const QString &s_message,
        bool b_add_before)
{
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

    return b_ret;
}

bool GitWrapper::push (
        const QString & s_path, const QString & s_remote)
{
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
        }

        b_ret = wrap_git.doAjob (s_path, args);
        break;
    }

    return b_ret;
}

bool GitWrapper::init (
        const QString & s_path, bool b_bare)
{
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

    return b_ret;
}

bool GitWrapper::addRemote (
        const QString & s_path, const QString & s_name,
        const QString & s_remote_path)
{
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

    return b_ret;
}

bool GitWrapper::setUpstream (
        const QString & s_path, const QString & s_name,
        const QString & s_ref)
{
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

    return b_ret;
}


bool GitWrapper::addSubmodule (
        const QString & s_path, const QString & s_subm_path,
        const QString & s_dir_name, const QString & s_message)
{
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

    return b_ret;
}

void GitWrapper::afterGeneric (
        int process_exit_code, const QString & s_output,
        const QString & s_error, const QString & s_combined,
        void * kb_data)
{
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
}

bool GitWrapper::doAjob (
        const QString & s_path, const QStringList & args)
{
    bool b_ret = false;
    ProgramRunner * prog_run;
    for (;;) {

        QString s_git = getGit();
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
            prog_run->terminate ();
            PilesGui::showError (
                        QObject::tr ("%1\nGit timed out in command\ngit %2")
                        .arg(s_error_message_)
                        .arg (args.join (" ")));
            break;
        }

        b_ret = true;
        break;
    }
    //    if (prog_run != NULL) {
    //        prog_run->terminate ();
    //    }
    return b_ret;

}
