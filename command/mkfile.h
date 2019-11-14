#ifndef MKFILE_H
#define MKFILE_H

#include "../var/globals.h"
#include "../fileManager/filesystem.h"

void exec_mkfile()
{
    if (session.id_group < 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.path) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetro path requerido\n" ANSI_COLOR_RESET);
        return;
    }

    Journal * journal = newJournal();
    journal->command = _MKFILE_;
    journal->owner = session.id_user;
    journal->size = values.size;
    journal->recursive = values.recursive;
    strcpy(journal->str_1, values.path);
    strcpy(journal->str_2, values.cont);

    int result = fs_createDirectoryFromPath(values.path, values.recursive, _FILE_TYPE_, __CREATE__);
    if (result < 0)
    {
        printf(ANSI_COLOR_RED "[e] No se pudo crear el archivo %s\n" ANSI_COLOR_RESET, journal->str_1);
        return;
    }

    Inode * current = getInode(result);
    if (strlen(values.cont) > 0)
        fs_generateContent_cont(values.cont, current, result);
    else if (values.size > 0)
        fs_generateContent_size(values.size, current, result);
    
    printf(ANSI_COLOR_GREEN "[i] Se ha creado el archivo %s\n" ANSI_COLOR_RESET, journal->str_1);
    if (command != _RECOVERY_)
        fs_backup(journal);
}

#endif //MKFILE_H