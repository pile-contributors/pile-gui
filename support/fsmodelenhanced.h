#ifndef FSMODELENHANCED_H
#define FSMODELENHANCED_H

#include <QFileSystemModel>

class FsModelEnhanced : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit FsModelEnhanced(QObject *parent = 0);

signals:
    void fsmeLoadStart ();
    void fsmeLoadEnd ();

protected:

    virtual void
    timerEvent (
            QTimerEvent *event);

    QVariant
    data(
            const QModelIndex & index,
            int role = Qt::DisplayRole) const;

private:

    void
    startDataRetreieveSequence ();

private:
    int timer_id_start_;
    int timer_id_continue_;
};

#endif // FSMODELENHANCED_H
