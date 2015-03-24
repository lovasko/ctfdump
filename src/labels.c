#include <stdio.h>

#include "labels.h"

static void
dump_label(ctf_label label, void* data)
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
	int rv;

	rv = ctf_file_foreach_label(file, NULL, dump_label);

	if (ctf_is_error(rv))
		fprintf(stderr, "ctfdump: ERROR: %s", ctf_get_error_string(rv));

	if (rv == CTF_EMPTY)
		printf("No labels.\n");
}

