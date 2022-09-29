/* fs.h - contains the on-disk inode structure */
#include <stdint.h>

#define NDIRECT 12

typedef struct fs_inode {
    short type;                 // File type
    short major;                // Major device number
    short minor;                // Minor device number
    short nlink;                // Number of links to inode in file system
    int size;                   // Size of file (bytes)
    int8_t addrs[NDIRECT + 1];  // Data block addresses
} Inode;
