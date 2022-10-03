#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fs.h"
#include "libmemdrv.c"
#include "findsize.c"

static int8_t free_list[MAX_BID];

void printArgs(int argc, char *argv[]){
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argc %d: %s\n", i, argv[i]);
    }
}

void shuffle(int8_t *array, int n) {
    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int8_t t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void store(char filename[]){    
    printf("Storing %s\n", filename);
    
    // Open File
    FILE* fp = fopen(filename, "r");  
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    // Split into blocks
    char blocks[MAX_BID][BLOCK_SIZE];
    int block_count = 0;
    char c = fgetc(fp);
    while (c != EOF && block_count < MAX_BID)
    {
        for (int i = 0; i < BLOCK_SIZE && c != EOF; i++)
        {
            blocks[block_count][i] = c;
            c = fgetc(fp);  
        }
        block_count ++;
        lseek(fp, BLOCK_SIZE, SEEK_CUR);
    }

    printf("Reached end of file, %d blocks created\n", block_count);

    // Write to Memory

    // Create Inode
    Inode inode;
    inode.size = block_count * BLOCK_SIZE;

    for (int i = 0; i < block_count && i <= NDIRECT; i++)
    {
        inode.addrs[i] = free_list[i]+1;
    }

    int8_t indirect_block[BLOCK_SIZE];  
    if(block_count > NDIRECT){
        for (int i = 0; i < MAX_BID-NDIRECT; i++)
        {
            if(i<block_count-NDIRECT){
                indirect_block[i] = free_list[i+14]+1;
            } else {
                indirect_block[i] = 0;
            }
        }
    }

    open_device();
    // Write inode
    printf("Write Inode");
    write_block(0, (char*) &inode);

    printf("\nFree List: ");
    for (int i = 0; i < MAX_BID; i++)
    {
        printf("%d, ", free_list[i]);
    }    

    // write direct blocks    
    for (int i = 0; i <= NDIRECT && i < block_count; i++)
    {
        write_block(free_list[i]+1, blocks[i]);        
    }

    // write indirect blocks
    if(block_count > NDIRECT){
        write_block(free_list[NDIRECT]+1, indirect_block);
        for (int i = NDIRECT; i <= block_count; i++)
        {
            write_block(free_list[i+1]+1, blocks[i]);
        }        
    }

    close_device();
    return;
}

int main(int argc, char *argv[]){
    if(argc <2){
        printf("Usage: %s <file> [OPTION]\n", argv[0]);
        exit(0);
    }

    // printArgs(argc, argv);

    // init free_list
    for (int i = 0; i < MAX_BID; i++)
    {
        free_list[i] = i;
    }
    if(argc == 3){
        if(argv[2][1] == 'r' && argv[2][0] == '-'){
            printf("shuffle");
            shuffle(free_list, MAX_BID);  
        }
    }    

    // check size
    int size = findSize(argv[1]);
    printf("\nsize of %s: %d\n", argv[1], size);
    if(size > 4864){
        fprintf(stderr, "truncated\n");
    }

    // Store file
    store(argv[1]);
}