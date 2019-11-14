#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "struct.h"

/* TIPO DE COMANDO */
#define _EXIT_ -1
#define _EXEC_ 0
#define _MKDISK_ 1
#define _RMDISK_ 2
#define _FDISK_ 3
#define _MOUNT_ 4
#define _UNMOUNT_ 5
#define _REP_ 6
#define _PAUSE_ 7
#define _MKFS_ 8
#define _LOGIN_ 9
#define _LOGOUT_ 10
#define _MKGRP_ 11
#define _RMGRP_ 12
#define _MKUSR_ 13
#define _RMUSR_ 14
#define _CHMOD_ 15
#define _MKFILE_ 16
#define _CAT_ 17
#define _REM_ 18
#define _EDIT_ 19
#define _REN_ 20
#define _MKDIR_ 21
#define _CP_ 22
#define _MV_ 23
#define _FIND_ 24
#define _CHOWN_ 25
#define _CHGRP_ 26
#define _RECOVERY_ 27
#define _LOSS_ 28
#define _SYNC_ 29

/* TIPO DE PARAMETRO */
#define _SIZE_ 0
#define _PATH_ 1
#define _UNIT_ 2
#define _NAME_ 3
#define _TYPE_ 4
#define _FIT_ 5
#define _DELETE_ 6
#define _ADD_ 7
#define _ID_ 8
#define _FS_ 9
#define _USR_ 10
#define _PWD_ 11
#define _GRP_ 12
#define _UGO_ 13
#define _R_ 14
#define _CONT_ 15
#define _FILE_ 16
#define _DEST_ 17
#define _RUTA_ 18

/* AUXILIARES PARA INTERPRETE */
#define _EMPTY_ -3
#define _ERROR_ -2
#define _COMMAND_ 0
#define _PARAM_ 1
#define _VALUE_ 2

/* AUXILIARES PARA SISTEMA DE ARCHIVOS */
#define _DIRECTORY_TYPE_ 0
#define _FILE_TYPE_ 1
#define _POINTER_TYPE_ 2
#define _INODE_ 3
#define _BLOCK_ 4

/* OPERACIONES SOBRE ARCHIVOS */
#define __CREATE__ 'C'
#define __READ__ 'R'
#define __UPDATE__ 'U'
#define __DELETE__ 'D'

/* TAMAÑOS DE ESTRUCTURAS */
#define __SUPERBLOCK__ sizeof(SuperBlock)
#define __JOURNAL__ sizeof(Journal)
#define __INODE__ sizeof(Inode)
#define __GENBLOCK__ sizeof(DirectoryBlock)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

Mount disks_mount[10];
Values values;
Permission permissions[20];
Session session;
int command;

void initDisksMount()
{
    for (int i = 0; i < 10; i++)
    {
        disks_mount[i].letter = '0';
        memset(disks_mount[i].path, 0, 300);
        for (int j = 0; j < 20; j++)
        {
            disks_mount[i].parts_mount[j].mount_type = '0';
            disks_mount[i].parts_mount[j].mount_start = 0;
            disks_mount[i].parts_mount[j].mount_size = 0;
            disks_mount[i].parts_mount[j].mount_id = 0;
            
            memset(disks_mount[i].parts_mount[j].mount_name, 0, 16);
        }
    }
}

void initPermissionlist()
{
    for (int i = 0; i < 20; i++)
    {
        permissions[i].id = 0;
        permissions[i].type = '0';
        memset(permissions[i].group, 0, 10);
        memset(permissions[i].name, 0, 10);
        memset(permissions[i].pass, 0, 10);
    }    
}

void initSession()
{
    session.id_user = -1;
    session.id_group = -1;
    session.part_start = -1;
    session.part_size = -1;
    memset(session.path, 0, 300);
    session.sb = NULL;
}

void clearValues()
{
    memset(values.path, 0, 300);
    memset(values.del, 0, 300);
    memset(values.name, 0, 300);
    memset(values.id, 0, 300);
    memset(values.cont, 0, 300);
    memset(values.file, 0, 300);
    memset(values.dest, 0, 300);
    memset(values.ruta, 0, 300);
    memset(values.usr, 0, 10);
    memset(values.pwd, 0, 10);
    memset(values.grp, 0, 10);
    memset(values.ugo, 0, 3);
    values.fit = '0';
    values.unit = '0';
    values.type = '0';
    values.fs = '2';
    values.size = 0;
    values.add = 0;
    values.recursive = 0;
    // command = -1;
}

void clearDiskMounted (int i)
{
    for (int j = 0; j < 20; j++)
    {
        if (disks_mount[i].parts_mount[j].mount_id != 0)
            return;
    }
    disks_mount[i].letter = '0';
    memset(disks_mount[i].path, 0, 300);
}

void clearPartMounted (int i, int j)
{
    disks_mount[i].parts_mount[j].mount_type = '0';
    disks_mount[i].parts_mount[j].mount_start = 0;
    disks_mount[i].parts_mount[j].mount_size = 0;
    disks_mount[i].parts_mount[j].mount_id = 0;
    memset(disks_mount[i].parts_mount[j].mount_name, 0, 16);   
}

void clearAllPartMounted (int i)
{
    for (int j = 0; j < 30; j++)
        clearPartMounted(i, j);    
}

SuperBlock * newSuperBlock()
{
    SuperBlock * sb = (SuperBlock *) calloc(1, sizeof(SuperBlock));
    return sb;
}

Inode * newInode(int type)
{
    Inode * in = (Inode *) calloc(1, sizeof(Inode));
    in->uid = (session.id_user >= 0) ? session.id_user : -1;
    in->gid = (session.id_group >= 0) ? session.id_group : -1;
    in->permission = 664;
    in->type = type;

    __time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(in->create_date, sizeof(in->create_date) - 1, "%d/%m/%y %H:%M", date);
    strftime(in->modified_date, sizeof(in->modified_date) - 1, "%d/%m/%y %H:%M", date);
    strftime(in->last_date, sizeof(in->last_date) - 1, "%d/%m/%y %H:%M", date);

    for (int i = 0; i < 15; i++)
        in->block[i] = -1;

    return in;
}

DirectoryBlock * newDirectoryBlock(int current, int upper)
{
    DirectoryBlock * db = (DirectoryBlock *) calloc(1, sizeof(DirectoryBlock));

    for (int i = 0; i < 4; i++)
        db->content[i].inode = -1;
    
    if (current != _EMPTY_) {
        db->content[0].inode = current;
        strcpy(db->content[0].name, ".");
    }
    if (upper != _EMPTY_) {
        db->content[1].inode = upper;
        strcpy(db->content[1].name, "..");
    }

    return db;
}

FileBlock * newFileBlock()
{
    FileBlock * fb = (FileBlock *) calloc(1, sizeof(FileBlock));
    return fb;
}

PointerBlock * newPointerBlock()
{
    PointerBlock * pb = (PointerBlock *) calloc(1, sizeof(PointerBlock));

    for (int i = 0; i < 16; i++)
        pb->pointers[i] = -1;
    
    return pb;
}

Journal * newJournal()
{
    Journal * journal = (Journal *) calloc(1, sizeof(Journal));
    journal->command = _EMPTY_;
    journal->owner = session.id_user;
    return journal;
}

#endif
