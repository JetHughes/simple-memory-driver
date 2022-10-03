#include "libmemdrv.h"

#include <stdio.h>

#include <stdlib.h>

#include "fs.h"

#include <sys/stat.h>

#include <stdbool.h>

#include <unistd.h>

#include <fcntl.h>

char buf[BLOCK_SIZE];
char oneline[BLOCK_SIZE];



//SOURCE: https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
//from user 

// This method checks if a file exists.  Returns true if it does

bool file_exists(char * filename) {
  struct stat buffer;
  return (stat(filename, & buffer) == 0);
}

//This method takes a block number, block size, file descriptor and a buffer
//It will read the data from the block number into the buffer
//and then write that to the file descriptor
int block_writer(int block_number, int block_size, int file_descriptor, char buf[]){
    if(block_number == 0){
        //printf("UH OH: block number = 0 \n");
        exit(1);
    }
    read_block(block_number, buf);
    fflush(stdout);
    
    int res = write(file_descriptor, buf, block_size);
    if (res < 0) {
        perror("oneline");
        exit(1);
    }
    //printf("wrote block %d", block_number);
    return(0);
}

//This method retrieves the data specified my the inode
//name of new file (if needed)
// create is a boolean. 1 for default, 0 for creating a newfile
int retrieve(char * name, int create) {
  int filedesc;
  //check if it exists
  if (file_exists(name)) {
    char overwrite;
    //Checks if user wants to overwrite
    printf("%s already exists.  Would you like to overwrite (y/n)?\n", name);
    scanf("%s", & overwrite);
    printf("You entered %c\n", overwrite);
    if (overwrite == 'n') {
      printf("Exiting program");
      exit(1);
    }
  }

  open_device();
  Inode * inode = malloc(BLOCK_SIZE);
  read_block(0, (char * ) inode);
  //File descriptor changes to either a file or the standard output
  if (create == 0) {
    filedesc = open(name, O_CREAT | O_WRONLY, 0644);
  } else {
    filedesc = 1;
  }
  //leftovers is an integer that describes how many bytes of the last block are used 
  int leftovers = (inode -> size) % 64;
  //numblocks is the number of used blocks
  int numblocks = (inode -> size) / 64;
  //If there are leftovers, then numblocks needs to be incremented
  if (leftovers > 0) {
    numblocks = numblocks + 1;
  }
  //Loop over the 13 addresses stored in the inode
  for (int i = 0; i < 13; i++) {
    //if the address is 0 then we are at the end of the file
    if(inode -> addrs[i] == 0){
        exit(1);
    } else {
            //Checks if it is the indirect index
            if(i == 12){
                
                read_block(inode -> addrs[12], buf);
                //copies to another buffer, so buf can still be used for block_writer
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    oneline[j] = buf[j];
                }
                //iterate over the addresses stored in the indirect indexes
                for(int j = 0; j < BLOCK_SIZE; j++){
                    //Checks to see if current address in the indirect addresses is the last one
                    if(oneline[j + 1] == 0){
                        //Changes the file size read from the block
                        if(leftovers == 0){
                            block_writer(oneline[j], BLOCK_SIZE, filedesc, buf);
                        } else {
                            block_writer(oneline[j], leftovers, filedesc, buf);
                        }
                    } else {
                        block_writer(oneline[j], BLOCK_SIZE, filedesc, buf);
                    }
                }
            //checks if the current address is the last one 
            } else if(inode -> addrs[i + 1] == 0){
                //Changes the file size read from the block
                if(leftovers != 0){
                    block_writer(inode -> addrs[i], leftovers, filedesc, buf);
                } else {
                    block_writer(inode -> addrs[i], BLOCK_SIZE, filedesc, buf);
                }
            } else {
                block_writer(inode -> addrs[i], BLOCK_SIZE, filedesc, buf);
            }
    }
  }
  printf("\n");
  exit(1);
}

int main(int argc, char * argv[]) {
  //inappropriate arguments  
  if (argc > 2) {
    printf("Usage: %s [FILE_OUTPUT_NAME]\n", argv[0]);
    exit(0);
  }
  //Case where writing to a file
  if (argc == 2) {
    retrieve(argv[1], 0);
  }
  //Case where writing to standard output
  if (argc == 1) {
    retrieve(NULL, 1);
  }
  return (0);
}