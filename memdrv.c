/*
 * File: memdrv.c
 * Date: 12/09/2022
 * Author: Zhiyi Huang
 * Version: 0.1
 */

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/aio.h>
#include <linux/uaccess.h>

#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhiyi Huang");
MODULE_DESCRIPTION("A simple memory device driver module");


/* The parameter used as a major number of the device. This variable is
 * readable in /sys/module/memdrv/parameters.*/
int major = 0;
module_param(major, int, S_IRUGO);
MODULE_PARM_DESC(major, "device major number");

// 256 4KB-pages
#define MAX_DSIZE 0x1000 * 0x100
struct my_dev {
    char data[MAX_DSIZE + 1]; /* data storage */
    size_t size;              /* 32-bit will suffice */
    struct semaphore sem;     /* Mutual exclusion */
    struct cdev cdev;         /* kernel device structure */
    struct class *class;      /* create a class for the device*/
    struct device *device;    /* create the device in the system */
} *temp_dev;

/*
 * This function will be called when the system call open() is called
 * in the user space. It is expected to check if the process has the
 * permission to access; suspend the calling process if the device is
 * busy or used by another process; otherwise record the id of the
 * calling process so that the device is owned by the process;
 * initialise or reset the device.  For such simple device, we have
 * done neither of the above.
 *
 * Params: inode - the inode of the device file /dev/memdrv
 *         filp - the file descriptor created when /dev/memdrv is opened.
 * Return: an integer indicating success (0) or failure (<0)
 */
int temp_open(struct inode *inode, struct file *filp) {
    return 0;
}

/*
 * This function will be called when the system call close() is called
 * in the user space. It is expected to undo everything of
 * temp_open(). For example, release the device by removing the id of
 * the process; check if there is any waiting process to wake up; undo
 * anything done at temp_open(), e.g. free memory.
 *
 * Params: inode - the inode of the device file /dev/memdrv
 *         filp - the file descriptor created when /dev/memdrv is opened.
 * Return: an integer indicating success (0) or failure (<0)
 */
int temp_release(struct inode *inode, struct file *filp) {
    return 0;
}

/* This function will be called when the system call read()
 * is called in the user space. It is expected to find the
 * location of data; ask the controller to copy data to
 * the kernel kbuf; then copy data from kbuf to user space.
 * The current function only find the data in kernel and copy
 * to user space after sanity check.
 *
 * Params: filp - the file descriptor created when /dev/memdrv is opened.
 *         buf - pointer to user space buffer
 *         count - the number of bytes to be read
 *         f_pos - the current location of the device file
 * Return: an integer indicating success (0) or failure (<0)
 */
ssize_t temp_read(struct file *filp, char __user *buf, size_t count,
                  loff_t *f_pos) {
    int rv = 0;

    if (down_interruptible(&temp_dev->sem))
        return -ERESTARTSYS;
    if (*f_pos > MAX_DSIZE)
        goto wrap_up;
    if (*f_pos + count > MAX_DSIZE)
        count = MAX_DSIZE - *f_pos;
    if (copy_to_user(buf, temp_dev->data + *f_pos, count)) {
        rv = -EFAULT;
        goto wrap_up;
    }
    up(&temp_dev->sem);
    *f_pos += count;
    return count;

wrap_up:
    up(&temp_dev->sem);
    return rv;
}

/*
 * This function will be called when the system call write() is called
 * in the user space. It is expected to copy data from user space buf
 * to kernel buffer kbuf. Then find the location of data; ask the
 * controller to copy data from the kernel kbuf to the device.  The
 * current function only copy the data in user space to the kernel
 * buffer in the right location after sanity check.
 *
 * Params: filp - the file descriptor created when /dev/memdrv is opened.
 *         buf - pointer to user space buffer
 *         count - the number of bytes to be read
 *         f_pos - the current location of the device file
 * Return: an integer indicating success (0) or failure (<0)
 */
ssize_t temp_write(struct file *filp, const char __user *buf, size_t count,
                   loff_t *f_pos) {
    int count1 = count, rv = count;

    if (down_interruptible(&temp_dev->sem))
        return -ERESTARTSYS;

    if (*f_pos > MAX_DSIZE)
        goto wrap_up;
    if (*f_pos + count > MAX_DSIZE)
        count1 = MAX_DSIZE - *f_pos;

    if (copy_from_user(temp_dev->data + *f_pos, buf, count1)) {
        rv = -EFAULT;
        goto wrap_up;
    }
    up(&temp_dev->sem);
    *f_pos += count1;
    return count;

wrap_up:
    up(&temp_dev->sem);
    return rv;
}

