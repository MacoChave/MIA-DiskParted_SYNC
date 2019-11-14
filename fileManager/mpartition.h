/**
 * @file mpartition.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los archivos necesarios para el manejo de particiones dentro del archivo de disco
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MPARTITION_H
#define MPARTITION_H

#include "../var/globals.h"
#include "../fileManager/manager.h"

SpaceDisk spaces[50];

/**
 * @brief Obtiene un disco montado por su id
 * 
 * @param id_i 
 * @return int 
 */
int getDiskById (char id_i)
{
    for (int i = 0; i < 10; i++)
    {
        if (disks_mount[i].letter == id_i)
            return i;
    }
    return _ERROR_;
}

/**
 * @brief Obtiene una partición por su id
 * 
 * @param id_a 
 * @param i 
 * @return int 
 */
int getPartById (int id_a, int i)
{
    if (i == _ERROR_) return _ERROR_;

    for (int j = 0; j < 20; j++)
    {
        if (disks_mount[i].parts_mount[j].mount_id == id_a)
            return j;
    }
    return _ERROR_;   
}

/**
 * @brief Obtiene un disco por su ruta
 * 
 * @param path 
 * @return int 
 */
int getDiskByPath (char path[])
{
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(disks_mount[i].path, path) == 0)
            return i;
    }
    return _ERROR_;
}

/**
 * @brief Obtiene una partición por su nombre
 * 
 * @param name 
 * @param i 
 * @return int 
 */
int getPartByName (char name[], int i)
{
    if (i == _ERROR_) return _ERROR_;

    for (int j = 0; j < 20; j++)
    {
        if (strcmp(disks_mount[i].parts_mount[j].mount_name, name) == 0)
            return j;
    }
    return _ERROR_;   
}

/**
 * @brief Limpia la caché de propiedades del archivo de disco
 * 
 */
void clearSpaceDisk()
{
    for (int i = 0; i < 50; i++)
    {
        spaces[i].start = 0;
        spaces[i].space = 0;
        spaces[i].type = '0';
        spaces[i].next = -1;
        spaces[i].prev = -1;
    }    
}

/**
 * @brief Obtiene el número de una partición dentro de un archivo de disco
 * 
 * @param parts 
 * @return int 
 */
int getPartNumber(Partition parts[])
{
    for (int i = 0; i < 4; i++)
    {
        if (parts[i].part_size == 0) continue;
        
        if (strcmp(parts[i].part_name, values.name) == 0)
            return i;
    }

    return _ERROR_;
}

/**
 * @brief Obtiene el número de partición extendida dentro de un archivo de disco
 * 
 * @param parts 
 * @return int 
 */
int getNumberExtendedPart (Partition parts[])
{
    for (int i = 0; i < 4; i++)
    {
        if (parts[i].part_size <= 0) continue;
        
        if (parts[i].part_type == 'e')
            return i;
    }

    return _ERROR_;
}

/**
 * @brief Obtiene la posición del mejor ajuste para insertar la partición primaria o extendida
 * 
 */
void getBestAdjustPart()
{
    for (int i = 1; i < 50; i++)
    {
        if (spaces[i].start == 0) break;
        for (int j = 0; j < 49; j++)
        {
            if (spaces[j].start == 0) break;
            if (spaces[j].space > spaces[j + 1].space)
            {
                SpaceDisk aux = spaces[j];
                spaces[j] = spaces[j + 1];
                spaces[j + 1] = aux;
            }
        }
    }
}

/**
 * @brief Obtiene la posición del pero ajuste para insertar la partición primaria o extendida
 * 
 */
void getWorstAdjustPart()
{
    for (int i = 1; i < 50; i++)
    {
        if (spaces[i].start == 0) break;
        for (int j = 0; j < 49; j++)
        {
            if (spaces[j].start == 0) break;
            if (spaces[j].space < spaces[j + 1].space)
            {
                SpaceDisk aux = spaces[j];
                spaces[j] = spaces[j + 1];
                spaces[j + 1] = aux;
            }
        }
    }
}

/**
 * @brief Obtiene la posición del primer ajuste para insertar la partición primaria o extendida
 * 
 * @return int 
 */
int getFirstAdjustPart()
{
    for (int i = 0; i < 50; i++)
    {
        if (spaces[i].type == 'f')
        {
            int free = spaces[i].space - values.size;
            if (free > 0) return i;
        }
    }    
}

/**
 * @brief Obtiene el espacio para el detalle de la partición primaria o extendida
 * 
 * @param parts 
 * @param size 
 */
void getSpacePrimaryDetail (Partition parts[], int size)
{
    int pivot = sizeof(MBR);
    int free = 0;
    int s = 0;
    int i = 0;
    while (pivot < size)
    {
        SpaceDisk space;
        if (parts[i].part_size == 0)
        {
            space.start = pivot;
            space.space = size - pivot;
            space.type = 'f';
            spaces[s] = space;
            return;
        }
        else if (pivot != parts[i].part_start)
        {
            space.start = pivot;
            space.space = parts[i].part_start - pivot;
            space.type = 'f';
            spaces[s] = space;
            pivot = parts[i].part_start;
            s++;
        }
        else
        {
            space.start = parts[i].part_start;
            space.space = parts[i].part_size;
            space.space = parts[i].part_type;
            spaces[s] = space;
            pivot += parts[i].part_size;
            s++;
            i++;
        }
    }
}

/**
 * @brief Ordena las particiones dentro del MBR
 * 
 * @param mbr 
 * @return MBR 
 */
MBR sortMBR (MBR mbr)
{
    for (int i = 1; i < 4; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (((mbr.partitions[j].part_start > mbr.partitions[j + 1].part_start) && mbr.partitions[j + 1].part_start != 0) || mbr.partitions[j].part_start == 0)
            {
                Partition p = mbr.partitions[j];
                mbr.partitions[j] = mbr.partitions[j + 1];
                mbr.partitions[j + 1] = p;
            }
        }
    }
    return mbr;
}

/**
 * @brief Obtiene el espacio para el detalle de las particiones lógicas
 * 
 * @param path 
 * @param ebr 
 * @param end 
 */
void getSpaceLogicalDetail (char path[], EBR ebr, int end)
{
    int pivot = ebr.part_start;
    int prev = -1;
    int s = 0;
    while (pivot < end)
    {
        SpaceDisk space;
        if (ebr.part_size == 0)
        {
            space.start = pivot;
            space.space = end - pivot;
            space.type = 'f';
            space.prev = prev;
            space.next = ebr.part_next;
            spaces[s] = space;
            return;
        }
        else if (pivot != ebr.part_start)
        {
            space.start = pivot;
            space.space = ebr.part_start - pivot;
            space.type = 'f';
            space.prev = prev;
            space.next = ebr.part_start;
            spaces[s] = space;
            pivot = ebr.part_start;
            s++;
        }
        else
        {
            space.start = ebr.part_start;
            space.space = ebr.part_size;
            space.type = 'l';
            space.prev = prev;
            space.next = ebr.part_next;
            spaces[s] = space;
            pivot += ebr.part_size;
            prev = ebr.part_start;
            s++;
            if (ebr.part_next > 0)
                ebr = getEBR(path, ebr.part_next);
            else
                return;
        }
    }
}

#endif