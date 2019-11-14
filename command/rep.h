/**
 * @file rep.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los métodos necesarios para generar los reportes solicitados
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef REP_H
#define REP_H

#include "../var/globals.h"
#include "../var/filename.h"
#include "../fileManager/mpartition.h"
#include "../fileManager/filesystem.h"

/**
 * @brief Cuenta las particiones primarias y extendidas para generar los reportes
 * 
 * @param part 
 * @return int 
 */
int countPrimary (Partition part[])
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (part[i].part_size > 0)
            count++;
    }
    return count;
}

/**
 * @brief Cuenta las particiones lógicas del disco para generar los reportes
 * 
 * @return int 
 */
int countLogical ()
{
    int count = 0;
    for (int i = 0; i < 10; i++)
    {
        if (spaces[i].type == 'l')
            count+= 2;
        if (spaces[i].type == 'f')
            count++;
    }
    return count;
}

/**
 * @brief Genera el reporte del estado del disco
 * 
 * @param mbr 
 * @param path_disk 
 */
void reportDisk (MBR mbr, char path_disk[])
{
    char dotfile[20] = "disk_report.dot";

    FILE * file;
    file = fopen(dotfile, "w");
    int no_p = countPrimary(mbr.partitions);
    int no_l = countLogical();
    double dividend = 0.0;
    double divider = 0.0;
    double percent = 0.0;
    int pivot = sizeof(MBR);

    if (file != NULL)
    {
        fprintf(file, "digraph {\n");
        fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
        fprintf(file, "\tnode [shape = plain]\n");
        fprintf(file, "\trankdir = LR\n");
        fprintf(file, "\tDISK [label=<\n");
        fprintf(file, "\t\t<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
        dividend = sizeof(MBR);
        divider = mbr.size;
        percent = dividend / divider;
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"crimson\" rowspan=\"2\">MBR<br/>%.4f %% de Disco</td>\n", percent * 100);
        
        for (int i = 0; i < 4; i++)
        {
            if (pivot != mbr.partitions[i].part_start)
            {
                if (mbr.partitions[i].part_size > 0)
                {
                    dividend = mbr.partitions[i].part_start - pivot;
                    percent = dividend / divider;
                    pivot = mbr.partitions[i].part_start;
                }
                else
                {
                    dividend = mbr.size - pivot;
                    percent = dividend / divider;
                    break;
                }
                fprintf(file, "\t\t\t\t<td bgcolor=\"snow2\" rowspan=\"2\">Free<br/>%.2f %% de Disco</td>\n", percent * 100);
            }
            if (mbr.partitions[i].part_type == 'p')
            {
                pivot = mbr.partitions[i].part_start + mbr.partitions[i].part_size;
                dividend = mbr.partitions[i].part_size;
                percent = dividend / divider;
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\" rowspan=\"2\">%s<br/>%.2f %% de Disco</td>\n", mbr.partitions[i].part_name, percent * 100);
            }
            else if (mbr.partitions[i].part_type == 'e')
            {
                pivot = mbr.partitions[i].part_start + mbr.partitions[i].part_size;
                fprintf(file, "\t\t\t\t<td bgcolor=\"firebrick1\" colspan=\"%d\">Extendida</td>\n", no_l);
            }
        }
        if (pivot < mbr.size)
        {
            dividend = mbr.size - pivot;
            percent = dividend / divider;
            fprintf(file, "\t\t\t\t<td bgcolor=\"snow2\" rowspan=\"2\">Free<br/>%.2f %% de Disco</td>\n", percent * 100);
        }
        fprintf(file, "\t\t\t</tr>\n");

        int idx_ext = getNumberExtendedPart(mbr.partitions);
        if (idx_ext != _ERROR_)
        {
            fprintf(file, "\t\t\t<tr>\n");
            for (int i = 0; i < 10; i++)
            {
                if (spaces[i].type == 'l')
                {
                    EBR ebr = getEBR(path_disk, spaces[i].start);
                    fprintf(file, "\t\t\t\t<td bgcolor=\"crimson\">EBR</td>\n");
                    dividend = spaces[i].space;
                    percent = dividend / divider;
                    fprintf(file, "\t\t\t\t<td bgcolor=\"bisque\">%s<br/>%.2f %% de Disco</td>\n", ebr.ebr_name, percent * 100);
                }
                else if (spaces[i].type == 'f')
                {
                    dividend = spaces[i].space;
                    percent = dividend / divider;
                    fprintf(file, "\t\t\t\t<td bgcolor=\"snow2\">Free<br/>%.2f %% de Disco</td>\n", percent * 100);
                }
            }
            fprintf(file, "\t\t\t</tr>\n");
        }
        fprintf(file, "\t\t</table>\n");
        fprintf(file, "\t>]\n");
        fprintf(file, "}\n");

        fclose(file);
    }

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, "disk_report.dot");
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf("%s\n", cmd);
    system(cmd);
}