/*
 * This function will be called when the system call ioctl() is called
 * in the user space. It is expected to take an ioctl command and
 * execute it. There is no command implemented in the current
 * function.
 *
 * Params: filp - the file descriptor created when /dev/memdrv is opened.
 *         cmd - command of ioctl() as an integer
 *         arg - argument passed with the command
 * Return: an integer indicating success (0) or failure (<0)
 */
long temp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    return 0;
}

/*
 * This function will be called when the system call lseek() is called
 * in the user space. It seeks to the location required by the system
 * call.
 *
 * Params: filp - the file descriptor created when /dev/memdrv is opened.
 *         off - offset to seek
 *         whence - origin of the location to seek
 *
 * Return: an integer indicating success (0) or failure (<0)
 */
loff_t temp_llseek(struct file *filp, loff_t off, int whence) {
    long newpos;

    switch (whence) {
        case SEEK_SET:
            newpos = off;
            break;

        case SEEK_CUR:
            newpos = filp->f_pos + off;
            break;

        case SEEK_END:
            newpos = temp_dev->size + off;
            break;

        default: /* can't happen */
            return -EINVAL;
    }
    if (newpos < 0 || newpos >= MAX_DSIZE) return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}

/*
 * File operation structure initialised to the functions defined in
 * this module/driver. It is used to register the device to the kernel
 * so that the system calls are associated with the functions in this
 * module.
 */
struct file_operations temp_fops = {
    .owner = THIS_MODULE,
    .llseek = temp_llseek,
    .read = temp_read,
    .write = temp_write,
    .unlocked_ioctl = temp_ioctl,
    .open = temp_open,
    .release = temp_release,
};

/*
 * Function called when the module is loaded.  It initialises the
 * module and registers the memdrv device to the kernel.
 */
int __init temp_init_module(void) {
    int rv;
    dev_t devno = MKDEV(major, 0);

    if (major) {
        rv = register_chrdev_region(devno, 1, "memdrv");
        if (rv < 0) {
            printk(KERN_WARNING "Can't use the major number %d;"
                                "try automatic allocation...\n",
                   major);
            rv = alloc_chrdev_region(&devno, 0, 1, "memdrv");
            major = MAJOR(devno);
        }
    } else {
        rv = alloc_chrdev_region(&devno, 0, 1, "memdrv");
        major = MAJOR(devno);
    }

    if (rv < 0) return rv;

    temp_dev = kmalloc(sizeof(struct my_dev), GFP_KERNEL);
    if (temp_dev == NULL) {
        rv = -ENOMEM;
        unregister_chrdev_region(devno, 1);
        return rv;
    }

    memset(temp_dev, 0, sizeof(struct my_dev));
    cdev_init(&temp_dev->cdev, &temp_fops);
    temp_dev->cdev.owner = THIS_MODULE;
    temp_dev->size = MAX_DSIZE;
    sema_init(&temp_dev->sem, 1);
    rv = cdev_add(&temp_dev->cdev, devno, 1);
    if (rv) printk(KERN_WARNING "Error %d adding device temp", rv);

    temp_dev->class = class_create(THIS_MODULE, "memdrv");
    if (IS_ERR(temp_dev->class)) {
        cdev_del(&temp_dev->cdev);
        unregister_chrdev_region(devno, 1);
        printk(KERN_WARNING "%s: can't create udev class\n", "memdrv");
        rv = -ENOMEM;
        return rv;
    }

    temp_dev->device = device_create(temp_dev->class, NULL,
                                     MKDEV(major, 0), "%s", "memdrv");
    if (IS_ERR(temp_dev->device)) {
        class_destroy(temp_dev->class);
        cdev_del(&temp_dev->cdev);
        unregister_chrdev_region(devno, 1);
        printk(KERN_WARNING "%s: can't create udev device\n", "memdrv");
        rv = -ENOMEM;
        return rv;
    }

    printk(KERN_WARNING "memdrv device MAJOR is %d, dev addr: %lx\n", major,
           (unsigned long)temp_dev);
    return 0;
}

/*
 * Function called when the module is removed.  It de-registers the
 * device and frees resources.
 */
void __exit temp_exit_module(void) {
    device_destroy(temp_dev->class, MKDEV(major, 0));
    class_destroy(temp_dev->class);
    cdev_del(&temp_dev->cdev);
    kfree(temp_dev);
    unregister_chrdev_region(MKDEV(major, 0), 1);
    printk(KERN_WARNING "Good bye from memdrv module\n");
}

module_init(temp_init_module);
module_exit(temp_exit_module);
