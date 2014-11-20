#ifndef PILESGUI_H
#define PILESGUI_H

#include <QObject>
#include <QProcessEnvironment>

class MainWindow;
class Downloader;
class PileContainer;

class PilesGui : public QObject
{
    Q_OBJECT
public:
    explicit PilesGui(QObject *parent = 0);

    virtual ~PilesGui();

    bool
    start();

    void
    stop();

    static MainWindow *
    mainWindow ()
    { return uniq_->w; }

    static Downloader *
    downloader ()
    { return uniq_->d; }

    static PileContainer *
    pileCont ()
    { return uniq_->pc; }

    static void
    showError (
            const QString &s_message);

    static QProcessEnvironment &
    processEnvironment () {
        return envp_;
    }


signals:

public slots:

private:
    MainWindow * w;
    Downloader * d;
    PileContainer * pc;
    QProcessEnvironment envp_;
    static PilesGui * uniq_;
};

#endif // PILESGUI_H
