#include <stdlib.h>
#include <stdio.h>

#include "dump.h"

static void
dump_data_object(void* data_object, void* data)
{
	ctf_type type;
	ctf_id id;
	char* symbol_name;

	(void) data;

	ctf_data_object_get_name(data_object, &symbol_name);
	ctf_data_object_get_type(data_object, &type);
	ctf_type_get_id(type, &id);

	printf(" Name: %s\n", symbol_name);
	printf("   ID: %d\n", id);
	printf("\n");

}

void
dump_data_objects(ctf_file file)
{
	ctf_count data_object_count;

	ctf_file_get_data_object_count(file, &data_object_count);
	if (data_object_count == 0)
		printf("No data objects.");
	else
		ctf_file_foreach_data_object(file, NULL, dump_data_object);
}

