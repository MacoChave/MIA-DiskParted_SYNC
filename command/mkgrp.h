#ifndef MKGROUP_H
#define MKGROUP_H

#include <string.h>
#include "../var/globals.h"
#include "../var/filename.h"

void exec_mkgroup()
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

    int state = 0;
    for (int i = 0; i < 20; i++)
    {
        state++;
        if (strcmp(permissions[i].group, values.name) == 0 && permissions[i].type == 'G')
        {
            if (permissions[i].id =! 0)
            {
                printf(ANSI_COLOR_RED "[e] Ya existe el grupo %s\n" ANSI_COLOR_RESET, values.name);
                return;
            }
            permissions[i].type = '0';
        }
        if (permissions[i].type == '0')
        {
            permissions[i].id = state;
            permissions[i].type = 'G';
            strcpy(permissions[i].group, values.name);
            break;
        }
    }

    fs_updatePermission();

    if (command != _RECOVERY_)
    {
        Journal * journal = newJournal();
        journal->command = _MKGRP_;
        strcpy(journal->str_1, values.name);
        fs_backup(journal);
    }

    printf(ANSI_COLOR_GREEN "[i] Se creó el grupo %s\n" ANSI_COLOR_RESET, values.name);
}

#endif //MKGROUP_H