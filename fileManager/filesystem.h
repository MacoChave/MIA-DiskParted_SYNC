#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string.h>
#include <time.h>

#include "../var/globals.h"
#include "../fileManager/manager.h"

extern void fs_traversalTree(Inode * current, int command, int ugo);
extern int fs_traversalModify(Inode * current, int operation);

/**
 * @brief Leer archivo de texto en sistema de archivos en apuntadores indirectos
 * 
 * @param current 
 * @param level 
 * @return char* 
 */
char * fs_readFile_Indirect(PointerBlock * current, int level)
{
    char * text = (char *)calloc(279552, sizeof(char));

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            if (current->pointers[i] == -1) continue;

            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            strcat(text, fs_readFile_Indirect(pb, level -1));
        }
        else 
        {
            if (current->pointers[i] == -1) continue;

            FileBlock * fb = (FileBlock *)getGenericBlock(current->pointers[i], _FILE_TYPE_);
            strcat(text, fb->content);
        }
    }
    return text;
}

/**
 * @brief Leer archivo de texto en sistema de archivos
 * 
 * @param current 
 * @return char* 
 */
char * fs_readFile(Inode * current)
{
    int level = 1;
    char * text = (char *)calloc(280320, sizeof(char));

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            if (current->block[i] == -1) continue;
            
            FileBlock * fb = (FileBlock *) getGenericBlock(current->block[i], _FILE_TYPE_);
            strcat(text, fb->content);
        }
        else 
        {
            /* BLOQUES INDIRECTOS */
            if (current->block[i] == -1) continue;

            PointerBlock * pb = NULL;
            pb = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            strcat(text, fs_readFile_Indirect(pb, level));

            level++;
        }
    }
    return text;
}

/**
 * @brief Escribir archivo de texto en sistema de archivos en apuntadores indirectos
 * 
 * @param text 
 * @param current 
 * @param no_current 
 * @param level 
 * @return int 
 */
int fs_writeFile_Indirect(char text[], PointerBlock * current, int no_current, int level)
{
    int result = 0;
    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            PointerBlock * pb = NULL;
            if (current->pointers[i] == -1)
            {
                pb = newPointerBlock();
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, pb);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else 
                pb = (PointerBlock *)getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            
            result = fs_writeFile_Indirect(text, pb, current->pointers[i], level -1);
            if (result) return result;
        }
        else 
        {
            FileBlock * fb = NULL;
            if (current->pointers[i] == -1)
            {
                fb = (FileBlock *) newFileBlock();
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, fb);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            }
            else 
            {
                fb = (FileBlock *)getGenericBlock(current->pointers[i], _FILE_TYPE_);
                memset(fb->content, 0, 64);
                updateGenericBlock(current->pointers[i], fb);
            }

            if (strlen(fb->content) < 64)
            {
                strcpy(fb->content, text);
                updateGenericBlock(current->pointers[i], fb);

                return 1;
            }
        }
        return 0;
    }
}

/**
 * @brief Escribir archivo de texto en sistema de archivos
 * 
 * @param text 
 * @param current 
 * @param no_current 
 * @param i 
 */
