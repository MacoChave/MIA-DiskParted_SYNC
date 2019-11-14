/**
 * @file filename.h
 * @author Marco Chávez (macochave.github.io)
 * @brief Contiene los métodos necesarios para la gestión del nombrado de rutas
 * @version 0.1
 * @date 2019-08-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef FILENAME_H
#define FILENAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Obtiene el directorio de una ruta /Proyectos/Code/Fragmentos_C/main.c -> /Proyectos/Code/Fragmentos_C
 * @param path 
 * @return char* 
 */
char * getDirectoryFilename(char path[]) {
    int i = 0;
    char cmd[300] = {0};
    char * directory = (char *)calloc(300, sizeof(char));
    char currentFolder[300] = {0};

    while (path[i] != '\0')
    {
        char character[300] = {path[i], '\0'};
        if (path[i] != '/' && path[i] != '.')
            strcat(currentFolder, character);
        else if (path[i] == '/')
        {
            strcat(directory, currentFolder);
            memset(currentFolder, 0, 300);
            strcat(currentFolder, "/");
        }
        else break;

        i++;
    }

    return directory;
}

/**
 * @brief Obtiene el nombre de archivo de una ruta /Proyectos/Code/Fragmentos_C/main.c -> main
 * 
 */
char * getFileFilename(char path[]) {
    int i = 0;
    char cmd[300] = {0};
    char * file = (char *)calloc(300, sizeof(char));
    char currentFolder[300] = {0};
    
    while (path[i] != '\0')
    {
        char character[300] = {path[i], '\0'};
        if (path[i] != '/' && path[i] != '.')
            strcat(currentFolder, character);
        else if (path[i] == '/')
            memset(currentFolder, 0, 300);
        else if (path[i] == '.')
        {
            strcat(file, currentFolder);
            memset(currentFolder, 0, 300);
        }
        else break;

        i++;
    }

    return file;
}

/**
 * @brief Obtiene el tipo de archivo de una ruta /Proyectos/Code/Fragmentos_C/main.c -> c
 * 
 * @param path 
 * @return char* 
 */
char * getTypeFilename(char path[]) {
    int i = 0;
    char cmd[30] = {0};
    char * type = (char *)calloc(30, sizeof(char));
    char currentFolder[30] = {0};

    while (path[i] != '\0')
    {
        char character[30] = {path[i], '\0'};
        
        if (path[i] != '/' && path[i] != '.')
            strcat(currentFolder, character);
        else if (path[i] == '/')
            memset(currentFolder, 0, 30);
        else if (path[i] == '.')
            memset(currentFolder, 0, 30);

        i++;
    }

    strcat(type, currentFolder);

    return type;
}

#endif // FILENAME_H