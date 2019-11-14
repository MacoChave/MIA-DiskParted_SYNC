/**
 * @file interpreter.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los métodos necesarios para el funcionamiento del interprete
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ctype.h>
#include "var/globals.h"

#include "command/mkdisk.h"
#include "command/rmdisk.h"
#include "command/fdisk.h"
#include "command/mount.h"
#include "command/unmount.h"
#include "command/rep.h"
#include "command/mkfs.h"
#include "command/login.h"
#include "command/logout.h"
#include "command/mkgrp.h"
#include "command/rmgrp.h"
#include "command/mkusr.h"
#include "command/rmusr.h"
#include "command/mkdir.h"
#include "command/mkfile.h"
#include "command/cat.h"
#include "command/chgrp.h"
#include "command/edit.h"
#include "command/chmod.h"
#include "command/ren.h"
#include "command/rem.h"
#include "command/mv.h"
#include "command/loss.h"
#include "command/recovery.h"

#include "command/sync.h"

extern void exec_exec();

/**
 * @brief Recibe una línea de comando, y parametriza el tipo de comando y 
 * atributos para realizar la acción correspondiente
 * 
 * @param input 
 * @return int 
 */
int loadCommand(char input[]) 
{
    strcat(input, "$"); // EOF

    int i = 0;
    int param = _ERROR_;

    int step = 0;
    int quotation_marks = 0;
    int comment = 0;

    char auxiliar[300] = {0};

    while (input[i] != 0)
    {
        if (input[i] == '#')
        {
            comment = !comment;
            input[i] = '$';
        }
        if (input[i] == '\n' || input[i] == '\r')
            input[i] = ' ';
        if (input[i] == '$')
            break;

        if (input[i] == '-')
        {
            if (param == _ERROR_)
            {
                i++;
                continue;
            }
        }
        if (isupper(input[i]))
        {
            if (!quotation_marks && 
                param != _PATH_ && 
                param != _NAME_ &&
                param != _USR_ &&
                param != _PWD_ &&
                param != _GRP_ &&
                param != _CONT_ &&
                param != _DEST_ &&
                param != _RUTA_)
                input[i] = tolower(input[i]);
        }

        char current_char[2] = {input[i], '\0'};

        if (step == _COMMAND_) // OBTENER TIPO COMANDO
        {
            if (input[i] != ' ')
            {
                strcat(auxiliar, current_char);
                i++;
                continue;
            }
            else
            {
                if (strcasecmp(auxiliar, "exit") == 0)
                    command = _EXIT_;
                else if (strcasecmp(auxiliar, "mkdisk") == 0)
                    command = _MKDISK_;
                else if (strcasecmp(auxiliar, "rmdisk") == 0)
                    command = _RMDISK_;
                else if (strcasecmp(auxiliar, "fdisk") == 0)
                    command = _FDISK_;
                else if (strcasecmp(auxiliar, "mount") == 0)
                    command = _MOUNT_;
                else if (strcasecmp(auxiliar, "unmount") == 0)
                    command = _UNMOUNT_;
                else if (strcasecmp(auxiliar, "rep") == 0)
                    command = _REP_;
                else if (strcasecmp(auxiliar, "exec") == 0)
                    command = _EXEC_;
                else if (strcasecmp(auxiliar, "pause") == 0)
                    command = _PAUSE_;
                else if (strcasecmp(auxiliar, "mkfs") == 0)
                    command = _MKFS_;
                else if (strcasecmp(auxiliar, "login") == 0)
                    command = _LOGIN_;
                else if (strcasecmp(auxiliar, "logout") == 0)
                    command = _LOGOUT_;
                else if (strcasecmp(auxiliar, "mkgrp") == 0)
                    command = _MKGRP_;
                else if (strcasecmp(auxiliar, "rmgrp") == 0)
                    command = _RMGRP_;
                else if (strcasecmp(auxiliar, "mkusr") == 0)
                    command = _MKUSR_;
                else if (strcasecmp(auxiliar, "rmusr") == 0)
                    command = _RMUSR_;
                else if (strcasecmp(auxiliar, "chmod") == 0)
                    command = _CHMOD_;
                else if (strcasecmp(auxiliar, "mkfile") == 0)
                    command = _MKFILE_;
                else if (strcasecmp(auxiliar, "cat") == 0)
                    command = _CAT_;
                else if (strcasecmp(auxiliar, "rem") == 0)
                    command = _REM_;
                else if (strcasecmp(auxiliar, "edit") == 0)
                    command = _EDIT_;
                else if (strcasecmp(auxiliar, "ren") == 0)
                    command = _REN_;
                else if (strcasecmp(auxiliar, "mkdir") == 0)
                    command = _MKDIR_;
                else if (strcasecmp(auxiliar, "cp") == 0)
                    command = _CP_;
                else if (strcasecmp(auxiliar, "mv") == 0)
                    command = _MV_;
                else if (strcasecmp(auxiliar, "find") == 0)
                    command = _FIND_;
                else if (strcasecmp(auxiliar, "chown") == 0)
                    command = _CHOWN_;
                else if (strcasecmp(auxiliar, "chgrp") == 0)
                    command = _CHGRP_;
                else if (strcasecmp(auxiliar, "recovery") == 0)
                    command = _RECOVERY_;
                else if (strcasecmp(auxiliar, "loss") == 0)
                    command = _LOSS_;
                else if (strcasecmp(auxiliar, "syncronice") == 0)
                    command = _SYNC_;
                else
                {
                    clearValues();
                    printf(ANSI_COLOR_RED "[e] Comando %s no reconocido\n" ANSI_COLOR_RESET, auxiliar);
                    return _ERROR_;
                }
                
                memset(auxiliar, 0, 300);
                step = _PARAM_;
                i++;
                continue;
            }
        }
        else if (step == _PARAM_) // OBTENER TIPO PARAMETRO
        {
            if (input[i] != '=' && input[i] != ' ')
            {
                strcat(auxiliar, current_char);
                i++;
                continue;
            }
            else
            {
                // printf(ANSI_COLOR_BLUE "[d] %d\n" ANSI_COLOR_RESET, strcasecmp(auxiliar, "ruta"));
                if (strcasecmp(auxiliar, "size") == 0)
                    param = _SIZE_;
                else if (strcasecmp(auxiliar, "path") == 0)
                    param = _PATH_;
                else if (strcasecmp(auxiliar, "unit") == 0)
                    param = _UNIT_;
                else if (strcasecmp(auxiliar, "name") == 0)
                    param = _NAME_;
                else if (strcasecmp(auxiliar, "type") == 0)
                    param = _TYPE_;
                else if (strcasecmp(auxiliar, "fit") == 0)
                    param = _FIT_;
                else if (strcasecmp(auxiliar, "delete") == 0)
                    param = _DELETE_;
                else if (strcasecmp(auxiliar, "add") == 0)
                    param = _ADD_;
                else if (strcasecmp(auxiliar, "id") == 0)
                    param = _ID_;
                else if (strcasecmp(auxiliar, "fs") == 0)
                    param = _FS_;
                else if (strcasecmp(auxiliar, "pwd") == 0)
                    param = _PWD_;
                else if (strcasecmp(auxiliar, "usr") == 0)
                    param = _USR_;
                else if (strcasecmp(auxiliar, "grp") == 0)
                    param = _GRP_;
                else if (strcasecmp(auxiliar, "ugo") == 0)
                    param = _UGO_;
                else if (auxiliar[0] == 'r' && strlen(auxiliar) == 1)
                    values.recursive = 1;
                else if (auxiliar[0] == 'p' && strlen(auxiliar) == 1)
                    values.recursive = 1;
                else if (strcasecmp(auxiliar, "cont") == 0)
                    param = _CONT_;
                else if (strcasecmp(auxiliar, "file") == 0)
                    param = _FILE_;
                else if (strcasecmp(auxiliar, "dest") == 0)
                    param = _DEST_;
                else if (strcasecmp(auxiliar, "ruta") == 0)
                    param = _RUTA_;
                else if (auxiliar[0] == ' ')
                    printf(ANSI_COLOR_RED "[e] Parámetro %s no reconocido\n" ANSI_COLOR_RESET, auxiliar);
                
                if (strcasecmp(auxiliar, "ruta") == 0) param = _RUTA_;
                if (strcasecmp(auxiliar, "file") == 0) param = _FILE_;
                if (strcasecmp(auxiliar, "cont") == 0) param = _CONT_;

                step = (strlen(auxiliar) == 1) ? _PARAM_ : _VALUE_;
                memset(auxiliar, 0, 300);
                i++;
                continue;
            }
        }
        else if (step == _VALUE_) // OBTENER VALOR PARAMETRO
        {
            if (input[i] == '"')
            {
                quotation_marks = !quotation_marks;
                i++;
                continue;
            }
            
            if (input[i] != ' ' && !quotation_marks)
            {
                strcat(auxiliar, current_char);
                i++;
                continue;
            }
            else
            {
                if (quotation_marks)
                {
                    strcat(auxiliar, current_char);
                    i++;
                    continue;
                }
                switch (param)
                {
                    case _SIZE_:
                        values.size = atoi(auxiliar);
                        break;
                    case _PATH_:
                        strcpy(values.path, auxiliar);
                        break;
                    case _UNIT_:
                        values.unit = auxiliar[0];
                        break;
                    case _NAME_:
                        strcpy(values.name, auxiliar);
                        break;
                    case _TYPE_:
                        values.type = auxiliar[0];
                        break;
                    case _FIT_:
                        values.fit = auxiliar[0];
                        break;
                    case _DELETE_:
                        strcpy(values.del, auxiliar);
                        break;
                    case _ADD_:
                        values.add = atoi(auxiliar);
                        break;
                    case _ID_:
                        strcpy(values.id, auxiliar);
                        break;
                    case _FS_:
                        values.fs = auxiliar[0];
                        break;
                    case _USR_:
                        strncpy(values.usr, auxiliar, 10);
                        break;
                    case _PWD_:
                        strncpy(values.pwd, auxiliar, 10);
                        break;
                    case _GRP_:
                        strncpy(values.grp, auxiliar, 10);
                        break;
                    case _UGO_:
                        strcpy(values.ugo, auxiliar);
                        break;
                    case _CONT_:
                        strcpy(values.cont, auxiliar);
                        break;
                    case _FILE_:
                        strcpy(values.file, auxiliar);
                        break;
                    case _DEST_:
                        strcpy(values.dest, auxiliar);
                        break;
                    case _RUTA_:
                        strcpy(values.ruta, auxiliar);
                        break;
                    default:
                        break;
                }
                memset(auxiliar, 0, 300);
                step = _PARAM_;
                param = _ERROR_;
                i++;
                if (comment)
                    break;
                else
                    continue;
            }
        }
    }

    switch (command)
    {
        case _EXIT_:
            command = _EXIT_;
            break;
        case _EXEC_:
            exec_exec();
            break;
        case _MKDISK_:
            exec_mkdisk();
            break;
        case _RMDISK_:
            exec_rmdisk();
            break;
        case _FDISK_:
            exec_fdisk();
            break;
        case _MOUNT_:
            exec_mount();
            break;
        case _UNMOUNT_:
            exec_unmount();
            break;
        case _REP_:
            exec_rep();
            break;
        case _PAUSE_:
        {
            char conf[999] = {0};
            fgets(conf, 999, stdin);
            break;
        }
        case _MKFS_:
            exec_mkfs();
            break;
        case _LOGIN_:
            exec_login();
            break;
        case _LOGOUT_:
            exec_logout();
            break;
        case _MKGRP_:
            exec_mkgroup();
            break;
        case _MKUSR_:
            exec_mkuser();
            break;
        case _RMGRP_:
            exec_rmgroup();
            break;
        case _RMUSR_:
            exec_rmuser();
            break;
        case _MKDIR_:
            exec_mkdir();
            break;
        case _MKFILE_:
            exec_mkfile();
            break;
        case _CAT_:
            exec_cat();
            break;
        case _CHGRP_:
            exec_chgrp();
            break;
        case _CHMOD_:
            exec_chmod();
            break;
        case _EDIT_:
            exec_edit();
            break;
        case _REN_:
            exec_ren();
            break;
        case _REM_:
            exec_rem();
            break;
        case _MV_:
            exec_mv();
            break;
        case _LOSS_:
            exec_loss();
            break;
        case _RECOVERY_:
            exec_recovery();
            break;
        case _SYNC_:
            exec_sync();
            break;
        default:
        {
            if (!comment) command = _ERROR_;
            else command = _EMPTY_;
            
            break;
        }
    }
    clearValues();
    return command;
}

/**
 * @brief Ejecuta una línea de comando recibida por el usuario
 * 
 */
void exec_exec()
{
    if (strlen(values.path) > 0)
    {
        char input[999] = {0};
        FILE * file;
        file = fopen(values.path, "r");

        if (file != NULL)
        {
            while (!feof(file))
            {
                command = -1;
                fgets(input, 999, file);
                sprintf(input, "%s\n", input);
                printf("[d] %s\n", input);
                if (input[0] != '#' || input[0] != '\n')
                    loadCommand(input);
            }            
        }
        else
            printf(ANSI_COLOR_RED "[e] No se puedo cargar el archivo: %s\n" ANSI_COLOR_RESET, values.path);   
    }
}

#endif
