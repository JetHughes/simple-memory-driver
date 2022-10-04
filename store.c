#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "fs.h"
#include "libmemdrv.c"

#define SHUFFLE 1
#define NOSHUFFLE 0

static int8_t free_list[MAX_BID];

void shuffle(int8_t *array, int n) {
    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int8_t t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

/* Function to check if a file exists
 * and if it does calculate its size
 * 
 * Params: filename - the name of the file to check
 * 
 * Return: An integer indicating the size of the file or (-1) for no file 
 */
int checkFile(char filename[])
{
    FILE* fp = fopen(filename, "r");  
    if (fp == NULL)
        return -1;    
    
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);

    fclose(fp);  
    return length;
}

/* Function to save a given file to the memdrv 
 * device using indexed allocation
 * 
 * Params: filename - The name of the file to save
 *         doShuffle - boolean indicating the use of random allocation.
 */
void store(char filename[], int doShuffle){    
    int size = checkFile(filename);
    if(size == -1){
        fprintf(stderr, "File Not Found!\n");
        exit(EXIT_FAILURE);
    } else if(size == 0){
        fprintf(stderr, "Empty File!\n");
        exit(EXIT_FAILURE);
    } else if(size > 4864){
        fprintf(stderr, "Truncated\n");
    }

    Inode inode;    
    int8_t indirect_block[BLOCK_SIZE];

    char buf[BLOCK_SIZE];
    int block_count, char_count = 0;
    int fp = open(filename, O_RDONLY);

    open_device();
    memset(indirect_block, 0, BLOCK_SIZE);
    memset(&inode, 0, sizeof(Inode));

    for (int i = 0; i < MAX_BID; i++)
        free_list[i] = i+1;    

    if(doShuffle == SHUFFLE)
        shuffle(free_list, MAX_BID);

    do {
        char_count = read(fp, buf, BLOCK_SIZE);
        inode.size += char_count;

        if(block_count < NDIRECT){
            inode.addrs[block_count] = free_list[block_count];
            write_block(free_list[block_count], buf);
        } else if(block_count == NDIRECT){
            inode.addrs[block_count] = free_list[block_count];
            indirect_block[0] = free_list[block_count+1];
            write_block(free_list[block_count+1], buf);
        } else {            
            indirect_block[block_count-NDIRECT] = free_list[block_count+1];
            write_block(free_list[block_count+1], buf);
        }

        block_count++;
    } while (char_count == 64 && block_count<MAX_BID);

    write_block(0, (char*) &inode);
    write_block(free_list[NDIRECT], indirect_block);

    printf("File stored, %d blocks created\n", block_count-1);
    close_device();
    return;
}

/* C program to save a file to the dev/memdrv 
 * block device using indexed allocation
 */
int main(int argc, char *argv[]){
    switch (argc){        
        case 2: // Args were just a file name, store without shuffling
            store(argv[1], NOSHUFFLE); 
            break;
        case 3: // 3 Args found, if 3rd arg is "-r" store using randomlyt ordered blocks
            if(argv[2][0] == '-' && argv[2][1] == 'r')
                store(argv[1], SHUFFLE);     
            break;

        default: // Invalid args
            printf("Usage: %s <file> [-r]\n", argv[0]);
    }
}