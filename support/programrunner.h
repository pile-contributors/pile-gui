#ifndef PROGRAMRUNNER_H
#define PROGRAMRUNNER_H

#include <QObject>
#include <QProcess>

class QProcess;

class ProgramRunner : public QObject
{
    Q_OBJECT

public:

    typedef void (*kbFinal) (
            int process_exit_code,
            const QString & s_output,
            const QString & s_error,
            const QString & s_combined,
            void * kb_data);

private:

    QProcess my_process;
    kbFinal kb;
    void * kb_data_;
    QString s_output;
    QString s_error;
    QString s_combined;

public:

    explicit ProgramRunner(
            QObject *parent = 0);

    void
    terminate ();

signals:

public slots:

    void
    readStandardOutput();

    void
    readStandardError();

    void
    finished (
            int exitCode,
            QProcess::ExitStatus exitStatus);

    void
    startedInProc ();

    void
    errorInProc(
            QProcess::ProcessError value);

    void
    stateChanged(
            QProcess::ProcessState value);

public:

    static ProgramRunner *
    startProgram (
            const QString & program,
            const QStringList & arguments,
            const QString & working_dir,
            kbFinal kb,
            void * kb_data = NULL);

    static const char *
    errorName(
            QProcess::ProcessError value);

    static const char *
    stateName(
            QProcess::ProcessState value);

    static void
    startProgramOrOpenFile (
            const QString & program);

};

#endif // PROGRAMRUNNER_H
