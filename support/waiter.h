#ifndef WAITER_H
#define WAITER_H

#include <QObject>
#include <QTime>
#include <QEventLoop>

class Waiter : public QObject
{
    Q_OBJECT

public:

    typedef bool (*kbAfter) (Waiter *, void *);

private:
    QEventLoop event_loop;
    QTime t_end_;
    kbAfter kb_;
    void * kb_data_;

public:
    explicit Waiter(
            kbAfter kb,
            void * kb_data = NULL,
            int timeout_msec = -1,
            QObject *parent = 0);

signals:

private slots:

    virtual void
    timerEvent (
            QTimerEvent *);

public:

    //! waits for a boolean variable to become true
    ///
    /// If the variable is initially true no waiting takes place.
    /// The minimum resolution in software for the timeout is
    /// 10 miliseconds.
    ///
    /// @return true if the variable became true, false if the timeout expitred
    static bool
    waitForBoolean (
            int timeout_msec,
            bool * b_var);

private:

    void
    run ();

};

#endif // WAITER_H