void fs_writeFile(char text[], Inode * current, int no_current, int i)
{
    int level = 1;

    if (i < 12)
    {
        /* BLOQUES DIRECTOS */
        FileBlock * bf = NULL;
        if (current->block[i] == -1)
        {
            bf = newFileBlock();
            current->block[i] = session.sb->first_block;
            updateInode(no_current, current);
            updateGenericBlock(_EMPTY_, bf);
            updateBitmap(current->block[i], '1', _BLOCK_);
            session.sb->free_blocks -= 1;
            session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            updateSuperBlock();
        }
        else
        {
            bf = (FileBlock *)getGenericBlock(current->block[i], _FILE_TYPE_);
            memset(bf->content, 0, 64);
        }
        strcpy(bf->content, text);
        updateGenericBlock(current->block[i], bf);
    }
    else 
    {
        /* BLOQUES INDIRECTOS */
        int result = 0;
        while (!result && level < 4)
        {
            PointerBlock * pb = NULL;
            if (current->block[11 + level] == -1)
            {
                pb = newPointerBlock();
                current->block[11 + level] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(current->block[11 + level], pb);
                updateBitmap(current->block[11 + level], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else 
                pb = (PointerBlock *)getGenericBlock(current->block[11 + level], _POINTER_TYPE_);
        
            result = fs_writeFile_Indirect(text, pb, current->block[11 + level], level);
            level++;
        }
    }
}

/**
 * @brief Obtener archivo de texto en sistema de archivos desde archivo externo
 * 
 * @param cont 
 * @param current 
 * @param no_current 
 */
void fs_generateContent_cont(char cont[300], Inode * current, int no_current)
{
    int i = 0;
    int size = 0;
    char text[64] = {0};
    FILE * file = fopen(cont, "r");

    current->size = 0;

    if (file != NULL)
    {
        while(!feof(file)){
            fgets(text, 64, file);
            size += strlen(text);
            if (strlen(text) > 0)
            {
                fs_writeFile(text, current, no_current, i);
                current->size += strlen(text);
            }
            memset(text, 0, 64);
            i++;
        }
        
        updateInode(no_current, current);
        fclose(file);
    }
}

/**
 * @brief Generar archivo de texto en sistema de archivos
 * 
 * @param size 
 * @param current 
 * @param no_current 
 */
void fs_generateContent_size(int size, Inode * current, int no_current)
{
    char text[64] = {0};
    current->size = size;
    int i = 0;
    int level = 1;
    int blocks_count = size / 64;
    
    if (size % 64 > 0)
        blocks_count++;
    
    while (i < blocks_count)
    {
        for (int j = 0, a = 0; j < 64; j++, a++)
        {
            char aux[3] = {a + '0', '\0'};
            strcat(text, aux);
            if (a == 9) a = -1;
        }

        fs_writeFile(text, current, no_current, i);
        memset(text, 0, 64);
        i++;
    }
    
    updateInode(no_current, current);
}

void fs_pushContent_Indirect(char content[], PointerBlock * current, int no_current, int level)
{

}

void fs_pushContent(char content[], Inode * current, int no_current)
{
    int i = (current->size / 64) + 1;

    if (i < 12)
    {
        FileBlock * bf = NULL;
        current->size += strlen(content);

        if (current->block[i] < 0)
        {
            bf = newFileBlock();
            current->block[i] = session.sb->first_block;
            updateInode(no_current, current);
            updateGenericBlock(_EMPTY_, bf);
            updateBitmap(current->block[i], '1', _BLOCK_);
            session.sb->free_blocks -= 1;
            session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
        }
        else
            bf = (FileBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
        
        strcat(bf->content, content);
        updateGenericBlock(current->block[i], bf);
    }
    else
    {
        i -= 12;
        // PTRS DIRECTOS   12       768
        // 1ROS INDIRECTOS 16       1024
        // 2DOS INDIRECTOS 256      16384
        // 3ROS INDIRECTOS 4096     262144
 
        /*
         * 320
         * 384
         * 
         * 350
         * 34
         */
    }
}

int fs_pushDirectory_Indirect(char name[], int no_inode, PointerBlock * current, int no_current, int level)
{
    int result = -1;

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            PointerBlock * new_bp = newPointerBlock();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, new_bp);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                new_bp = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);

            result = fs_pushDirectory_Indirect(name, no_inode, new_bp, current->pointers[i], level - 1);
            if (result > 0) return result;
        }
        else
        {
            DirectoryBlock * bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->pointers[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    bd->content[j].inode = no_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->pointers[i], bd);
                    return bd->content[j].inode;
                }
            }
        }
    }
    return result;
}

int fs_pushDirectory(char name[], int no_inode, Inode * current, int no_current)
{
    int no_next = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    bd->content[j].inode = no_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->block[i], bd);
                 
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            PointerBlock * bp = newPointerBlock();
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bp);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bp = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            
            no_next = fs_pushDirectory_Indirect(name, no_inode, bp, current->block[i], level);
            if (no_next > 0)
                return no_next;
            level++;
        }
    }

    return no_next;
}

/**
 * @brief Buscar directorios por nombre en apuntadores indirectos
 * 
 * @param name 
 * @param bp 
 * @param level 
 * @return int 
 */
