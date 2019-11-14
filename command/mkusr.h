#ifndef MKUSR_H
#define MKUSR_H

#include <string.h>

#include "../fileManager/filesystem.h"
#include "../var/globals.h"

void exec_mkuser()
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
    if (strlen(values.usr) == 0 || strlen(values.pwd) == 0 || strlen(values.grp) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetro incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    int existUser = 0;
    int existGroup = 0;
    int id = 0;

    for (int i = 0; i < 20; i++)
    {
        id ++;

        if (strcmp(permissions[i].group, values.grp) == 0 && permissions[i].type == 'G')
        {
            if (permissions[i].id == 0)
            {
                printf(ANSI_COLOR_RED "[e] El grupo está eliminado\n" ANSI_COLOR_RESET);
                return;
            }
            existGroup = !existGroup;
            continue;
        }
        if (strcmp(permissions[i].name, values.usr) == 0)
        {
            if (permissions[i].id != 0)
            {
                printf(ANSI_COLOR_RED "[e] Ya existe el usuario %s\n" ANSI_COLOR_RESET, values.usr);
                return;
            }
            permissions[i].type = '0';
        }
        if (permissions[i].type == '0')
        {
            if (!existGroup)
            {
                printf(ANSI_COLOR_RED "[e] El grupo %s no existe\n" ANSI_COLOR_RESET, values.grp);
            }

            permissions[i].id = id;
            permissions[i].type = 'U';
            strcpy(permissions[i].name, values.usr);
            strcpy(permissions[i].group, values.grp);
            strcpy(permissions[i].pass, values.pwd);
            break;
        }
    }
    
    fs_updatePermission();

    if (command != _RECOVERY_)
    {
        Journal * journal = newJournal();
        journal->command = _MKUSR_;
        strcpy(journal->str_1, values.usr);
        strcpy(journal->str_2, values.pwd);
        strcpy(journal->str_3, values.grp);
        fs_backup(journal);
    }

    printf(ANSI_COLOR_GREEN "[i] Se creó el usuario %s\n" ANSI_COLOR_RESET, values.usr);
}

#endif //MKUSR_H