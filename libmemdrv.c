/* libmemdrv.c - interface for interacting with the memdrv module */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "libmemdrv.h"

static int memdrv_fd;

void open_device() {
    int fd;
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open_device");
        exit(0);
    }
    memdrv_fd = fd;
}

void read_block(int block_id, char *buf) {
    int res;
    res = lseek(memdrv_fd, block_id * BLOCK_SIZE, SEEK_SET);
    if (res < 0) {
        perror("read_block");
        exit(0);
    }
    res = read(memdrv_fd, buf, BLOCK_SIZE);
    if (res < BLOCK_SIZE) {
        perror("read_block");
        exit(0);
    }
}

void write_block(int block_id, char *buf) {
    int res;
    res = lseek(memdrv_fd, block_id * BLOCK_SIZE, SEEK_SET);
    if (res < 0) {
        perror("write_block");
        exit(0);
    }
    res = write(memdrv_fd, buf, BLOCK_SIZE);
    if (res < BLOCK_SIZE) {
        perror("write_block");
        exit(0);
    }
}

void close_device() {
    close(memdrv_fd);
}
