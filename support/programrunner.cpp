#include "programrunner.h"

#include <QDebug>

ProgramRunner::ProgramRunner(QObject *parent) :
    QObject(parent),
    my_process(),
    kb(NULL),
    kb_data_(NULL),
    s_output(),
    s_error(),
    s_combined()
{
}

void ProgramRunner::terminate()
{
    qDebug() << "ProgramRunner - terminate - error: " << errorName (my_process.error ()) << my_process.errorString ().toUtf8 ().constData ();
    qDebug() << "ProgramRunner - terminate - state: " << stateName (my_process.state ());

    my_process.terminate ();
    kb = NULL;
    kb_data_ = NULL;
    deleteLater ();
}

ProgramRunner *ProgramRunner::startProgram(
        const QString & program, const QStringList & arguments,
        const QString & working_dir, kbFinal kb, void * kb_data)
{
    ProgramRunner * pr = new ProgramRunner();
    pr->kb = kb;
    pr->kb_data_ = kb_data;

    connect(&pr->my_process, SIGNAL(readyReadStandardOutput()),
            pr, SLOT(readStandardOutput()));
    connect(&pr->my_process, SIGNAL(readyReadStandardError()),
            pr, SLOT(readStandardError()));
    connect(&pr->my_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            pr, SLOT(finished(int, QProcess::ExitStatus)));
    connect(&pr->my_process, SIGNAL(started()),
            pr, SLOT(startedInProc()));
    connect(&pr->my_process, SIGNAL(error(QProcess::ProcessError)),
            pr, SLOT(errorInProc(QProcess::ProcessError)));
    connect(&pr->my_process, SIGNAL(stateChanged(QProcess::ProcessState)),
            pr, SLOT(stateChanged(QProcess::ProcessState)));


    pr->my_process.setProgram (program);
    pr->my_process.setArguments (arguments);

    if (!working_dir.isEmpty ()) {
        pr->my_process.setWorkingDirectory (working_dir);
    }

    QProcessEnvironment env =
            QProcessEnvironment::systemEnvironment();
    env.insert("TRICKY", "PileGui");
    pr->my_process.setProcessEnvironment (env);

    // pr->my_process.closeWriteChannel();
    pr->my_process.start();
    return pr;
}

void ProgramRunner::readStandardOutput()
{
    QByteArray processOutput;
    processOutput = my_process.readAllStandardOutput();

    QString s(processOutput);
    s_output.append (s);
    s_combined.append (s);

    qDebug() << "ProgramRunner - readStandardOutput: " << s.toUtf8 ().constData ();
}

void ProgramRunner::readStandardError()
{
    QByteArray processOutput;
    processOutput = my_process.readAllStandardError ();

    QString s(processOutput);
    s_error.append (s);
    s_combined.append (s);

    qDebug() << "ProgramRunner - readStandardError: " << s.toUtf8 ().constData ();
}

void ProgramRunner::finished (
        int exitCode, QProcess::ExitStatus )
{
    if (kb != NULL) {
        kb (exitCode, s_output, s_error, s_combined, kb_data_);
    }
    deleteLater ();

    qDebug() << "ProgramRunner - finished: " << exitCode;
}

void ProgramRunner::startedInProc()
{
    qDebug() << "ProgramRunner - started: " << (unsigned long long)this;
}

void ProgramRunner::errorInProc(QProcess::ProcessError value)
{
    qDebug() << "ProgramRunner - errorInProc: " << errorName (value);
}

void ProgramRunner::stateChanged(QProcess::ProcessState value)
{
    qDebug() << "ProgramRunner - stateChanged: " << stateName(value);
}

const char * ProgramRunner::errorName(QProcess::ProcessError value)
{
    const char * reason = "unknown";
    switch (value) {
    case QProcess::FailedToStart: reason = "file not found, resource error"; break;
    case QProcess::Crashed: reason = "crashed"; break;
    case QProcess::Timedout: reason = "Timedout"; break;
    case QProcess::ReadError: reason = "ReadError"; break;
    case QProcess::WriteError: reason = "WriteError"; break;
    case QProcess::UnknownError: reason = "UnknownError"; break;
    }
    return reason;
}

const char * ProgramRunner::stateName(QProcess::ProcessState value)
{
    const char * state_name = "unknown";
    switch (value) {
    case QProcess::NotRunning: state_name = "NotRunning"; break;
    case QProcess::Starting: state_name = "Starting"; break;
    case QProcess::Running: state_name = "Running"; break;
    }
    return state_name;
}