/**
 * @brief Genera el reporte MBR del disco indicado
 * 
 * @param mbr 
 * @param path_disk 
 */
void reportMBR (MBR mbr, char path_disk[])
{
    char dotfile[20] = "mbr_report.dot";

    FILE * file;
    file = fopen(dotfile, "w");

    if (file != NULL)
    {
        fprintf(file, "digraph {\n");
        fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
        fprintf(file, "\tnode [shape = plain]\n");
        fprintf(file, "\trankdir = LR\n");
        fprintf(file, "\tMBR [label=<\n");
        fprintf(file, "\t\t<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"crimson\" colspan=\"2\">MBR Report</td>\n");
        fprintf(file, "\t\t\t</tr>\n");

        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"firebrick1\">Nombre</td>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"firebrick1\">Valor</td>\n");
        fprintf(file, "\t\t\t</tr>\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">MBR Tamaño</td>\n");
        fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.size);
        fprintf(file, "\t\t\t</tr>\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">MBR Fecha de Creación</td>\n");
        fprintf(file, "\t\t\t\t<td>%s</td>\n", mbr.mbr_creation);
        fprintf(file, "\t\t\t</tr>\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">MBR Disk Signature</td>\n");
        fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.mbr_disk_signature);
        fprintf(file, "\t\t\t</tr>\n");

        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Disk fit</td>\n");
        fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.fit);
        fprintf(file, "\t\t\t</tr>\n");
        
        for (int i = 0; i < 4; i++)
        {
            if (mbr.partitions[i].part_size > 0)
            {
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"firebrick1\" colspan=\"2\">Partición %d</td>\n", i);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part status</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_status);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part type</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_type);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part fit</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_fit);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part start</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.partitions[i].part_start);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part size</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.partitions[i].part_size);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part name</td>\n");
                fprintf(file, "\t\t\t\t<td>%s</td>\n", mbr.partitions[i].part_name);
                fprintf(file, "\t\t\t</tr>\n");
            }
        }

        fprintf(file, "\t\t</table>\n");
        fprintf(file, "\t>]\n");


        int ebr_index = 1;
        EBR ebr;
        for (int i = 0; i < 50; i++)
        {
            if (spaces[i].type == 'l')
            {
                ebr = getEBR(path_disk, spaces[i].start);
                fprintf(file, "\tEBR%d [label=<\n", ebr_index);
                fprintf(file, "\t\t<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
            
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"crimson\" colspan=\"2\">Reporte EBR %d</td>\n", ebr_index);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"firebrick1\">Nombre</td>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"firebrick1\">Valor</td>\n");
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part status</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", ebr.part_status);
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part fit</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", ebr.part_fit);
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part start</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_start);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part size</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_size);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part next</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_next);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td bgcolor=\"salmon\">Part name</td>\n");
                fprintf(file, "\t\t\t\t<td>%s</td>\n", ebr.ebr_name);
                fprintf(file, "\t\t\t</tr>\n");
            
                fprintf(file, "\t\t</table>\n");
                fprintf(file, "\t>]\n");
                ebr_index++;
            }
        }
        
        fprintf(file, "}");
        fclose(file);
    }

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, "mbr_report.dot");
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf("%s\n", cmd);
    system(cmd);
}

void reportInodes()
{
    char dotfile[20] = "inodes_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    if (file == NULL) return;

    fprintf(file, "digraph {\n");
    fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
    fprintf(file, "\tnode [shape = plain]\n");
    fprintf(file, "\trankdir = TB\n");

    for (int i = 0; i < session.sb->inodes_count; i++)
    {
        char bit = getBitmap(i, _INODE_);
        if (bit == '0') continue;

        Inode * current = getInode(i);
        fprintf(file, "\tINODE_%d [\n", i);
        fprintf(file, "\t\tlabel = <\n");
        if (current->type == _FILE_TYPE_)
            fprintf(file, "\t\t\t<table bgcolor = \"salmon3\">\n");
        else if (current->type == _DIRECTORY_TYPE_)
            fprintf(file, "\t\t\t<table bgcolor = \"skyblue3\">\n");
        
        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td colspan = \"2\">Inode %d</td>\n", i);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>UID</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", permissions[current->uid].id);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>GID</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", permissions[current->gid].id);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Size</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", current->size);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Readed date</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", current->last_date);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Created date</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", current->create_date);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Modified date</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", current->modified_date);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Type</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", current->type);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Permission</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", current->permission);
        fprintf(file, "\t\t\t\t</tr>\n");

        for (int j = 0; j < 16; j++)
        {
            if (current->block[j] < 0) continue;
            
            fprintf(file, "\t\t\t\t<tr>\n");
            fprintf(file, "\t\t\t\t\t<td>Pointer</td>\n");
            fprintf(file, "\t\t\t\t\t<td>%d</td>\n", current->block[j]);
            fprintf(file, "\t\t\t\t</tr>\n");
        }

        fprintf(file, "\t\t\t</table>\n");
        fprintf(file, "\t\t>\n");
        fprintf(file, "\t]\n");
    }

    fprintf(file, "}\n"); 
    fclose(file);

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
    system(cmd);
}

