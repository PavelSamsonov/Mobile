#ifndef ___FILE_READER___
#define ___FILE_READER___

#include <stdio.h>
#include <stdlib.h>
//fileInfo
struct file_info
{
	char * content;
	int size;
};

struct file_info read_file(char * file_name);
#endif
