/**
 * @file mount.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los métodos necesarios para el montaje de particiones
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MOUNT_H
#define MOUNT_H

#include "../var/globals.h"
#include "../fileManager/manager.h"
#include "../fileManager/mpartition.h"

/**
 * @brief Monta el disco validando que no esté montado
 * 
 * @return int 
 */
int mountDisk()
{
    Mount mount;
    char letter = 'a';
    for (int j = 0; j < 30; j++)
    {
        mount.parts_mount[j].mount_type = '0';
        mount.parts_mount[j].mount_start = 0;
        mount.parts_mount[j].mount_size = 0;
        mount.parts_mount[j].mount_id = 0;
        
        memset(mount.parts_mount[j].mount_name, 0, 16);
    }
    
    for (int i = 0; i < 26; i++)
    {
        if (disks_mount[i].letter == '0')
        {
            strcpy(mount.path, values.path);
            mount.letter = letter;
            disks_mount[i] = mount;
            return i;
        }
        if (strcmp(disks_mount[i].path, values.path) == 0)
            return i;
        letter++;
    }
}

/**
 * @brief Monta la partición validando que no esté montada
 * 
 * @param partMount 
 * @param i 
 * @return int 
 */
int mountPart(PartMount partMount, int i)
{
    for (int j = 0; j < 30; j++)
    {
        if (disks_mount[i].parts_mount[j].mount_size <= 0)
        {
            partMount.mount_id = j + 1;
            disks_mount[i].parts_mount[j] = partMount;
            return j + 1;
        }
        if (strcmp(disks_mount[i].parts_mount[j].mount_name, values.name) == 0)
            return 0;
    }    
}

/**
 * @brief Monta las particiones desde el mbr del disco recibido
 * 
 * @param mbr 
 */
void mounting(MBR mbr)
{
    PartMount partMount;
    partMount.mount_size = 0;
    partMount.mount_start = 0;
    partMount.mount_id = 0;
    char i_part = '0';
    int i = getPartNumber(mbr.partitions);

    if (i == _ERROR_)
    {
        i = getNumberExtendedPart(mbr.partitions);
        EBR ebr = getEBR(values.path, mbr.partitions[i].part_start);
        clearSpaceDisk();
        getSpaceLogicalDetail(values.path, ebr, mbr.partitions[i].part_start + mbr.partitions[i].part_size);
        for (int i = 0; i < 50; i++)
        {
            if (spaces[i].type == 'l')
            {
                ebr = getEBR(values.path, spaces[i].start);
                if (strcmp(ebr.ebr_name, values.name) == 0)
                {
                    partMount.mount_size = ebr.part_size;
                    partMount.mount_start = ebr.part_start;
                    partMount.mount_type = 'l';
                    break;
                }
            }
        }
        if (partMount.mount_size == 0)
        {
            printf(ANSI_COLOR_RED "[e] No existe la partición %s en %s\n" ANSI_COLOR_RESET, values.name, values.path);
            return;
        }
    }
    else
    {
        partMount.mount_size = mbr.partitions[i].part_size;
        partMount.mount_start = mbr.partitions[i].part_start;
        partMount.mount_type = mbr.partitions[i].part_type;
    }
    strcpy(partMount.mount_name, values.name);
    
    int i_dsk = mountDisk();
    i_part = mountPart(partMount, i_dsk);
    if (i_part > 0)
        printf(ANSI_COLOR_GREEN "[i] Se montó la partición %s como vd%c%d\n" ANSI_COLOR_RESET, values.name, disks_mount[i_dsk].letter, i_part);
    else
        printf(ANSI_COLOR_RED "[e] No se montó la partición %s en vd%c\n" ANSI_COLOR_RESET, values.name, disks_mount[i_dsk].letter);
}

/**
 * @brief Muestra el listado de particiones montadas indicando el disco de procedencia
 * 
 */
void showMountedPartition()
{
    for (int i = 0; i < 10; i++)
    {
        if (disks_mount[i].letter == '0') continue;

        printf(ANSI_COLOR_BLUE "vd%c\n" ANSI_COLOR_RESET, disks_mount[i].letter);
        for (int j = 0; j < 20; j++)
        {
            if (disks_mount[i].parts_mount[j].mount_id == 0) continue;
            
            printf(ANSI_COLOR_BLUE "\tvd%c%d\n" ANSI_COLOR_RESET, disks_mount[i].letter, disks_mount[i].parts_mount[j].mount_id);
        }
    }    
}

/**
 * @brief Monta la partición del disco indicado
 * 
 */
void exec_mount()
{
    if (strlen(values.path) < 0 || strlen(values.name) < 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    if (!existDisk(values.path))
    {
        printf(ANSI_COLOR_RED "[e] Disco %s no existe\n" ANSI_COLOR_RESET, values.path);
        return;
    }

    MBR mbr = getMBR(values.path);

    mounting(mbr);
    showMountedPartition();
}

#endif