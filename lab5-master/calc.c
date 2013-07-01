#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "calc.h"

/* Char devices files names. */
#define FILENAME_FIRST "calc_first"
#define FILENAME_SECOND "calc_second"
#define FILENAME_ACTION "calc_action"
#define FILENAME_RESULT "calc_result"

#define STR_OUT "%s"

/* Char devices maximum file size. */
#define MAX_FILENAME 16
#define FILES_COUNT 4
#define MESSAGE_LEN 32

typedef struct file_operations FILE_OPERATIONS, *PFILE_OPERATIONS;
typedef struct cdev CDEV, *PCDEV;

typedef struct class CLASS, *PCLASS;
typedef PCLASS* PCLASS_ARRAY;

typedef struct inode INODE, *PINODE;
typedef struct file  FILE , *PFILE;

/* Char devices files names. */
static char names[][MAX_FILENAME] = {
        FILENAME_FIRST,
        FILENAME_SECOND,
        FILENAME_ACTION,
        FILENAME_RESULT
};

/* Device opened counter. */
static int device_opened = 0;

/* Char devices files buffers. */
static char** devices_buffer;

/* User message. */
static char message[MESSAGE_LEN];

/* Device operations struct. */
static FILE_OPERATIONS fops = {
        .owner = THIS_MODULE,
        .read = read_routine,
        .write = write_routine,
        .open = open_routine,
        .release = release_routine
};

/* Devices numbers. */
static dev_t numbers[FILES_COUNT];

/* Global var for the character device struct */
static PCDEV pDev;

/* Global var for the device class */
static PCLASS_ARRAY classes;

static int open_routine(PINODE inode, PFILE file)
{
        if (device_opened)
                return -EBUSY;

        device_opened++;
        try_module_get(THIS_MODULE);

        return 0;
}

static int release_routine(PINODE inode, PFILE file)
{
        device_opened--;

        module_put(THIS_MODULE);

        return 0;
}

static ssize_t read_routine(PFILE filp, char *buffer, size_t length, loff_t * offset)
{
        static int fin = 0;//static
        long a, b, result = 0;
        char name[MESSAGE_LEN], op = 0, *end;
        int written = 0;
        int i = 0;

        if (fin) {
                fin = 0;
                return 0;
        }

        strcpy(name, filp->f_dentry->d_name.name);

        if (strcmp(name, FILENAME_FIRST) == 0) {
                sprintf(message, STR_OUT, devices_buffer[0]);
        } else if (strcmp(name, FILENAME_SECOND) == 0) {
                sprintf(message, STR_OUT, devices_buffer[1]);
        } else if (strcmp(name, FILENAME_ACTION) == 0) {
                sprintf(message, STR_OUT, devices_buffer[2]);
        } else {
                a = simple_strtol(devices_buffer[0], &end, 10);
                if (a == 0 && (*end == devices_buffer[0][0])) {
                        sprintf(message, STR_OUT, "ERR_FIRST");
                } else {
                        b = simple_strtol(devices_buffer[1], &end, 10);
                        if (b == 0 && (*end == devices_buffer[1][0])) {
                                sprintf(message, STR_OUT, "ERR_SECOND");
                        } else {
                                op = devices_buffer[2][0];
                                switch (op) {
                                        case '+': result = a + b; break;
                                        case '-': result = a - b; break;
                                        case '*': result = a * b; break;
                                        case '/':
                                                if (b == 0) {
                                                        sprintf(message, STR_OUT,
                                                                "ERR_DIV_BY_ZERO");
                                                        written = 1;
                                                        break;
                                                }
                                                result = a / b;
                                                break;
                                        default:
                                                sprintf(message, STR_OUT, "ERR_ACTION");
                                                written = 1;
                                                break;
                                }
                        }
                        if (!written) {
                                sprintf(message, "%ld",result);
                        }
                }
        }

        for(i = 0; i < length && message[i] && message[i] != '\n'; i++) {
                put_user(message[i], buffer + i);
        }

        	put_user('\n', buffer + (i++));

        fin = 1;

        return i;
}

static ssize_t write_routine(PFILE filp, const char *buff, size_t len, loff_t * off)
{
        static int fin = 0;
        char name[MESSAGE_LEN];
        int buf_size = 0;
        int index = 0;

        if (fin) {
                fin = 0;
                return 0;
        }

        strcpy(name, filp->f_dentry->d_name.name);

        if (strcmp(name, FILENAME_FIRST) == 0) {
                index = 0;
        } else if (strcmp(name, FILENAME_SECOND) == 0) {
                index = 1;
        } else if (strcmp(name, FILENAME_ACTION) == 0) {
                index = 2;
        } else {
                index = 3;
        }

        if (len >= MAX_FILENAME) {
                buf_size = MAX_FILENAME - 1;
        } else {
                buf_size = len;
        }

        if (index != 3) {
                if (copy_from_user(devices_buffer[index], buff, buf_size)) {
                                printk(KERN_ERR MODULE_PREFIX "Failed to write to /dev/%s\n", names[index]);
                                return -EFAULT;
                }
                devices_buffer[index][buf_size + 1] = '\0';
        }

        fin = 1;

        return buf_size;
}

/* Module init function */
static int __init calc_init(void)
{
        int i = 0;

        printk(KERN_INFO MODULE_PREFIX "Calc driver was loaded.\n");

        classes = (PCLASS_ARRAY) kmalloc(sizeof(PCLASS) * FILES_COUNT, GFP_KERNEL);
        pDev = (PCDEV) kmalloc(sizeof(CDEV) * FILES_COUNT, GFP_KERNEL);
        devices_buffer = (char**) kmalloc(sizeof(char*) * FILES_COUNT, GFP_KERNEL);

        for (i = 0; i < FILES_COUNT; i++) {
                devices_buffer[i] = (char*) kmalloc(sizeof(char) * MAX_FILENAME, GFP_KERNEL);
                devices_buffer[i][0] = '\0';
        }

        for (i = 0; i < FILES_COUNT; i++) {
                if (alloc_chrdev_region(&numbers[i], 0, 1, names[i]) < 0) {
                        return -1;
                }

                if ((classes[i] = class_create(THIS_MODULE, names[i])) == NULL) {
                        unregister_chrdev_region(numbers[i], 1);
                        return -1;
                }

                if (device_create(classes[i], NULL, numbers[i], NULL, names[i]) == NULL) {
                        class_destroy(classes[i]);
                        unregister_chrdev_region(numbers[i], 1);
                        return -1;
                }

                cdev_init(&pDev[i], &fops);

                if (cdev_add(&pDev[i], numbers[i], 1) == -1) {
                        device_destroy(classes[i], numbers[i]);
                        class_destroy(classes[i]);
                        unregister_chrdev_region(numbers[i], 1);
                        return -1;
                }
        }

        printk(KERN_INFO MODULE_PREFIX "Calc driver devices were created.\n");
        return 0;
}

/* Module exit function */
static void __exit calc_exit(void)
{
        int i;

        for (i = 0; i < FILES_COUNT; i++) {
                cdev_del(&pDev[i]);
                device_destroy(classes[i], numbers[i]);
                class_destroy(classes[i]);
                unregister_chrdev_region(numbers[i], 1);
                kfree(devices_buffer[i]);
        }

        kfree(devices_buffer);

        printk(KERN_INFO MODULE_PREFIX "Calc driver devices were removed.\n");
        printk(KERN_INFO MODULE_PREFIX "Calc driver was unloaded.\n");
}

module_init(calc_init); /* Register module entry point */
module_exit(calc_exit); /* Register module cleaning up */
