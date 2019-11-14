#ifndef REN_H
#define REN_H

#include <string.h>

#include "../var/globals.h"
#include "../fileManager/filesystem.h"

void exec_ren()
{
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.path) == 0 || strlen(values.name) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    Journal * journal = newJournal();
    journal->command = _REN_;
    strcpy(journal->str_1, values.path);
    strcpy(journal->str_2, values.name);
    journal->owner = session.id_user;

    int no_block = 0;
    int ptr_inodo = 0;
    int no_current = fs_getDirectoryByPath(values.path, __UPDATE__, &no_block, &ptr_inodo);
    if (no_current <= 0 && no_block < 0 && ptr_inodo < 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay acceso al directorio\n" ANSI_COLOR_RESET);
        return;
    }

    Inode * current = getInode(no_current);
    if (fs_checkPermission(current->uid, current->gid, current->permission, __UPDATE__))
    {
        DirectoryBlock * db = (DirectoryBlock *) getGenericBlock(no_block, _DIRECTORY_TYPE_);
        strcpy(db->content[ptr_inodo].name, values.name);
        updateGenericBlock(no_block, db);
        if (command != _RECOVERY_)
            fs_backup(journal);
        printf(ANSI_COLOR_GREEN "[i] Se cambió el nombre de %s a %s\n" ANSI_COLOR_RESET, journal->str_1, journal->str_2);
    }
    else
    {
        printf(ANSI_COLOR_GREEN "[e] No posee permisos sobre el archivo\n" ANSI_COLOR_RESET);
        return;
    }
}

#endif //REN_H