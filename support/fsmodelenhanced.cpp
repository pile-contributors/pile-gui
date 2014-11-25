#include "fsmodelenhanced.h"
#include <QTimerEvent>

FsModelEnhanced::FsModelEnhanced(QObject *parent) :
    QFileSystemModel(parent),
    timer_id_continue_(-1)
{
}

void FsModelEnhanced::startDataRetreieveSequence ()
{

    if (timer_id_continue_ != -1) {
        killTimer (timer_id_continue_);
        timer_id_continue_ = startTimer (100);
    } else if (timer_id_start_ == -1) {
        timer_id_start_ = startTimer (20);
    } else {
        killTimer (timer_id_start_);
        timer_id_start_ = -1;

        if (timer_id_continue_ == -1) {
            // this is the first item in the chain
            emit fsmeLoadStart();
        } else {
            killTimer (timer_id_continue_);
        }

        // another wait
        timer_id_continue_ = startTimer (100);
    }
}

QVariant FsModelEnhanced::data (
        const QModelIndex & index, int role) const
{
    for (;;) {

        if (role != Qt::DisplayRole) break;
        if (!index.isValid ()) break;
        if (!index.column () != 0) break;

        // this is nasty
        ((FsModelEnhanced*)this)->startDataRetreieveSequence();
        break;
    }
    return QFileSystemModel::data(index,role);
}

void FsModelEnhanced::timerEvent (QTimerEvent *event)
{
    for (;;) {
        if (timer_id_start_ != -1) {
            if (event->timerId () == timer_id_start_) {
                killTimer (timer_id_start_);
                timer_id_start_ = -1;

                break;
            }
        }
        if (timer_id_continue_ == -1) break;
        if (event->timerId () != timer_id_continue_) break;

        killTimer (timer_id_continue_);
        timer_id_continue_ = -1;
        emit fsmeLoadEnd();
        return;
    }
    QFileSystemModel::timerEvent (event);
}

#if 0
#include <QDebug>
const char * role_name = "";
switch (role) {
case Qt::DisplayRole: role_name = "DisplayRole"; break;
case Qt::DecorationRole: role_name = "DecorationRole"; break;
case Qt::EditRole: role_name = "EditRole"; break;
case Qt::ToolTipRole: role_name = "ToolTipRole"; break;
case Qt::StatusTipRole: role_name = "StatusTipRole"; break;
case Qt::WhatsThisRole: role_name = "WhatsThisRole"; break;
case Qt::FontRole: role_name = "FontRole"; break;
case Qt::TextAlignmentRole: role_name = "TextAlignmentRole"; break;
case Qt::BackgroundColorRole: role_name = "BackgroundColorRole"; break;
case Qt::ForegroundRole: role_name = "ForegroundRole"; break;
case Qt::CheckStateRole: role_name = "CheckStateRole"; break;
case Qt::AccessibleTextRole: role_name = "AccessibleTextRole"; break;
case Qt::AccessibleDescriptionRole: role_name = "AccessibleDescriptionRole"; break;
case Qt::SizeHintRole: role_name = "SizeHintRole"; break;
case Qt::InitialSortOrderRole: role_name = "InitialSortOrderRole"; break;
case Qt::DisplayPropertyRole: role_name = "DisplayPropertyRole"; break;
case Qt::DecorationPropertyRole: role_name = "DecorationPropertyRole"; break;
case Qt::ToolTipPropertyRole: role_name = "ToolTipPropertyRole"; break;
case Qt::StatusTipPropertyRole: role_name = "StatusTipPropertyRole"; break;
case Qt::WhatsThisPropertyRole: role_name = "WhatsThisPropertyRole"; break;
case Qt::UserRole: role_name = "UserRole"; break;
}
qDebug() << "FsModelEnhanced::data "
         << "row " << index.row ()
         << "column " << index.column ()
         << "role" << role_name << " " << role;
#endif
