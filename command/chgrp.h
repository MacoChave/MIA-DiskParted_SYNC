#ifndef CHGRP_H
#define CHGRP_H

#include "../var/globals.h"
#include "../fileManager/filesystem.h"

void exec_chgrp()
{
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }
    if (strcmp(permissions[session.id_user].group, "root") != 0)
    {
        printf(ANSI_COLOR_RED "[e] No tiene permisos\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.usr) == 0 || strlen(values.grp) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    int id_grp = -1;
    int id_usr = -1;

    for (id_grp = 0; id_grp < 20; id_grp++)
    {
        if (permissions[id_grp].type == 'G')
        {
            if (strcmp(permissions[id_grp].group, values.grp) == 0 && permissions[id_grp].id > 0)
                break;
        }
    }
    
    for (id_usr = 0; id_usr < 20; id_usr++)
    {
        if (permissions[id_usr].type == 'U')
        {
            if (strcmp(permissions[id_usr].name, values.usr) == 0 && permissions[id_usr].id > 0)
                break;
        }
    }

    if (id_usr < 20 && id_grp < 20)
    {
        strcpy(permissions[id_usr].group, values.grp);
        fs_updatePermission();

        printf(ANSI_COLOR_GREEN "[i] Se cambió %s al grupo %s\n" ANSI_COLOR_RESET, values.usr, values.grp);
        
        Journal * journal = newJournal();
        journal->command = _CHGRP_;
        journal->owner = session.id_user;
        strcpy(journal->str_1, values.usr);
        strcpy(journal->str_2, values.grp);
        if (command != _RECOVERY_)
            fs_backup(journal);

        return;
    }
    else
    {
        printf(ANSI_COLOR_RED "[e] No se pudo cambiar el usuario %s al grupo %s\n" ANSI_COLOR_RESET, values.usr, values.grp);
        return;
    }
}

#endif //CHGRP_H