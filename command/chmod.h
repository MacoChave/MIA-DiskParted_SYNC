#ifndef CHMOD_H
#define CHMOD_H

#include <string.h>

#include "../fileManager/filesystem.h"
#include "../var/globals.h"

int validateUGO(char ugo[])
{
    if (ugo[0] - '0' < 0 || ugo[0] -'0' > 7) return 0;
    if (ugo[1] - '0' < 0 || ugo[1] -'0' > 7) return 0;
    if (ugo[2] - '0' < 0 || ugo[2] -'0' > 7) return 0;

    return 1;
}

void exec_chmod()
{
    if (session.id_user < 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesion activa\n" ANSI_COLOR_RESET);
        return;
    }

    if (strlen(values.path) == 0 || !validateUGO(values.ugo))
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    Journal * journal = newJournal();
    journal->command = _CHMOD_;
    journal->owner = session.id_user;
    strcpy(journal->str_1, values.path);
    strcpy(journal->ugo, values.ugo);
    journal->recursive = values.recursive;

    int no_block = 0;
    int ptr_inode = 0;
    int no_inode = fs_getDirectoryByPath(values.path, __UPDATE__, &no_block, &ptr_inode);
    Inode * current = getInode(no_inode);
    if(!fs_checkPermission(current->uid, current->gid, current->permission, __UPDATE__))
    {
        printf(ANSI_COLOR_RED "[e] No posee permisos sobre %s\n" ANSI_COLOR_RESET, journal->str_1);
        return;
    }

    current->permission = atoi(values.ugo);
    updateInode(no_inode, current);
    if (current->type == _DIRECTORY_TYPE_ && values.recursive)
        fs_traversalTree(current, command, atoi(values.ugo));
    
    if (command != _RECOVERY_)
        fs_backup(journal);

    printf(ANSI_COLOR_GREEN "[i] Se cambio permisos a %s\n" ANSI_COLOR_RESET, journal->str_1);
}

#endif //CHMOD_H