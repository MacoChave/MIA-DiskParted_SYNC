#ifndef MV_H
#define MV_H

#include <string.h>

#include "../var/filename.h"
#include "../fileManager/filesystem.h"

void exec_mv()
{
    if (session.id_user < 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay usuario activo\n" ANSI_COLOR_RESET);
        return;
    }

    if (strlen(values.path) == 0 || strlen(values.dest) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    Journal * journal = newJournal();
    journal->command = _MV_;
    journal->owner = session.id_user;
    strcpy(journal->str_1, values.path);
    strcpy(journal->str_2, values.dest);

    int no_origin_block = 0;
    int ptr_origin_inode = 0;
    int no_dest_block = 0;
    int ptr_dest_inode = 0;
    int no_origin = fs_getDirectoryByPath(values.path, __UPDATE__, &no_origin_block, &ptr_origin_inode);
    int no_dest = fs_getDirectoryByPath(values.dest, __DELETE__, &no_dest_block, &ptr_dest_inode);

    if (no_origin < 0 || no_dest < 0)
    {
        printf(ANSI_COLOR_RED "[e] No se encontró uno o varios directorios\n" ANSI_COLOR_RESET);
        return;
    }

    Inode * origin = getInode(no_origin);
    Inode * dest = getInode(no_dest);
    int origin_result = fs_checkPermission(origin->uid, origin->gid, origin->permission, __DELETE__);
    int dest_result = fs_checkPermission(dest->uid, dest->gid, dest->permission, __UPDATE__);
    
    if (!dest_result)
    {
        printf(ANSI_COLOR_RED "[e] No tiene permisos en %s\n" ANSI_COLOR_RESET, values.dest);
        return;
    }
    if (!origin_result)
    {
        printf(ANSI_COLOR_RED "[e] No tiene permisos en %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }
    
    if (dest->type == _FILE_TYPE_)
    {
        printf(ANSI_COLOR_RED "[e] El destino no puede ser un archivo\n" ANSI_COLOR_RESET);
        return;
    }
    if (origin->type == _DIRECTORY_TYPE_)
        origin_result = fs_traversalModify(origin, __DELETE__);
    
    if (origin_result)
    {
        DirectoryBlock * db = (DirectoryBlock *)getGenericBlock(no_origin_block, _DIRECTORY_TYPE_);
        
        fs_pushDirectory(db->content[ptr_origin_inode].name, db->content[ptr_origin_inode].inode, dest, no_dest);
        
        db->content[ptr_origin_inode].inode = -1;
        memset(db->content[ptr_origin_inode].name, 0, 12);
        updateGenericBlock(no_origin_block, db);

        if (command != _RECOVERY_)
            fs_backup(journal);

        printf(ANSI_COLOR_GREEN "[i] Se ha movido el directorio\n" ANSI_COLOR_RESET);
    }
    else
        printf(ANSI_COLOR_RED "[e] No tiene todos los permisos\n" ANSI_COLOR_RESET);
}

#endif //MV_H