int fs_getDirectoryByName_Indirect(char name[], PointerBlock * bp, int level, int * no_block, int * ptr_inodo)
{
    int no_inode = -1;
    for (int i = 0; i < 16; i++)
    {
        if (bp->pointers[i] < 0) continue;
        if (level > 1){
            no_inode = fs_getDirectoryByName_Indirect(name, bp, level - 1, no_block, ptr_inodo);
            if (no_inode > 0) return no_inode; 
            else continue;
        }
    
        DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(bp->pointers[i], _DIRECTORY_TYPE_);
        for (int j = 0; j < 4; j++)
        {
            if (bd->content[j].inode < 0) continue;
            if (strcmp(name, bd->content[j].name) == 0)
            {
                *no_block = bp->pointers[i];
                *ptr_inodo = j;
                return bd->content[j].inode;
            }
        }
    }
    return no_inode;
}

/**
 * @brief Buscar directorios por nombre
 * 
 * @param name 
 * @param current 
 * @return int 
 */
int fs_getDirectoryByName(char name[], Inode * current, int * no_block, int * ptr_inodo)
{
    int no_inode = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (current->block[i] < 0) continue;
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * bd = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0) continue;
                if (strcmp(name, bd->content[j].name) == 0)
                {
                    *no_block = current->block[i];
                    *ptr_inodo = j;
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            /*  BLOQUES INDIRECTOS */
            PointerBlock * bp = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            no_inode = fs_getDirectoryByName_Indirect(name, bp, level, no_block, ptr_inodo);
            if (no_inode > 0)
                return no_inode;
            level++;
        }
    }

    return no_inode;
}

/**
 * @brief Crear directorio en sistema de archivos en apuntadores indirectos
 * 
 * @param name 
 * @param current 
 * @param no_upper 
 * @param no_current 
 * @param level 
 * @return int 
 */
int fs_createDirectory_Indirect(char name[], PointerBlock * current, int no_upper, int no_current, int level)
{
    int no_inode = -1;

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            PointerBlock * new_bp = newPointerBlock();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, new_bp);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                new_bp = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);

            no_inode = fs_createDirectory_Indirect(name, new_bp, no_upper, current->pointers[i], level - 1);
            if (no_inode > 0) return no_inode;
        }
        else
        {
            DirectoryBlock * bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->pointers[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_DIRECTORY_TYPE_);
                    DirectoryBlock * new_bd = newDirectoryBlock(session.sb->first_inode, no_upper);
                    next->block[0] = session.sb->first_block;
                    bd->content[j].inode = session.sb->first_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->pointers[i], bd);
                    updateInode(_EMPTY_, next);
                    updateGenericBlock(_EMPTY_, new_bd);
                    updateBitmap(bd->content[j].inode, '1', _INODE_);
                    updateBitmap(next->block[0], '1', _BLOCK_);
                    session.sb->free_inodes -= 1;
                    session.sb->free_blocks -= 1;
                    session.sb->first_inode = getNextFreeBit_Bitmap(_INODE_);
                    session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
    }
    return no_inode;
}

/**
 * @brief Crear directorio en sistema de archivos
 * 
 * @param name 
 * @param current 
 * @param no_current 
 * @return int 
 */
int fs_createDirectory(char name[], Inode * current, int no_current)
{
    int no_next = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_DIRECTORY_TYPE_);
                    DirectoryBlock * new_bd = newDirectoryBlock(session.sb->first_inode, no_current);
                    next->block[0] = session.sb->first_block;

                    bd->content[j].inode = session.sb->first_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->block[i], bd);
                    updateInode(_EMPTY_, next);
                    updateGenericBlock(_EMPTY_, new_bd);
                    updateBitmap(bd->content[j].inode, '1', _INODE_);
                    updateBitmap(next->block[0], '1', _BLOCK_);
                    session.sb->free_inodes -= 1;
                    session.sb->free_blocks -= 1;
                    session.sb->first_inode = getNextFreeBit_Bitmap(_INODE_);
                    session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            PointerBlock * bp = newPointerBlock();
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bp);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bp = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            
            no_next = fs_createDirectory_Indirect(name, bp, no_current, current->block[i], level);
            if (no_next > 0)
                return no_next;
            level++;
        }
    }

    return no_next;
}

/**
 * CREAR INODO DE ARCHIVO
 * */

/**
 * @brief Crear archivo en sistema de archivos
 * 
 * @param name 
 * @param current 
 * @param no_current 
 * @param level 
 * @return int 
 */
