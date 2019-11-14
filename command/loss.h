#ifndef LOSS_H
#define LOSS_H

#include <string.h>

#include "../var/globals.h"
#include "../fileManager/filesystem.h"
#include "../fileManager/manager.h"
#include "logout.h"

void lossPartition()
{
    int size = session.part_size;
    size -= __SUPERBLOCK__ - session.sb->inodes_count * __JOURNAL__;

    clearPartDisk(session.path, session.sb->bm_inode_start, size);
}

void exec_loss()
{
    exec_logout();
    initPermissionlist();
    
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
        printf(ANSI_COLOR_RED "[e] La partición no contiene un sistema de archivos\n" ANSI_COLOR_RESET);
        return;
    }
    
    session.sb = sb;
    lossPartition();

    printf(ANSI_COLOR_GREEN "[i] Se ha simulado la pérdida con éxito\n" ANSI_COLOR_RESET);
}

#endif //LOSS_H