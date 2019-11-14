#ifndef LOGIN_H
#define LOGIN_H

#include "../var/globals.h"
#include "../fileManager/mpartition.h"
#include "../fileManager/filesystem.h"

/* Inodo users.txt = 1 */

typedef struct Row
{
    char text[40];
} Row;

Row rows[20];

void initRows()
{
    for (int i = 0; i < 20; i++)
        memset(rows[i].text, 0, 40);
}

void fillUsersTable()
{
    Inode * current = getInode(1);
    if (current == NULL) return;

    char * text = NULL;
    initRows();
    text = fs_readFile(current);

    int i = 0;
    char * str_row;
    str_row = strtok(text, "\n");

    while (str_row != NULL)
    {
        strcpy(rows[i].text, str_row);
        str_row = strtok(NULL, "\n");
        i++;
    }

    i = 0;
    int no_col = 0;
    char * str_columns;

    while (i < 20)
    {
        str_columns = strtok(rows[i].text, ",");
        while (str_columns != NULL)
        {
            switch (no_col)
            {
            case 0: /* id */
                permissions[i].id = atoi(str_columns);
                break;
            case 1: /* type */
                permissions[i].type = str_columns[0];
                break;
            case 2: /* group */
                strcpy(permissions[i].group, str_columns);
                break;
            case 3: /* user */
                strcpy(permissions[i].name, str_columns);
                break;
            case 4: /* pass */
                strcpy(permissions[i].pass, str_columns);
                break;
            default:
                break;
            }
            no_col++;
            str_columns = strtok(NULL, ",");
        }
        no_col = 0;
        i++;
    }
}

int validateUser()
{
    for (int i = 0; i < 20; i++)
    {
        if (permissions[i].id > 0 && permissions[i].type == 'U')
        {
            if (strcmp(permissions[i].name, values.usr) == 0 && strcmp(permissions[i].pass, values.pwd) == 0)
                return i;
        }
    }
    return -1;
}

void getIdGroup()
{
    for (int i = 0; i < 20; i++)
    {
        if (permissions[i].type == 'G')
        {
            if (strcmp(permissions[i].group, permissions[session.id_user].group) == 0)
            {
                session.id_group = i;
                break;
            }
        }
    }
}

void exec_login()
{
    if (session.id_user > 0)
    {
        printf(ANSI_COLOR_RED "[e] Cierre sesión antes de iniciar una nueva sesión\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.usr) == 0 || strlen(values.pwd) == 0 || strlen(values.id) == 0)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }
    
    char id_i = values.id[2];
    char id_j = values.id[3] - '0';

    int i = getDiskById(id_i);
    int j = getPartById(id_j, i);

    if (j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    strcpy(session.path, disks_mount[i].path);
    session.part_start = disks_mount[i].parts_mount[j].mount_start;
    session.part_size = disks_mount[i].parts_mount[j].mount_size;
    session.sb = getSuperBlock();

    initPermissionlist();
    
    fillUsersTable();
    int id_user = validateUser();
    if (id_user < 0)
    {
        printf(ANSI_COLOR_RED "[e] Usuario y/o contraseña incorrecta\n" ANSI_COLOR_RESET);
        return;
    }

    session.id_user = id_user;
    getIdGroup();

    printf(ANSI_COLOR_GREEN "[i] Usuario %s logueado\n" ANSI_COLOR_RESET, values.usr);
}

#endif // LOGIN_H