int fs_createFile_Indirect(char name[], PointerBlock * current, int no_current, int level)
{
    int no_inode = -1;

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            PointerBlock * new_bp = newPointerBlock();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, new_bp);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                new_bp = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);

            no_inode = fs_createFile_Indirect(name, new_bp, current->pointers[i], level - 1);
            if (no_inode > 0) return no_inode;
        }
        else
        {
            DirectoryBlock * new_bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = session.sb->first_block;
                updateGenericBlock(no_current, current);
                updateGenericBlock(_EMPTY_, new_bd);
                updateBitmap(current->pointers[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                new_bd = (DirectoryBlock *) getGenericBlock(current->pointers[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (new_bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_FILE_TYPE_);
                    new_bd->content[j].inode = session.sb->first_inode;
                    strcpy(new_bd->content[j].name, name);
                    updateGenericBlock(current->pointers[i], new_bd);
                    updateInode(_EMPTY_, next);
                    updateBitmap(new_bd->content[j].inode, '1', _INODE_);
                    session.sb->free_inodes -= 1;
                    session.sb->first_inode = getNextFreeBit_Bitmap(_INODE_);
                    updateSuperBlock();
                    return new_bd->content[j].inode;
                }
            }
        }
    }
    return no_inode;
}

/**
 * @brief Crear archivo en sistema de archivos
 * 
 * @param name 
 * @param current 
 * @param no_current 
 * @return int 
 */
int fs_createFile(char name[], Inode * current, int no_current)
{
    int no_next = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * bd = NULL;
            if (current->block[i] == -1)
            {
                bd = newDirectoryBlock(_EMPTY_, _EMPTY_);
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(_EMPTY_, bd);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
            }
            else
                bd = (DirectoryBlock *) getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_FILE_TYPE_);
                    bd->content[j].inode = session.sb->first_inode;
                    strcpy(bd->content[j].name, name);
                    updateGenericBlock(current->block[i], bd);
                    updateInode(_EMPTY_, next);
                    updateBitmap(bd->content[j].inode, '1', _INODE_);
                    session.sb->free_inodes -= 1;
                    int next_inode = getNextFreeBit_Bitmap(_INODE_);
                    session.sb->first_inode = next_inode;
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            PointerBlock * bp = newPointerBlock();
            if (current->block[i] == -1)
            {
                current->block[i] = session.sb->first_block;
                updateInode(no_current, current);
                updateGenericBlock(current->block[i], bp);
                updateBitmap(current->block[i], '1', _BLOCK_);
                session.sb->free_blocks -= 1;
                session.sb->first_block = getNextFreeBit_Bitmap(_BLOCK_);
                updateSuperBlock();
            }
            else
                bp = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            
            no_next = fs_createFile_Indirect(name, bp, current->block[i], level);
            if (no_next > 0)
                return no_next;
            level++;
        }
    }

    return no_next;
}

/**
 * VERIFICAR PERMISOS
 * */

/**
 * @brief Verificar permisos dentro del sistema de archivos
 * 
 * @param uid 
 * @param gid 
 * @param inode_permission 
 * @param operation 
 * @return int 
 */
int fs_checkPermission(int uid, int gid, int inode_permission, char operation)
{
    if (strcmp(permissions[session.id_group].group, "root") == 0) return 1;
    
    char str_perm[4];
    sprintf(str_perm, "%d", inode_permission);
    int u = str_perm[0] - '0';
    int g = str_perm[1] - '0';
    int o = str_perm[2] - '0';
    int sameGroup = (permissions[session.id_group].id == permissions[gid].id) ? 1 : 0;
    int userIsOwner = (permissions[session.id_user].id == permissions[uid].id) ? 1 : 0;

    switch (operation)
    {
        case __CREATE__: // > 6
            if (o >= 6) return 1;
            if (sameGroup && g >= 6) return 1;
            if (userIsOwner && u >= 6) return 1;
            break;
        case __READ__: // > 4
            if (o >= 4) return 1;
            if (sameGroup && g >= 4) return 1;
            if (userIsOwner && u >= 4) return 1;
            break;
        case __UPDATE__: // >= 6
            if (o >= 6) return 1;
            if (sameGroup && g >= 6) return 1;
            if (userIsOwner && u >= 6) return 1;
            break;
        case __DELETE__: // >= 6
            if (o >= 6) return 1;
            if (sameGroup && g >= 6) return 1;
            if (userIsOwner && u >= 6) return 1;
            break;
        default: 
            break;
    }
    return 0;
}

