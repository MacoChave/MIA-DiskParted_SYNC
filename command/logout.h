#ifndef LOGOUT_H
#define LOGOUT_H

#include "../var/globals.h"

void exec_logout()
{
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesion activa\n" ANSI_COLOR_RESET);
        return;
    }

    session.part_start = 0;
    session.part_size = 0;
    session.id_user = 0;
    session.id_group = 0;
    memset(session.path, 0, 300);
    session.sb = NULL;

    printf(ANSI_COLOR_GREEN "[i] Sesión finalizada\n" ANSI_COLOR_RESET);
}

#endif //LOGOUT_H