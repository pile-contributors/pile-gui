#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>

class Downloader : public QObject
{
    Q_OBJECT

public:

    typedef void (*Callback) (
            const QString &,
            void *);

public:
    explicit Downloader(QObject *parent = 0);

    bool
    doDownload(
            const QString &s_url,
            const QString & s_destination = QString(),
            Callback kb = NULL,
            void * user_data = NULL);

    bool
    doDownload(
            const QUrl &url,
            const QString & s_destination = QString(),
            Callback kb = NULL,
            void * user_data = NULL);

    bool
    busy()
    {return busy_; }

    void
    cancel();

signals:
    void
    downloadFinished (
            const QString &s_url);

public slots:
   void
   replyFinished (
           QNetworkReply *reply);

private:
   QNetworkAccessManager *manager;
   QNetworkReply *crt_reply;
   bool busy_;
   QString s_destination_file_;
   Callback kb_;
   void * user_data_;
   bool per_request_;
};

#endif // DOWNLOADER_H