void reportBlocks_Indirect(FILE * file, int no_current, PointerBlock * current, int type, int level)
{
    for (int i = 0; i < 16; i++)
    {
        if (current->pointers[i] < 0) continue;

        if (level > 1)
        {
            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);

            fprintf(file, "\tBLOCK_%d [\n", pb->pointers[i]);
            fprintf(file, "\t\tlabel = <\n");
            fprintf(file, "\t\t\t<table bgcolor = \"slateblue\">\n");
            
            fprintf(file, "\t\t\t\t<tr>\n");
            fprintf(file, "\t\t\t\t\t<td colspan = \"2\">BLock %d</td>\n", pb->pointers[i]);
            fprintf(file, "\t\t\t\t</tr>\n");

            for (int z = 0; z < 16; z++)
            {
                fprintf(file, "\t\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t\t<td>%d</td>\n", pb->pointers[z]);
                fprintf(file, "\t\t\t\t</tr>\n");
            }

            fprintf(file, "\t\t\t</table>\n");
            fprintf(file, "\t\t>\n");
            fprintf(file, "\t]\n");
            
            reportBlocks_Indirect(file, current->pointers[i], pb, type, level - 1);
        }
        else 
        {
            for (int j = 0; j < 15; j++)
            {
                if (type == _FILE_TYPE_)
                {
                    FileBlock * bf = (FileBlock *) getGenericBlock(current->pointers[j], _FILE_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->pointers[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"salmon\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>Block %d</td>\n", current->pointers[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bf->content);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");
                }
                else if (type == _DIRECTORY_TYPE_)
                {
                    DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(current->pointers[j], _DIRECTORY_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->pointers[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"skyblue\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td colspan = \"2\">BLock %d</td>\n", current->pointers[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    for (int z = 0; z < 4; z++)
                    {
                        fprintf(file, "\t\t\t\t<tr>\n");
                        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bd->content[z].name);
                        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", z, bd->content[z].inode);
                        fprintf(file, "\t\t\t\t</tr>\n");
                    }

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");
                }
            }                    
        }
    }
}

void reportBlocks()
{
    int level = 1;
    char dotfile[20] = "block_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    if (file == NULL) return;

    fprintf(file, "digraph {\n");
    fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
    fprintf(file, "\tnode [shape = plain]\n");
    fprintf(file, "\trankdir = TB\n");

    for (int i = 0; i < session.sb->inodes_count; i++)
    {
        char bit = getBitmap(i, _INODE_);
        if (bit == '0') continue;

        Inode * current = getInode(i);
        for (int j = 0; j < 15; j++)
        {
            if (current->block[j] < 0) continue;

            if (j < 12)
            {
                if (current->type == _FILE_TYPE_)
                {
                    FileBlock * bf = (FileBlock *) getGenericBlock(current->block[j], _FILE_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->block[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"salmon\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>Block %d</td>\n", current->block[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bf->content);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");
                }
                else if (current->type == _DIRECTORY_TYPE_)
                {
                    DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(current->block[j], _DIRECTORY_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->block[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"skyblue\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td colspan = \"2\">BLock %d</td>\n", current->block[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    for (int z = 0; z < 4; z++)
                    {
                        fprintf(file, "\t\t\t\t<tr>\n");
                        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bd->content[z].name);
                        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", bd->content[z].inode);
                        fprintf(file, "\t\t\t\t</tr>\n");
                    }

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");
                }
            }
            else 
            {
                PointerBlock * pb = (PointerBlock *) getGenericBlock(current->block[j], _POINTER_TYPE_);

                fprintf(file, "\tBLOCK_%d [\n", current->block[j]);
                fprintf(file, "\t\tlabel = <\n");
                fprintf(file, "\t\t\t<table bgcolor = \"skyblue\">\n");
                
                fprintf(file, "\t\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t\t<td colspan = \"2\">BLock %d</td>\n", current->block[j]);
                fprintf(file, "\t\t\t\t</tr>\n");

                for (int z = 0; z < 4; z++)
                {
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", pb->pointers[z]);
                    fprintf(file, "\t\t\t\t</tr>\n");
                }

                fprintf(file, "\t\t\t</table>\n");
                fprintf(file, "\t\t>\n");
                fprintf(file, "\t]\n");

                reportBlocks_Indirect(file, current->block[j], pb, current->type, level);

                level++;
            }
        }        
    }

    fprintf(file, "}\n");    
    fclose(file);

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
    system(cmd);
}

void reportBitmap(char type)
{
    if (strcmp(getTypeFilename(values.path), "txt") != 0) 
    {
        printf(ANSI_COLOR_RED "[e] El reporte debe ser en .txt\n" ANSI_COLOR_RESET);
        return;
    }

    int count = (type == _INODE_) ? 
            session.sb->inodes_count :
            session.sb->blocks_count;
    
    FILE * file;
    file = fopen(values.path, "w");

    if (file == NULL) return;

    for (int i = 0; i < count; i++)
    {
        char bit = getBitmap(i, _INODE_);
        fprintf(file, " %c ", bit);
    }

    fclose(file);
}

void reportTree_Indirect(FILE * file, int no_current, PointerBlock * current, int type, int level)
{
    for (int i = 0; i < 16; i++)
    {
        if (current->pointers[i] < 0) continue;

        if (level > 1)
        {
            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);

            fprintf(file, "\tBLOCK_%d [\n", pb->pointers[i]);
            fprintf(file, "\t\tlabel = <\n");
            fprintf(file, "\t\t\t<table bgcolor = \"slateblue\">\n");
            
            fprintf(file, "\t\t\t\t<tr>\n");
            fprintf(file, "\t\t\t\t\t<td>BLock %d</td>\n", pb->pointers[i]);
            fprintf(file, "\t\t\t\t</tr>\n");

            for (int z = 0; z < 16; z++)
            {
                if (pb->pointers[z] < 0) continue;

                fprintf(file, "\t\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t\t<td port = '%d'>%d</td>\n", z, pb->pointers[z]);
                fprintf(file, "\t\t\t\t</tr>\n");
            }

            fprintf(file, "\t\t\t</table>\n");
            fprintf(file, "\t\t>\n");
            fprintf(file, "\t]\n");

            for (int z = 0; z < 16; z++)
            {
                if (pb->pointers[z] < 0) continue;

                fprintf(file, "\n\tBLOCK_%d : %d -> BLOCK_%d\n\n", no_current, z, pb->pointers[z]);
            }
            
            reportTree_Indirect(file, current->pointers[i], pb, type, level - 1);
        }
        else 
        {
            for (int j = 0; j < 15; j++)
            {
                if (current->pointers[j] < 0) continue;

                fprintf(file, "\n\tBLOCK_%d : %d -> BLOCK_%d\n\n", no_current, j, current->pointers[j]);

                if (type == _FILE_TYPE_)
                {
                    FileBlock * bf = (FileBlock *) getGenericBlock(current->pointers[j], _FILE_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->pointers[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"salmon\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>Block %d</td>\n", current->pointers[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bf->content);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");
                }
                else if (type == _DIRECTORY_TYPE_)
                {
                    DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(current->pointers[j], _DIRECTORY_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->pointers[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"skyblue\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td colspan = \"2\">BLock %d</td>\n", current->pointers[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    for (int z = 0; z < 4; z++)
                    {
                        if (bd->content[z].inode < 0) continue;

                        fprintf(file, "\t\t\t\t<tr>\n");
                        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bd->content[z].name);
                        fprintf(file, "\t\t\t\t\t<td port = '%d'>%d</td>\n", z, bd->content[z].inode);
                        fprintf(file, "\t\t\t\t</tr>\n");
                    }

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");

                    for (int z = 0; z < 4; z++)
                    {
                        if (bd->content[z].inode < 0) continue;
                        if (strcmp(bd->content[z].name, ".") == 0) continue;
                        if (strcmp(bd->content[z].name, "..") == 0) continue;

                        fprintf(file, "\n\tBLOCK_%d : %d -> INODE_%d\n", current->pointers[j], z, bd->content[z].inode);
                    }
                }
            }                    
        }
    }
}

void reportTree()
{
    int level = 1;
    char dotfile[20] = "tree_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    if (file == NULL) return;

    fprintf(file, "digraph {\n");
    fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
    fprintf(file, "\tnode [shape = plain]\n");
    fprintf(file, "\trankdir = LR\n");

    for (int i = 0; i < session.sb->inodes_count; i++)
    {
        char bit = getBitmap(i, _INODE_);
        if (bit == '0') continue;

        Inode * current = getInode(i);
        
        fprintf(file, "\tINODE_%d [\n", i);
        fprintf(file, "\t\tlabel = <\n");
        if (current->type == _FILE_TYPE_)
            fprintf(file, "\t\t\t<table bgcolor = \"salmon3\">\n");
        else if (current->type == _DIRECTORY_TYPE_)
            fprintf(file, "\t\t\t<table bgcolor = \"skyblue3\">\n");
        
        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td colspan = \"2\">Inode %d</td>\n", i);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>UID</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", permissions[current->uid].id);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>GID</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", permissions[current->gid].id);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Size</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", current->size);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Readed date</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", current->last_date);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Created date</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", current->create_date);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Modified date</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", current->modified_date);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Type</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", current->type);
        fprintf(file, "\t\t\t\t</tr>\n");

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>Permission</td>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", current->permission);
        fprintf(file, "\t\t\t\t</tr>\n");

        for (int j = 0; j < 15; j++)
        {
            if (current->block[j] < 0) continue;
            
            fprintf(file, "\t\t\t\t<tr>\n");
            fprintf(file, "\t\t\t\t\t<td>Pointer</td>\n");
            fprintf(file, "\t\t\t\t\t<td port = '%d'>%d</td>\n", j, current->block[j]);
            fprintf(file, "\t\t\t\t</tr>\n");
        }

        fprintf(file, "\t\t\t</table>\n");
        fprintf(file, "\t\t>\n");
        fprintf(file, "\t]\n");

        for (int j = 0; j < 15; j++)
        {
            if (current->block[j] < 0) continue;

            fprintf(file, "\n\tINODE_%d : %d -> BLOCK_%d\n\n", i, j, current->block[j]);

            if (j < 12)
            {
                if (current->type == _FILE_TYPE_)
                {
                    FileBlock * bf = (FileBlock *) getGenericBlock(current->block[j], _FILE_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->block[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"salmon\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>Block %d</td>\n", current->block[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bf->content);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");
                }
                else if (current->type == _DIRECTORY_TYPE_)
                {
                    DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(current->block[j], _DIRECTORY_TYPE_);

                    fprintf(file, "\tBLOCK_%d [\n", current->block[j]);
                    fprintf(file, "\t\tlabel = <\n");
                    fprintf(file, "\t\t\t<table bgcolor = \"skyblue\">\n");
                    
                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td colspan = \"2\">BLock %d</td>\n", current->block[j]);
                    fprintf(file, "\t\t\t\t</tr>\n");

                    for (int z = 0; z < 4; z++)
                    {
                        fprintf(file, "\t\t\t\t<tr>\n");
                        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", bd->content[z].name);
                        fprintf(file, "\t\t\t\t\t<td port = '%d'>%d</td>\n", z, bd->content[z].inode);
                        fprintf(file, "\t\t\t\t</tr>\n");
                    }

                    fprintf(file, "\t\t\t</table>\n");
                    fprintf(file, "\t\t>\n");
                    fprintf(file, "\t]\n");

                    for (int z = 0; z < 4; z++)
                    {
                        if (bd->content[z].inode == -1) continue;
                        if (strcmp(bd->content[z].name, ".") == 0) continue;
                        if (strcmp(bd->content[z].name, "..") == 0) continue;

                        fprintf(file, "\n\tBLOCK_%d : %d -> INODE_%d\n", current->block[j], z, bd->content[z].inode);
                    }
                }
            }
            else
            {
                PointerBlock * pb = (PointerBlock *) getGenericBlock(current->block[j], _POINTER_TYPE_);

                fprintf(file, "\tBLOCK_%d [\n", current->block[j]);
                fprintf(file, "\t\tlabel = <\n");
                fprintf(file, "\t\t\t<table bgcolor = \"skyblue\">\n");
                
                fprintf(file, "\t\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t\t<td>BLock %d</td>\n", current->block[j]);
                fprintf(file, "\t\t\t\t</tr>\n");

                for (int z = 0; z < 4; z++)
                {
                    if (pb->pointers[z] < 0) continue;

                    fprintf(file, "\t\t\t\t<tr>\n");
                    fprintf(file, "\t\t\t\t\t<td port = '%d'>%d</td>\n", z, pb->pointers[z]);
                    fprintf(file, "\t\t\t\t</tr>\n");
                }

                fprintf(file, "\t\t\t</table>\n");
                fprintf(file, "\t\t>\n");
                fprintf(file, "\t]\n");

                reportTree_Indirect(file, current->block[j], pb, current->type, level);

                level++;
            }
        }        
    }

    fprintf(file, "}\n");    
    fclose(file);

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
    system(cmd);
}

void reportSuperBlock()
{
    char dotfile[25] = "superblock_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    if (file == NULL) return;

    fprintf(file, "digraph {\n");
    fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
    fprintf(file, "\tnode [shape = plain]\n");
    fprintf(file, "\trankdir = LR\n");

    fprintf(file, "\tSUPERBLOCK [\n");
    fprintf(file, "\t\tlabel = <\n");
    fprintf(file, "\t\t\t<table bgcolor = \"darkorchid\">\n");
    
    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td colspan = \"2\">Superblock</td>\n");
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Inodes count</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->inodes_count);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Blocks count</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->blocks_count);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Free blocks</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->free_blocks);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Free inodes</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->free_inodes);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Mounted date</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%s</td>\n", session.sb->mounted_date);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Unmounted date</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%s</td>\n", session.sb->unmounted_date);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Mounted count</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->blocks_count);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Magic</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->magic);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Inode size</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->inode_size);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Block size</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->block_size);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>First inode</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->first_inode);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>First block</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->first_block);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Bitmap inode start</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->bm_inode_start);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Bitmap block start</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->bm_block_start);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Inode start</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->inode_start);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Block start</td>\n");
    fprintf(file, "\t\t\t\t\t<td>%d</td>\n", session.sb->block_start);
    fprintf(file, "\t\t\t\t</tr>\n");

    fprintf(file, "\t\t\t</table>\n");
    fprintf(file, "\t\t>\n");
    fprintf(file, "\t]\n");

    fprintf(file, "}\n"); 
    fclose(file);

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
    system(cmd);
}

