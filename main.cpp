#include <QCoreApplication>
#include <stdio.h>
#include <string.h>
#include "var/globals.h"
#include "interpreter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    initSession();
    initDisksMount();
    initPermissionlist();
    clearValues();
    printf("---> Disk Parted <---\n");
    char input[999] = "";
    int exit = 0;
    while (exit != _EXIT_)
    {
        fgets(input, 999, stdin);
        exit = loadCommand(input);
    }

    return 0;
}
