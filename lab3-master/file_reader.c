#include "file_reader.h"

struct file_info read_file(char * file_name) 
{
 	FILE *pFile;
 	int lSize;
 	size_t result;
 	struct file_info info;

 	pFile = fopen(file_name, "r");
 	if (pFile == NULL) {
		fputs("Opening file error", stderr);
		exit(1);
  	}

	fseek(pFile, 0, SEEK_END);
	info.size = ftell(pFile);
	rewind(pFile);

	info.content = (char*)malloc(sizeof(char)*info.size);
	if (info.content == NULL) {
		fputs("Memory error", stderr);
		exit(2);
	}

	result = fread(info.content, 1, info.size, pFile);
	if (result != info.size) {
		fputs("Reading error", stderr);
		exit(3);
	}
	fclose(pFile);
	return info;
}
