#include "calc.h"

char names[][PROCFS_MAX_SIZE] = {PROCFS_FIRST, PROCFS_SECOND, PROCFS_OPERAND, PROCFS_RESULT};
char** procfs_buffer;
struct proc_dir_entry** proc_entries;

#define _CALC_C_INC_
#include "calc.c"


//load module func
int __init module_load(void)
{
    int i;

	printk(KERN_INFO MODULE_PREFIX "Module is loaded.\n");

	procfs_buffer = (char**) kmalloc(sizeof(char*) * FILE_COUNT, GFP_KERNEL);
	for (i = 0; i < FILE_COUNT; i++) {
		procfs_buffer[i] = (char*) kmalloc(sizeof(char) * PROCFS_MAX_SIZE, GFP_KERNEL);
		procfs_buffer[i][0] = '\0';
	}

	proc_entries = (struct proc_dir_entry**) kmalloc(
		sizeof(struct proc_dir_entry *) * FILE_COUNT, GFP_KERNEL);

	for (i = 0; i < FILE_COUNT; i++) {
		proc_entries[i] = create_proc_entry(names[i], ACCESS_TOKEN, NULL);
		if (!proc_entries[i]) {
			remove_proc_entry(names[i], NULL);
			printk(KERN_ERR MODULE_PREFIX "Failed to create /proc/%s", names[i]);
			return 1;
		}
		proc_entries[i]->read_proc  = proc_read;
		proc_entries[i]->write_proc = proc_write;
		proc_entries[i]->data	   = (void*)(i+1);
	}

	printk(KERN_INFO MODULE_PREFIX "proc entries created!\n");
	return 0;
}

void __exit module_unload(void)
{	
	int i;

	for (i = 0; i < FILE_COUNT; i++) {
		remove_proc_entry(names[i], NULL);
		kfree(procfs_buffer[i]);
	}

	kfree(proc_entries);
	kfree(procfs_buffer);

	printk(KERN_INFO MODULE_PREFIX "proc entries removed.\n");
	printk(KERN_INFO MODULE_PREFIX "Module has been unload!");
}

module_init(module_load);
module_exit(module_unload);
