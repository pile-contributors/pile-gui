#include "waiter.h"

#include <QTimerEvent>

#if 0
#    define STGS_DEBUGM printf
#else
#    define STGS_DEBUGM black_hole
#endif

#if 0
#    define STGS_TRACE_ENTRY printf("STGS ENTRY %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#    define STGS_TRACE_ENTRY
#endif

#if 0
#    define STGS_TRACE_EXIT printf("STGS EXIT %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#    define STGS_TRACE_EXIT
#endif


Waiter::Waiter (
        kbAfter kb, void * kb_data,
        int timeout_msec, QObject *parent) :
    QObject(parent),
    event_loop(),
    t_end_(QTime::currentTime ()),
    kb_(kb), kb_data_(kb_data)
{
    if (timeout_msec < 0) {
        timeout_msec = 1000 * 60 * 60 * 24 * 7;
    }
    t_end_ = t_end_.addMSecs (timeout_msec);
    startTimer (10);
}

void Waiter::timerEvent (QTimerEvent * te)
{
    for (;;) {
        QTime t (QTime::currentTime ());
        if (t >= t_end_) break;
        if (kb_ != NULL) {
            if (!kb_(this, kb_data_)) break;
        }
        return;
    }
    killTimer (te->timerId ());
    event_loop.exit();
}

void Waiter::run ()
{
    event_loop.exec ();
    deleteLater ();
}

static bool kbCheckBool (Waiter *, void * data)
{
    bool * b_data = (bool*)data;
    return (*b_data == false);
}

bool Waiter::waitForBoolean (int timeout_msec, bool *b_var)
{
    if ((timeout_msec <= 0) && (timeout_msec != -1)) return false;
    if (b_var == NULL) return false;

    Waiter * w = new Waiter (
                kbCheckBool, b_var,
                timeout_msec);
    w->run ();

    return *b_var;
}
