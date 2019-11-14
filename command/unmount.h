/**
 * @file unmount.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los métodos necesarios para el desmontaje de particiones
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef UNMOUNT_H
#define UNMOUNT_H

#include "../var/globals.h"
#include "../fileManager/manager.h"
#include "../fileManager/mpartition.h"

/**
 * @brief Desmonta la partición indicada, validando que esté montada
 * 
 */
void exec_unmount()
{
    if (strlen(values.id) < 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    char id_i = values.id[2];
    int id_a = values.id[3] - '0';

    int i = getDiskById(id_i);
    int j = getPartById(id_a, i);

    if (j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    clearPartMounted(i, j);
    clearDiskMounted(i);

    printf(ANSI_COLOR_GREEN "[i] Partición %s desmontada\n" ANSI_COLOR_RESET, values.id);
}

#endif