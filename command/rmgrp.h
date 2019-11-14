#ifndef RMGROUP_H
#define RMGROUP_H

#include <string.h>

#include "../var/globals.h"
#include "../fileManager/filesystem.h"

void exec_rmgroup()
{
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strcmp(permissions[session.id_user].group, "root") != 0)
    {
        printf(ANSI_COLOR_RED "[e] Inicie sesión como usuario root\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.name) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetro name obligatorio\n" ANSI_COLOR_RESET);
        return;
    }

    for (int i = 0; i < 20; i++)
    {
        if (strcmp(permissions[i].group, values.name) == 0 && permissions[i].type == 'G')
        {
            if (permissions[i].id == 0)
            {
                printf(ANSI_COLOR_RED "[e] El grupo %s ya ha sido eliminado\n" ANSI_COLOR_RESET);
                return;
            }
            permissions[i].id = 0;
        }
    }

    fs_updatePermission();

    if (command != _RECOVERY_)
    {
        Journal * journal = newJournal();
        journal->command = _RMGRP_;
        strcpy(journal->str_1, values.name);
        fs_backup(journal);
    }

    printf(ANSI_COLOR_GREEN "[i] Se eliminó el grupo %s\n" ANSI_COLOR_RESET, values.name);
}

#endif //MRGROUP_H