void reportFile()
{
    char dotfile[25] = "file_report.md";
    int no_block = 0;
    int ptr_inodo = 0;
    int no_inode = fs_getDirectoryByPath(values.ruta, __READ__, &no_block, &ptr_inodo);
    Inode * current = getInode(no_inode);
    char * text = fs_readFile(current);

    FILE * file;
    file = fopen(dotfile, "w");

    if (file == NULL) return;

    fprintf(file, "# %s\n", values.ruta);
    fprintf(file, "%s", text);
    fclose(file);

    char cmd[300] = {0};
    strcpy(cmd, "markdown-pdf ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
    system(cmd);
}

char * octalToLetters(int n)
{
    switch (n)
    {
    case 7:
        return "rwx";
    case 6:
        return "rw-";
    case 5:
        return "r-x";
    case 4:
        return "r--";
    case 3:
        return "-wx";
    case 2:
        return "-w-";
    case 1:
        return "--x";
    case 0:
        return "---";
    }
}

void reportLs_indirect(FILE * file, PointerBlock * current, int level)
{
    for (int i = 0; i < 16; i++)
    {
        if (current->pointers[i] < 0) continue;

        if (level > 1)
        {
            PointerBlock * pointer = (PointerBlock *)getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            reportLs_indirect(file, pointer, level - 1);
        }
        else
        {
            DirectoryBlock * block = (DirectoryBlock *)getGenericBlock(current->pointers[i], _DIRECTORY_TYPE_);

            for (int i = 0; i < 4; i++)
            {
                if (block->content[i].inode < 0) continue;
                
                Inode * child = getInode(block->content[i].inode);
                
                char str_perm[4] = {0};
                sprintf(str_perm, "%d", child->permission);

                fprintf(file, "\t\t\t\t<tr>\n");
                if (child->type == _FILE_TYPE_)
                    fprintf(file, "\t\t\t\t\t<td>-");
                else
                    fprintf(file, "\t\t\t\t\t<td>d");
                fprintf(file, "%s%s%s</td>\n", octalToLetters(str_perm[0] - '0'), octalToLetters(str_perm[1] - '0'), octalToLetters(str_perm[2] - '0'));
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", permissions[child->uid].name);
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", permissions[child->gid].group);
                fprintf(file, "\t\t\t\t\t<td>%d</td>\n", child->size);
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", child->create_date);
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", block->content[i].name);
                fprintf(file, "\t\t\t\t</tr>\n");
            }
        }
    }
}

void reportLs()
{
    int level = 1;
    int no_block = 0;
    int ptr_inodo = 0;
    int no_current = fs_getDirectoryByPath(values.ruta, __READ__, &no_block, &ptr_inodo);
    if (no_current < 0)
    {
        printf(ANSI_COLOR_RED "[e] No posee permisos para comando LS\n" ANSI_COLOR_RESET);
        return;
    }

    Inode * current = getInode(no_current);
    if (current->type == _FILE_TYPE_)
    {
        printf(ANSI_COLOR_RED "[e] LS no se puede ejecutar sobre un archivo\n" ANSI_COLOR_RESET);
        return;
    }
    
    char dotfile[20] = "ls_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    if (file == NULL) return;

    fprintf(file, "digraph {\n");
    fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
    fprintf(file, "\tnode [shape = plain]\n");
    fprintf(file, "\trankdir = TB\n");
    fprintf(file, "\tLS [\n");
    fprintf(file, "\t\tlabel = <\n");
    fprintf(file, "\t\t\t<table bgcolor = \"orchid\">\n");
    
    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Permission</td>\n");
    fprintf(file, "\t\t\t\t\t<td>User</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Group</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Size</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Date</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Name</td>\n");
    fprintf(file, "\t\t\t\t</tr>\n");

    for (int i = 0; i < 15; i++)
    {
        if (current->block[i] < 0) continue;

        if (i < 12)
        {
            DirectoryBlock * block = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            
            for (int i = 0; i < 4; i++)
            {
                if (block->content[i].inode < 0) continue;

                Inode * child = getInode(block->content[i].inode);
                
                char str_perm[4] = {0};
                sprintf(str_perm, "%d", child->permission);

                fprintf(file, "\t\t\t\t<tr>\n");
                if (child->type == _FILE_TYPE_)
                    fprintf(file, "\t\t\t\t\t<td>-");
                else
                    fprintf(file, "\t\t\t\t\t<td>d");
                fprintf(file, "%s%s%s</td>\n", octalToLetters(str_perm[0] - '0'), octalToLetters(str_perm[1] - '0'), octalToLetters(str_perm[2] - '0'));
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", permissions[child->uid].name);
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", permissions[child->gid].group);
                fprintf(file, "\t\t\t\t\t<td>%d</td>\n", child->size);
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", child->create_date);
                fprintf(file, "\t\t\t\t\t<td>%s</td>\n", block->content[i].name);
                fprintf(file, "\t\t\t\t</tr>\n");
            }
        }
        else
        {
            PointerBlock * pointer = (PointerBlock *) getGenericBlock(current->block[i], _FILE_TYPE_);
            reportLs_indirect(file, pointer, level);
            level++;
        }
    }

    fprintf(file, "\t\t\t</table>\n");
    fprintf(file, "\t\t>\n");
    fprintf(file, "\t]\n");

    fprintf(file, "}\n");    
    fclose(file);

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
    system(cmd);
}

void reportJournal()
{
    char dotfile[20] = "journal_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    int start = session.part_start + __SUPERBLOCK__;
    int count = session.sb->inodes_count;

    fprintf(file, "digraph {\n");
    fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
    fprintf(file, "\tnode [shape = plain]\n");
    fprintf(file, "\trankdir = TB\n");

    fprintf(file, "\tJOURNAL [\n");
    fprintf(file, "\t\tlabel = <\n");
    fprintf(file, "\t\t\t<table bgcolor = \"khaki\">\n");
    
    fprintf(file, "\t\t\t\t<tr>\n");
    fprintf(file, "\t\t\t\t\t<td>Operation</td>\n");
    fprintf(file, "\t\t\t\t\t<td>String 1</td>\n");
    fprintf(file, "\t\t\t\t\t<td>String 2</td>\n");
    fprintf(file, "\t\t\t\t\t<td>String 3</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Date</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Permission</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Owner</td>\n");
    fprintf(file, "\t\t\t\t\t<td>Size</td>\n");
    fprintf(file, "\t\t\t\t</tr>\n");

    Journal * journal = NULL;

    for (int i = 0; i < count; i++)
    {
        journal = getJournal(start);
        if (journal->command == _EMPTY_) break;

        fprintf(file, "\t\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", journal->command);
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", journal->str_1);
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", journal->str_2);
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", journal->str_3);
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", journal->date);
        fprintf(file, "\t\t\t\t\t<td>%s</td>\n", journal->ugo);
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", journal->owner);
        fprintf(file, "\t\t\t\t\t<td>%d</td>\n", journal->size);
        fprintf(file, "\t\t\t\t</tr>\n");

        start += __JOURNAL__;
    }

    fprintf(file, "\t\t\t</table>\n");
    fprintf(file, "\t\t>\n");
    fprintf(file, "\t]\n");

    fprintf(file, "\n}");
    fclose(file);

    char cmd[300] = {0};
    strcpy(cmd, "dot -T");
    strcat(cmd, getTypeFilename(values.path));
    strcat(cmd, " ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
    system(cmd);
}

/**
 * @brief Genera el reporte indicado
 * 
 */
void exec_rep ()
{
    if (strlen(values.name) == 0 || strlen(values.path) == 0 || strlen(values.id) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    clearSpaceDisk();
    char id_i = values.id[2];
    int id_a = values.id[3] - '0';

    int i = getDiskById(id_i);
    int j = getPartById(id_a, i);

    if (i == _ERROR_ || j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    if(!createDirectory(values.path))
    {
        printf(ANSI_COLOR_RED "[e] No se pudo crear el directorio %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }
    
    MBR mbr = getMBR(disks_mount[i].path);
    int i_ext = getNumberExtendedPart(mbr.partitions);
    EBR ebr = getEBR(disks_mount[i].path, mbr.partitions[i_ext].part_start);
    getSpaceLogicalDetail(disks_mount[i].path, ebr, mbr.partitions[i_ext].part_start + mbr.partitions[i_ext].part_size);

    /* REPORTES DE FASE 1 */
    if (strcmp(values.name, "disk") == 0)
    {
        reportDisk(mbr, disks_mount[i].path);
        clearSpaceDisk();
        return;
    }
    else if (strcmp(values.name, "mbr") == 0)
    {
        reportMBR(mbr, disks_mount[i].path);
        clearSpaceDisk();
        return;
    }
    
    /* REPORTES DE FASE 2 */
    if (session.id_user <= 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }

    Partition  part;
    int a;
    for (a = 0; a < 4; a++)
    {
        if (mbr.partitions[a].part_type == 'p')
        {
            if (strcmp(mbr.partitions[a].part_name, disks_mount[i].parts_mount[j].mount_name) == 0)
            {
                part = mbr.partitions[a];
                break;
            }
        }
    }
    
    if (i == 4) return;

    if (strcmp(values.name, "inode") == 0)
    {
        if (strcmp(permissions[session.id_user].group, "root") != 0)
        {
            printf(ANSI_COLOR_RED "[e] Reporte solo para usuarios root\n" ANSI_COLOR_RESET);
            return;
        }

        reportInodes();
    }
    else if (strcmp(values.name, "block") == 0)
    {
        if (strcmp(permissions[session.id_user].group, "root") != 0)
        {
            printf(ANSI_COLOR_RED "[e] Reporte solo para usuarios root\n" ANSI_COLOR_RESET);
            return;
        }

        reportBlocks();
    }
    else if (strcmp(values.name, "bm_inode") == 0)
    {
        if (strcmp(permissions[session.id_user].group, "root") != 0)
        {
            printf(ANSI_COLOR_RED "[e] Reporte solo para usuarios root\n" ANSI_COLOR_RESET);
            return;
        }

        reportBitmap(_INODE_);
    }
    else if (strcmp(values.name, "bm_block") == 0)
    {
        if (strcmp(permissions[session.id_user].group, "root") != 0)
        {
            printf(ANSI_COLOR_RED "[e] Reporte solo para usuarios root\n" ANSI_COLOR_RESET);
            return;
        }

        reportBitmap(_BLOCK_);
    }
    else if (strcmp(values.name, "tree") == 0)
    {
        if (strcmp(permissions[session.id_user].group, "root") != 0)
        {
            printf(ANSI_COLOR_RED "[e] Reporte solo para usuarios root\n" ANSI_COLOR_RESET);
            return;
        }

        reportTree();
    }
    else if (strcmp(values.name, "sb") == 0)
    {
        if (strcmp(permissions[session.id_user].group, "root") != 0)
        {
            printf(ANSI_COLOR_RED "[e] Reporte solo para usuarios root\n" ANSI_COLOR_RESET);
            return;
        }

        reportSuperBlock();
    }
    else if (strcmp(values.name, "file") == 0)
        reportFile();
    else if (strcmp(values.name, "ls") == 0)
        reportLs();
    else if (strcmp(values.name, "journaling") == 0)
    {
        if (strcmp(permissions[session.id_user].group, "root") != 0)
        {
            printf(ANSI_COLOR_RED "[e] Reporte solo para usuarios root\n" ANSI_COLOR_RESET);
            return;
        }

        reportJournal();
    }

    clearSpaceDisk();
}

#endif