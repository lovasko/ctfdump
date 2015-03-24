#include <stdlib.h>
#include <stdio.h>

#include "data.h"

static void
dump_data_object(ctf_data_object data_object, void* data)
{
	ctf_type type;
	char* symbol_name;
	char* type_name;

	(void) data;

	ctf_data_object_get_name(data_object, &symbol_name);
	ctf_data_object_get_type(data_object, &type);
	ctf_type_to_string(type, &type_name);

	printf(" Name: %s\n", symbol_name);	
	printf(" Type: %s\n", type_name);	
	printf("\n");

	free(type_name);
}

void
dump_data_objects(ctf_file file)
{
	int rv;

	rv = ctf_file_foreach_data_object(file, NULL, dump_data_object);

	if (ctf_is_error(rv))
		fprintf(stderr, "ctfdump: ERROR: %s\n", ctf_get_error_string(rv));

	if (rv == CTF_EMPTY)
		printf("No data objects.");
}

