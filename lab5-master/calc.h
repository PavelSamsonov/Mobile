#include <linux/fs.h>

static int open_routine(struct inode *inode, struct file *file);
static int release_routine(struct inode *inode, struct file *file);
static ssize_t read_routine(struct file *filp, char *buffer, size_t length, loff_t * offset);
static ssize_t write_routine(struct file *filp, const char *buff, size_t len, loff_t * off);

#define MODULE_PREFIX "[CALC CDEV] "

MODULE_AUTHOR("Samsonov");
MODULE_DESCRIPTION("Calculator Driver");
MODULE_LICENSE("GPL");
