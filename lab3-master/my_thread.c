#include "my_thread.h"

void * my_thread(void* a)
{
	struct _message *mes;
	mes = (struct message *)a;
 	printf("file sent!\n");
	write(mes->index, mes->file, mes->size);
	return;

}
