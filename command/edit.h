#ifndef EDIT_H
#define EDIT_H

#include <string.h>

#include "../fileManager/filesystem.h"
#include "../var/globals.h"

void exec_edit()
{
    if (session.id_user < 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }

    if (strlen(values.path) == 0 || strlen(values.cont) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    int no_block = 0;
    int ptr_inodo = 0;
    Journal * journal = newJournal();
    journal->command = _EDIT_;
    strcpy(journal->str_1, values.path);
    strcpy(journal->str_2, values.cont);
    journal->owner = session.id_user;

    int result = fs_getDirectoryByPath(values.path, __UPDATE__, &no_block, &ptr_inodo);
    if (result < 0)
    {
        printf(ANSI_COLOR_RED "[e] No se pudo obtener el archivo %s\n" ANSI_COLOR_RESET, journal->str_1);
        return;
    }

    Inode * current = getInode(result);
    fs_pushContent(values.cont, current, result);

    printf(ANSI_COLOR_GREEN "[e] Se ha modificado el archivo %s\n" ANSI_COLOR_RESET, journal->str_1);
    if (command != _RECOVERY_)
        fs_backup(journal);
}

#endif //EDIT_H