/**
 * @brief Crear directorio desde ruta en sistema de archivos
 * 
 * @param path 
 * @param isRecursive 
 * @param inodeType 
 * @param operation 
 * @return int 
 */
int fs_createDirectoryFromPath(char path[], int isRecursive, char inodeType, char operation)
{
    int pivot = 0;
    int lenght_path = strlen(path);
    int no_container = 0;
    int no_next = 0;
    int no_block = 0;
    int ptr = 0;
    char * str_path = NULL;
    Inode * current = getInode(0);
    str_path = strtok(path, "/");

    while (str_path != NULL)
    {
        pivot += strlen(str_path) + 1;
        no_container = no_next;
        int hasPermission = fs_checkPermission(current->uid, current->gid, current->permission, operation);
        no_next = fs_getDirectoryByName(str_path, current, &no_block, &ptr);
        if (no_next < 0)
        {
            if (isRecursive || (operation == __CREATE__ && pivot == lenght_path))
            {
                if (!hasPermission) return -1;
                if (inodeType == _DIRECTORY_TYPE_ || pivot != lenght_path)
                {
                    no_next = fs_createDirectory(str_path, current, no_container);
                    current = getInode(no_next);
                }
                else if (inodeType == _FILE_TYPE_)
                {
                    no_next = fs_createFile(str_path, current, no_container);
                    return no_next;
                }
            }
            else return no_next;
        }
        else
        {
            if (!hasPermission) return -1;
            current = getInode(no_next);
            if (current->type == _FILE_TYPE_) return no_next;
        }
        str_path = strtok(NULL, "/");
    }
    return no_next;
}

/**
 * @brief Buscar directorio por ruta
 * 
 * @param path 
 * @param operation 
 * @return int 
 */
int fs_getDirectoryByPath(char path[], char operation, int * no_block, int * ptr_inodo)
{
    int pivot = 0;
    int lenght_path = strlen(path);
    int no_container = 0;
    int no_next = 0;
    char * str_path = NULL;
    Inode * current = getInode(0);
    str_path = strtok(path, "/");

    while (str_path != NULL)
    {
        pivot += strlen(str_path) + 1;
        no_container = no_next;
        int hasPermission = fs_checkPermission(current->uid, current->gid, current->permission, operation);
        no_next = fs_getDirectoryByName(str_path, current, no_block, ptr_inodo);
        if (no_next < 0) return -1;
        if (pivot == lenght_path) return no_next;
        if (!hasPermission && no_container != 0) return -1;
        
        current = getInode(no_next);
        str_path = strtok(NULL, "/");
    }
    return no_next;
}

void fs_updatePermission()
{
    char perm[960] = {0};
    char text[64] = {0};
    
    for (int i = 0; i < 20; i++)
    {
        if (permissions[i].type == '0') continue;

        if (strlen(perm) == 0)
            sprintf(perm, "%d,%c,%s", permissions[i].id, permissions[i].type, permissions[i].group);
        else
            sprintf(perm, "%s,%d,%c,%s", perm, permissions[i].id, permissions[i].type, permissions[i].group);
        
        if (permissions[i].type == 'U')
        {
            strcat(perm, ",");
            strcat(perm, permissions[i].name);
            strcat(perm, ",");
            strcat(perm, permissions[i].pass);
        }
        strcat(perm, "\n");
    }
    
    int pos = 0;

    Inode * current = getInode(1);

    for (int i = 0; i < 15; i++)
    {
        if (perm[pos] == 0) break;

        for (int j = 0; j < 64; j++)
        {
            if (perm[pos] == 0) break;

            char aux[3] = {perm[pos], '\0'};
            strcat(text, aux);
            pos++;
        }
        
        fs_writeFile(text, current, 1, i);
        memset(text, 0, 64);
    }
}

void fs_traversalTree_Indirect(PointerBlock * current, int command, int ugo, int level)
{
    int result = 0;
    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            if (current->pointers[i] == -1) continue;

            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            fs_traversalTree_Indirect(pb, command, ugo, level);
        }
        else 
        {
            if (current->pointers[i] == -1) continue;
            
            Inode * child = getInode(current->pointers[i]);
            if(!fs_checkPermission(child->uid, child->gid, child->permission, __UPDATE__))
                continue;
            
            if (command == _CHMOD_)
                child->permission = ugo;
            updateInode(current->pointers[i], child);
            if (child->type == _DIRECTORY_TYPE_)
                fs_traversalTree(child, command, ugo);
            if (command == _REM_)
            {
                updateBitmap(current->pointers[i], '0', _INODE_);
            }
        }
    }
}

