#ifndef STRUCT_H
#define STRUCT_H

typedef struct Partition Partition;
typedef struct MBR MBR;
typedef struct EBR EBR;

typedef struct Mount Mount;
typedef struct PartMount PartMount;

typedef struct SuperBlock SuperBlock;
typedef struct Inode Inode;
typedef struct DirectoryBlock DirectoryBlock;
typedef struct ContentDirectory ContentDirectory;
typedef struct FileBlock FileBlock;
typedef struct PointerBlock PointerBlock;
typedef struct Journal Journal;

typedef struct Values Values;
typedef struct SpaceDisk SpaceDisk;

typedef struct Permission Permission;
typedef struct Session Session;


/**
 * DISCO VIRTUAL Y SUS PARTICIONES
 * */
struct Partition 
{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
};

struct MBR 
{
    int size;
    char mbr_creation[16];
    int mbr_disk_signature;
    char fit;
    Partition partitions[4];
};

struct EBR 
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char ebr_name[16];
};

/**
 * MONTAJE DE PARTICIONES
 * */
struct PartMount
{
    char mount_type;
    int mount_start;
    int mount_size;
    int mount_id;
    char mount_name[16];
};

struct Mount
{
    char path[300];
    char letter;
    PartMount parts_mount[30];
};

/**
 * ESTRUCTURAS PARA EL SISTEMA DE ARCHIVOS
 * */
struct SuperBlock
{
    int filesystem;
    int inodes_count;
    int blocks_count;
    int free_blocks;
    int free_inodes;
    char mounted_date[16];
    char unmounted_date[16];
    int mounting_count;
    int magic; //0xEF53
    int inode_size;
    int block_size;
    int first_inode;
    int first_block;
    int bm_inode_start;
    int bm_block_start;
    int inode_start;
    int block_start;
};

/* 80 */
struct Inode
{
    int uid;
    int gid;
    int size;
    char last_date[16];
    char create_date[16];
    char modified_date[16];
    int block[15];
    char type;
    int permission;
};

struct ContentDirectory
{
    char name[12];
    int inode;
};

/* 64 */
struct DirectoryBlock
{
    ContentDirectory content[4];
};

/* 64 */
struct FileBlock
{
    char content[64];
};

/* 64 */
struct PointerBlock
{
    int pointers[16];
};

/* 242 */
struct Journal
{
    char str_1[200];
    char str_2[200];
    char str_3[20];
    char date[16];
    char ugo[3];
    int recursive;
    int owner;
    int command;
    int size;
};

/**
 * VALORES DE COMANDOS
 * */
struct Values
{
    char path[300];
    char del[300];
    char name[300];
    char id[300];
    char cont[300];
    char file[300];
    char dest[300];
    char ruta[300];
    char usr[10];
    char pwd[10];
    char grp[10];
    char ugo[3];
    char fit;
    char unit;
    char type;
    char fs;
    int size;
    int add;
    int recursive;
};

struct SpaceDisk
{
    int start;
    int space;
    char type;
    int next;
    int prev;
};

struct Permission
{
    int id;
    char type;
    char group[10];
    char name[10];
    char pass[10];
};

struct Session
{
    int id_user;
    int id_group;
    int part_start;
    int part_size;
    char path[300];
    SuperBlock * sb;
};

#endif