#include "downloader.h"

#include <QTemporaryFile>

Downloader::Downloader(QObject *parent) :
    QObject(parent),
    manager(new QNetworkAccessManager(this)),
    crt_reply(NULL),
    busy_(false),
    s_destination_file_(),
    kb_(NULL),
    user_data_(NULL),
    per_request_(false)
{
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

void Downloader::cancel()
{
    if (!busy_) return;
    if (crt_reply == NULL) return;
    crt_reply->abort ();
}

bool Downloader::doDownload (
        const QString & s_url, const QString & s_destination,
        Callback kb, void * user_data)
{
    return doDownload (QUrl(s_url), s_destination, kb, user_data);
}

bool Downloader::doDownload (
        const QUrl & url, const QString & s_destination,
        Callback kb, void * user_data)
{
    if (busy_) return false;

    if (kb != NULL) {
        per_request_ = true;
        kb_ = kb;
        user_data_ = user_data;
    }

    s_destination_file_ = s_destination;
    if (s_destination.isEmpty ()) {
        QTemporaryFile tf;
        if (!tf.open ()) return false;
        s_destination_file_ = tf.fileName ();
    }

    busy_ = true;
    crt_reply = manager->get(QNetworkRequest(url));

    return true;
}

void Downloader::replyFinished (QNetworkReply *reply)
{
    if (reply->error()) {
        //        qDebug() << "ERROR!";
        //        qDebug() << reply->errorString();
    } else {
        //        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        //        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        //        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        //        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        //        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        QFile file (s_destination_file_);
        if(file.open (QFile::WriteOnly))  {
            file.write (reply->readAll());
            file.flush();
            file.close();
        }
    }

    reply->deleteLater();
    busy_ = false;
    crt_reply = NULL;

    if (kb_ != NULL) {
        kb_ (s_destination_file_, user_data_);
    }
    emit downloadFinished (s_destination_file_);

    if (per_request_) {
        kb_ = NULL;
        user_data_ = NULL;
    }

}
