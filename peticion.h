#ifndef PETICION_H
#define PETICION_H

#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonValue>

class Peticion
{
private:
    QString API_URI;
public:
    QString USUARIO;
    QString DISCO;
    QString PARTICION;
    QString JOURNAL;
    QString CARPETA;
    QString ARCHIVO;
    QString DETALLEDISCO;
    QString SYNC;

    Peticion();

    QString sendPost(QString path, QByteArray json);
    QString sendPut(QString path, QString json);
    QString sendGet(QString path, QString json);
};

#endif // PETICION_H
