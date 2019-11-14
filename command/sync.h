#ifndef SYNC_H
#define SYNC_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "../var/globals.h"
#include "../var/filename.h"
#include "../fileManager/mpartition.h"
#include "../fileManager/filesystem.h"
#include "login.h"
#include "logout.h"

#include "../peticion.h"

QJsonArray extractTree(Inode * current, int no_upper, int no_current)
{
    QJsonArray array;

    for(int i = 0; i < 12; i++)
    {
        if (current->block[i] == -1) continue;

        if (current->type == _DIRECTORY_TYPE_)
        {
            DirectoryBlock * db = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            for (int j = 0; j < 4; j++)
            {
                if (db->content[j].inode < 0) continue;
                if (strcmp(db->content[j].name, ".") == 0) continue;
                if (strcmp(db->content[j].name, "..") == 0) continue;
                Inode * child = getInode(db->content[j].inode);
                QJsonObject object;

                object["no_inodo"] = db->content[j].inode;
                object["nombre"] = db->content[j].name;
                object["permiso"] = child->permission;
                object["padre"] = (no_upper == 0) ? QJsonValue::Null : no_upper;
                object["tipo"] = child->type;

                QJsonArray content = extractTree(child, no_current, db->content[j].inode);
                object["contenido"] = content;
                array.append(object);
            }
        }
        else {
            FileBlock * fb = (FileBlock *) getGenericBlock(current->block[i], _FILE_TYPE_);
            array.append(fb->content);
        }
    }

    return array;
}

void syncTree(char disk[], char part[])
{
    QFile file("tree_json.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        Inode * current = getInode(0);
        QJsonObject root;
        root["disco"] = disk;
        root["particion"] = part;

        QJsonArray content = extractTree(current, 0, 0);
        root["contenido"] = content;

        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));

        file.close();

        Peticion * peticion = new Peticion();
        peticion->sendPost(peticion->SYNC, QJsonDocument(root).toJson(QJsonDocument::Indented));
    }
}

void exec_sync()
{
    if (strlen(values.id) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parametro id requerido\n" ANSI_COLOR_RESET);
        return;
    }

    strcpy(values.usr, "root");
    strcpy(values.pwd, "123");
    exec_login();

    char id_i = values.id[2];
    int id_a = values.id[3] - '0';

    int i = getDiskById(id_i);
    int j = getPartById(id_a, i);

    if (i == _ERROR_ || j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    MBR mbr = getMBR(disks_mount[i].path);
    Partition  part;
    int a;
    for (a = 0; a < 4; a++)
    {
        if (mbr.partitions[a].part_type == 'p')
        {
            if (strcmp(mbr.partitions[a].part_name, disks_mount[i].parts_mount[j].mount_name) == 0)
            {
                part = mbr.partitions[a];
                break;
            }
        }
    }

    if (a == 4) return;

    syncTree(disks_mount[i].path, disks_mount[i].parts_mount[j].mount_name);

    exec_logout();
}

#endif // SYNC_H
