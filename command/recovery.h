#ifndef RECOVERY_H
#define RECOVERY_H

#include <string.h>

#include "mkfs.h"
#include "login.h"
#include "logout.h"
#include "mkgrp.h"
#include "mkusr.h"
#include "rmgrp.h"
#include "rmusr.h"
#include "chmod.h"
#include "edit.h"
#include "ren.h"
#include "mv.h"
#include "rem.h"

void recovery_struct()
{
    values.fs = '3';
    session.id_group = -1;
    session.id_user = -1;
    settingSuperBlock();
    updateSuperBlock();
    settingBitmaps();
    initFileSystem();
}

void extractJournal()
{
    int start = session.part_start + __SUPERBLOCK__;
    int count = session.sb->inodes_count;

    Journal * current = NULL;

    for(int i = 0; i < count; i++)
    {
        current = getJournal(start);
        if (current->command == _EMPTY_) break;

        switch (current->command)
        {
            case _MKGRP_:
            {
                session.id_group = 0;
                session.id_user = 1;
                strcpy(values.name, current->str_1);
                initPermissionlist();
                fillUsersTable();
                exec_mkgroup();
                break;
            }
            case _MKUSR_:
            {
                session.id_group = 0;
                session.id_user = 1;
                strcpy(values.usr, current->str_1);
                strcpy(values.pwd, current->str_2);
                strcpy(values.grp, current->str_3);
                initPermissionlist();
                fillUsersTable();
                exec_mkuser();
                break;
            }
            case _RMGRP_:
            {
                session.id_group = 0;
                session.id_user = 1;
                strcpy(values.name, current->str_1);
                initPermissionlist();
                fillUsersTable();
                exec_rmgroup();
                break;
            }
            case _RMUSR_:
            {
                session.id_group = 0;
                session.id_user = 1;
                strcpy(values.usr, current->str_1);
                initPermissionlist();
                fillUsersTable();
                exec_rmuser();
                break;
            }
            case _MKDIR_:
            {
                session.id_user = current->owner;
                getIdGroup();
                strcpy(values.path, current->str_1);
                values.recursive = current->recursive;
                initPermissionlist();
                fillUsersTable();
                exec_mkdir();
                break;
            }
            case _MKFILE_:
            {
                session.id_user = current->owner;
                getIdGroup();
                values.size = current->size;
                strcpy(values.path, current->str_1);
                strcpy(values.cont, current->str_2);
                values.recursive = current->recursive;
                initPermissionlist();
                fillUsersTable();
                exec_mkfile();
                break;
            }
            case _CHGRP_:
            {
                session.id_user = current->owner;
                getIdGroup();
                strcpy(values.usr, current->str_1);
                strcpy(values.grp, current->str_2);
                initPermissionlist();
                fillUsersTable();
                exec_chgrp();
                break;
            }
            case _CHMOD_:
            {
                session.id_user = current->owner;
                getIdGroup();
                strcpy(values.path, current->str_1);
                strcpy(values.ugo, current->ugo);
                initPermissionlist();
                fillUsersTable();
                exec_chmod();
                break;
            }
            case _EDIT_:
            {
                session.id_user = current->owner;
                getIdGroup();
                strcpy(values.path, current->str_1);
                strcpy(values.cont, current->str_2);
                initPermissionlist();
                fillUsersTable();
                exec_edit();
                break;
            }
            case _REN_:
            {
                session.id_user = current->owner;
                getIdGroup();
                strcpy(values.path, current->str_1);
                strcpy(values.name, current->str_2);
                initPermissionlist();
                fillUsersTable();
                exec_ren();
                break;
            }
            case _MV_:
            {
                session.id_user = current->owner;
                getIdGroup();
                strcpy(values.path, current->str_1);
                strcpy(values.dest, current->str_2);
                initPermissionlist();
                fillUsersTable();
                exec_mv();
                break;
            }
            case _REM_:
            {
                session.id_user = current->owner;
                getIdGroup();
                strcpy(values.path, current->str_1);
                initPermissionlist();
                fillUsersTable();
                exec_rem();
                break;
            }
            default:
                break;
        }

        start += __JOURNAL__;
    }
}

void exec_recovery()
{
    if (strlen(values.id) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Se requiere id de partición\n" ANSI_COLOR_RESET);
        return;
    }
    
    char id_x = values.id[2];
    char id_y = values.id[3] - '0';

    int x = getDiskById(id_x);
    int y = getPartById(id_y, x);

    if (y == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no está montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    session.part_start = disks_mount[x].parts_mount[y].mount_start;
    session.part_size = disks_mount[x].parts_mount[y].mount_size;
    strcpy(session.path, disks_mount[x].path);

    SuperBlock * sb = getSuperBlock();

    if (sb->magic <= 0)
    {
        printf("[e] La partición no contiene un sistema de archivos\n");
        return;
    }

    recovery_struct();
    extractJournal();
    exec_logout();
}

#endif //RECOVERY_H