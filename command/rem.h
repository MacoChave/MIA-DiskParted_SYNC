#ifndef REM_H
#define REM_H

#include <string.h>

#include "../var/filename.h"
#include "../fileManager/filesystem.h"

void exec_rem()
{
    if (session.id_user < 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay sesión activa\n" ANSI_COLOR_RESET);
        return;
    }

    if (strlen(values.path) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetro path requerido\n" ANSI_COLOR_RESET);
        return;
    }

    Journal * journal = newJournal();
    journal->command = _REM_;
    strcpy(journal->str_1, values.path);

    int no_block = 0;
    int ptr_inodo = 0;
    int no_inode = fs_getDirectoryByPath(values.path, __DELETE__, &no_block, &ptr_inodo);

    if (no_inode < 0)
    {
        printf(ANSI_COLOR_RED "[e] No se encontró uno o varios directorios\n" ANSI_COLOR_RESET);
        return;
    }

    Inode * current = getInode(no_inode);
    int result = fs_checkPermission(current->uid, current->gid, current->permission, __DELETE__);
    if (!result)
    {
        printf(ANSI_COLOR_RED "[e] No tiene permisos\n" ANSI_COLOR_RESET);
        return;
    }
    if (current->type == _DIRECTORY_TYPE_)
        result = fs_traversalModify(current, __DELETE__);
    
    if (result)
    {
        fs_traversalTree(current, command, 777);
        DirectoryBlock * db = (DirectoryBlock *)getGenericBlock(no_block, _DIRECTORY_TYPE_);
        updateBitmap(db->content[ptr_inodo].inode, '0', _INODE_);
        
        db->content[ptr_inodo].inode = -1;
        memset(db->content[ptr_inodo].name, 0, 12);
        updateGenericBlock(no_block, db);


        if (command != _RECOVERY_)
            fs_backup(journal);

        printf(ANSI_COLOR_GREEN "[i] Se eliminó %s y todo su contenido\n" ANSI_COLOR_RESET, values.path);
    }
    else
        printf(ANSI_COLOR_RED "[e] No tiene todos los permisos\n" ANSI_COLOR_RESET);
}

#endif // REM_H