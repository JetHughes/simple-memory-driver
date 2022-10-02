#include "libmemdrv.h"
#include <stdio.h>
#include <stdlib.h>
#include "findsize.c"

// for debugging
void printArgs(int argc, char *argv[]){
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argc %d: %s\n", i, argv[i]);
    }
}

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
    printf("size of %s: %d\n", argv[1], size); //for debugging
    
    if(size > 4864){
        fprintf(stderr, "truncated\n");
    }

}