#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fs.h"
#include "libmemdrv.c"
#include "findsize.c"

// for debugging
void printArgs(int argc, char *argv[]){
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argc %d: %s\n", i, argv[i]);
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

    printf("\nReached end of file, %d blocks created\n", block_count);

    // Write to Memory
    // Inode inode;
    // inode.size = block_count * BLOCK_SIZE;
    // inode.addrs
    open_device();
    for (int i = 0; i < block_count; i++)
    {
        write_block(i+1, blocks[i]);        
    } 
    return;
}

    // for (int i = 0; i < block_count; i++)
    // {
    //     printf("block %d: ", i);
    //     for (int j = 0; j < BLOCK_SIZE; j++)
    //     {
    //         printf("%c", blocks[i][j]);
    //     }
    //     printf("\n");
    // }
int main(int argc, char *argv[]){
    if(argc <2){
        printf("Usage: %s <file> [OPTION]\n", argv[0]);
        exit(0);
    }
    printArgs(argc, argv); //for debugging
    if(argc == 3){
        if(argv[2] == "-r"){
            //set random
        }
    }    
    int size = findSize(argv[1]);
    printf("\nsize of %s: %d\n", argv[1], size); //for debugging
    
    if(size > 4864){
        fprintf(stderr, "truncated\n");
    }

    store(argv[1]);
}