void fs_traversalTree(Inode * current, int command, int ugo)
{
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            if (current->block[i] == -1) continue;
            DirectoryBlock * db = (DirectoryBlock *)getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            for (int i = 0; i < 4; i++)
            {
                if (db->content[i].inode < 0) continue;
                if (strcmp(db->content[i].name, ".") == 0) continue;
                if (strcmp(db->content[i].name, "..") == 0) continue;

                Inode * child = getInode(db->content[i].inode);
                if(!fs_checkPermission(child->uid, child->gid, child->permission, __UPDATE__))
                    continue;
                
                if (command == _CHMOD_)
                    child->permission = ugo;
                
                updateInode(db->content[i].inode, child);
                if (child->type == _DIRECTORY_TYPE_)
                    fs_traversalTree(child, command, ugo);
                if (command == _REM_)
                {
                    updateBitmap(db->content[i].inode, '0', _INODE_);
                }
            }
        }
        else 
        {
            /* BLOQUES INDIRECTOS */
            if (current->block[i] == -1) continue;

            PointerBlock * pb = NULL;
            pb = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            fs_traversalTree_Indirect(pb, command, ugo, level);

            level++;
        }
    }
}

int fs_traversalModify_Indirect(PointerBlock * current, int level, int operation)
{
    int response = 1;

    for (int i = 0; i < 16; i++)
    {
        if (current->pointers[i] == -1) continue;

        if (level > 1)
        {
            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->pointers[i], _POINTER_TYPE_);
            response = fs_traversalModify_Indirect(pb, level -1, operation);
            if (!response) return response;
        }
        else 
        {
            DirectoryBlock * db = (DirectoryBlock *)getGenericBlock(current->pointers[i], _DIRECTORY_TYPE_);
            for (int j = 0; j < 4; j++)
            {
                if (db->content[j].inode < 0) continue;
                if (strcmp(db->content[j].name, ".") == 0) continue;
                if (strcmp(db->content[j].name, "..") == 0) continue;

                Inode * child = getInode(db->content[j].inode);
                response = fs_checkPermission(child->uid, child->gid, child->permission, operation);
                if (!response) return response;
                if (child->type == _FILE_TYPE_) continue;

                response = fs_traversalModify(child, operation);
                if (!response) return response;
            }
        }
    }
    return response;
}

int fs_traversalModify(Inode * current, int operation)
{
    int response = 1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (current->block[i] == -1) continue;
        
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            DirectoryBlock * db = (DirectoryBlock *)getGenericBlock(current->block[i], _DIRECTORY_TYPE_);
            for (int j = 0; j < 4; j++)
            {
                if (db->content[j].inode < 0) continue;
                if (strcmp(db->content[j].name, ".") == 0) continue;
                if (strcmp(db->content[j].name, "..") == 0) continue;

                Inode * child = getInode(db->content[j].inode);
                
                response = fs_checkPermission(child->uid, child->gid, child->permission, operation);
                if (!response) return response;
                if (child->type == _FILE_TYPE_) continue;

                response = fs_traversalModify(child, operation);
                if (!response) return response;
            }
        }
        else 
        {
            /* BLOQUES INDIRECTOS */
            PointerBlock * pb = (PointerBlock *) getGenericBlock(current->block[i], _POINTER_TYPE_);
            response = fs_traversalModify_Indirect(pb, level, operation);
            if (!response) return response;

            level++;
        }
    }

    return response;
}

void fs_backup(Journal * journal)
{
    int start = session.part_start + __SUPERBLOCK__;
    int count = session.sb->inodes_count;

    __time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(journal->date, sizeof(journal->date), "%d/%m/%y %H:%M", date);

    Journal * current = NULL;

    for(int i = 0; i < count; i++)
    {
        current = getJournal(start);
        if (current->command == _EMPTY_) break;

        start += __JOURNAL__;
    }

    updateJournal(journal, start);
}

#endif // FILESYSTEM_H