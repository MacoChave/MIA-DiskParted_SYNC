#include "peticion.h"

Peticion::Peticion()
{
    API_URI = "http://localhost:3000/api";

    USUARIO      = API_URI + "/usuario";
    DISCO        = API_URI + "/disco";
    PARTICION    = API_URI + "/particion";
    JOURNAL      = API_URI + "/journal";
    CARPETA      = API_URI + "/carpeta";
    ARCHIVO      = API_URI + "/archivo";
    DETALLEDISCO = API_URI + "/detalledisco";
    SYNC         = API_URI + "/sync";
}

QString Peticion::sendPost(QString path, QByteArray json)
{
    QEventLoop loop;

    QNetworkAccessManager accessManager;
    QObject::connect(&accessManager, SIGNAL(finished(QNetworkReply*)),
                     &loop,
                     SLOT(quit())
                     );

    QNetworkRequest request((QUrl(path)));
    request.setRawHeader("Content-Type", "application/json");

    QNetworkReply *reply = accessManager.post(request, json);
    loop.exec();

    QString result = "{}";
    if (reply->error() == QNetworkReply::NoError)
        result = QString(reply->readAll());

    delete reply;
    return result;
}

QString Peticion::sendPut(QString path, QString json)
{
    QEventLoop loop;

    QNetworkAccessManager accessManager;
    QObject::connect(&accessManager, SIGNAL(finished(QNetworkReply*)),
                     &loop,
                     SLOT(quit())
                     );

    QNetworkRequest request((QUrl(path)));
    request.setRawHeader("Content-Type", "application/json");

    QNetworkReply *reply = accessManager.put(request,json.toUtf8());
    loop.exec();

    QString result = "{}";
    if (reply->error() == QNetworkReply::NoError)
        result = QString(reply->readAll());

    delete reply;
    return result;
}

//QString Peticion::sendGet(QString path, QString json)
//{

//}
