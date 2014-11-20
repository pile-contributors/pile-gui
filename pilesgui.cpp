#include "pilesgui.h"
#include "mainwindow.h"
#include "downloader.h"
#include "logic/pilecontainer.h"
#include "config.h"
#include "setting_names.h"

#include <QMessageBox>
#include <QApplication>
#include <QSettings>

#ifdef PILESGUI_WINDOWS
#   include <windows.h>
#   include <Fcntl.h>
#   include <io.h>
#endif // PILESGUI_WINDOWS

PilesGui * PilesGui::uniq_ = NULL;


PilesGui::PilesGui(QObject *parent) :
    QObject(parent),
    w(NULL),
    d(NULL),
    pc(NULL),
    envp_(QProcessEnvironment::systemEnvironment())
{   
    uniq_ = this;
}

PilesGui::~PilesGui()
{
}

bool PilesGui::start()
{
    QSettings stg;

#   ifdef PILESGUI_DEBUG
#   ifdef PILESGUI_WINDOWS
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    // DEBUG
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    int hCrt;
    FILE *hf;

    AllocConsole();
    hCrt = _open_osfhandle(
                (long) GetStdHandle(STD_OUTPUT_HANDLE),
                _O_TEXT
                );
    hf = _fdopen(hCrt, "w");
    *stdout = *hf;
    /*int i = */setvbuf(stdout, NULL, _IONBF, 0);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#   endif // PILESGUI_WINDOWS
#   endif // PILESGUI_DEBUG


    d = new Downloader (this);

    pc = new PileContainer();
    pc->loadFromSettings();

    w = new MainWindow();
    w->show();
    w->afterShown();

    QString s_env_file = stg.value (STG_ENVAR_DB).toString ();
    if (!s_env_file.isEmpty ()) {
        // TODO

    }


    return true;
}

void PilesGui::stop()
{
    pc->saveToSettings();
}

void PilesGui::showError (const QString & s_message)
{
    QMessageBox::warning (
                QApplication::activeWindow (),
                tr("Error"),
                s_message);
}

void PilesGui::loadEnvironment(const QProcessEnvironment &value)
{
    foreach(const QString & k, value.keys ()) {
        qputenv (k.toLatin1 ().constData (), value.value (k).toLatin1 ());
    }
}

void PilesGui::clearEnvironment(const QProcessEnvironment &value)
{
    foreach(const QString & k, value.keys ()) {
        qunsetenv (k.toLatin1 ().constData ());
    }
}

void PilesGui::setProcessEnvironment(QProcessEnvironment &value)
{
    uniq_->clearEnvironment (uniq_->envp_);
    uniq_->loadEnvironment (value);

    uniq_->envp_ = value;
}

