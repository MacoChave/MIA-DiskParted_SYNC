/**
 * @file mkdisk.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los métodos necesarios para la creación de los discos
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MKDISK_H
#define MKDISK_H

#include "../var/globals.h"
#include "../var/filename.h"
#include "../fileManager/manager.h"

/**
 * @brief Crea un disco con los valores recibidos
 * 
 */
void exec_mkdisk()
{
    MBR mbr;

    if (strlen(values.path) <= 0 || values.size < 1)
    {
        printf(ANSI_COLOR_RED "[e] Path y/o size requerido\n" ANSI_COLOR_RESET);
        return;
    }

    if (values.unit == 'k')
        values.size = values.size * 1024;
    else if (values.unit == 'm')
        values.size = values.size * 1024 * 1024;
    else
        values.size = values.size * 1024;
    
    char * raid = (char *)calloc(300, sizeof(char));
    strcpy(raid, getDirectoryFilename(values.path));
    strcat(raid, "/");
    strcat(raid, getFileFilename(values.path));
    strcat(raid, "_ra1.");
    strcat(raid, getTypeFilename(values.path));
    
    if (!createDirectory(values.path))
    {
        printf(ANSI_COLOR_RED "[e] No se puede crear el directorio %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }

    if (existDisk(raid))
    {
        printf(ANSI_COLOR_RED "[e] Ya existe un raid de este disco\n" ANSI_COLOR_RESET);
        return;
    }
    else if (existDisk(values.path))
    {
        printf(ANSI_COLOR_RED "[e] El disco ya existe\n" ANSI_COLOR_RESET);
        return;
    }

    if (!createDisk(values.path))
    {
        printf(ANSI_COLOR_RED "[e] No se pudo crear el disco\n" ANSI_COLOR_RESET);
        return;
    }
    else if (!createDisk(raid))
    {
        printf(ANSI_COLOR_RED "[e] No se pudo crear el disco raid\n" ANSI_COLOR_RESET);
        return;
    }

    mbr.mbr_disk_signature = 2010;
    mbr.size = values.size;
    // TODO: TOMAR EL FIT RECONOCIDO O EL POR DEFECTO
    mbr.fit = (values.fit == '0') ? 'p' : values.fit;
    for (int i = 0; i < 4; i++)
    {
        mbr.partitions[i].part_status ='0';
        mbr.partitions[i].part_type = 'p';
        mbr.partitions[i].part_fit = 'f';
        mbr.partitions[i].part_start = 0;
        mbr.partitions[i].part_size = 0;
        memset(mbr.partitions[i].part_name, 0, 16);
    }

    time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(mbr.mbr_creation, sizeof(mbr.mbr_creation) - 1, "%d/%m/%y %H:%M", date);

    updateMBR(values.path, mbr);
    updateMBR(raid, mbr);
    printf(ANSI_COLOR_GREEN "[i] Disco de %d bytes creado en %s\n" ANSI_COLOR_RESET, values.size, values.path);
}

#endif
