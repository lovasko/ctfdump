#include <stdio.h>

#include "dump.h"

static void
dump_label(void* label, void* data)
{
	ctf_label_index index;
	char* name;

	(void) data;

	ctf_label_get_name(label, &name);
	ctf_label_get_index(label, &index);

	printf(" Name: %s\n", name);	
	printf("Index: %d\n", index);	
	printf("\n");
}

void
dump_labels(ctf_file file)
{
	ctf_count label_count;

	printf("-- Labels -------\n");

	ctf_file_get_label_count(file, &label_count);
	if (label_count == 0)
		printf("No labels.");
	else
		ctf_file_foreach_label(file, NULL, dump_label);
}

