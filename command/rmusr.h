#ifndef RMUSR_H
#define RMUSR_H

#include <string.h>

#include "../fileManager/filesystem.h"
#include "../var/globals.h"

void exec_rmuser()
{
    if (session.id_user < 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strcmp(permissions[session.id_user].group, "root") != 0)
    {
        printf(ANSI_COLOR_RED "[e] Inicie sesión como usuario root\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.usr) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetro usuario requerido\n" ANSI_COLOR_RESET);
        return;
    }

    for (int i = 0; i < 20; i++)
    {
        if (strcmp(permissions[i].name, values.usr) == 0)
        {
            if (permissions[i].id == 0)
            {
                printf(ANSI_COLOR_RED "[e] El usuario %s ya ha sido eliminado\n" ANSI_COLOR_RESET, values.usr);
                return;
            }
            permissions[i].id = 0;
        }
    }

    fs_updatePermission();

    if (command != _RECOVERY_)
    {
        Journal * journal = newJournal();
        journal->command = _RMUSR_;
        strcpy(journal->str_1, values.usr);
        fs_backup(journal);
    }

    printf(ANSI_COLOR_GREEN "[i] Se eliminó el usuario %s\n" ANSI_COLOR_RESET, values.usr);
}

#endif //RMUSR_H