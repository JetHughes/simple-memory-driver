#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libmemdrv.h"
#include "fs.h"

char buf[BLOCK_SIZE];
char oneline[BLOCK_SIZE];

/* 
 * Prints the contents of /dev/memdrv to stdout in a simple format.
 * Block 0 is the Inode.
 * The indirect address block is printed as numbers.
 * All other blocks are printed as characters on a single line.
 */
int main() {
    Inode *inode = malloc(BLOCK_SIZE);
    
    open_device();
    read_block(0, (char *)inode);
    printf("Inode: %d %d %d %d size: %d addrs: ", inode->type, inode->major,
        inode->minor, inode->nlink, inode->size);
    for (int i = 0; i <= NDIRECT; i++) {
        printf("%d ", inode->addrs[i]);
    }
    printf("\n");
    for (int i = 1; i < MAX_BID; i++) {
        read_block(i, buf);
        if (i == inode->addrs[NDIRECT]) {
            printf("Indirect: ");
            for (int j = 0; j < BLOCK_SIZE; j++) {
                printf("%d ", buf[j]);
            }
            printf("\n");
        } else {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (buf[j] == '\n') {
                    oneline[j] = ' ';
                } else {
                    oneline[j] = buf[j];
                }
            }
            printf("Block %d: ", i);
            fflush(stdout);
            int res = write(1, oneline, BLOCK_SIZE);
            if (res < 0) {
                perror("oneline");
                exit(1);
            }
            printf("\n");
        }
    }
    close_device();
    return EXIT_SUCCESS;
}
