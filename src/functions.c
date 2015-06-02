#include <stdio.h>

#include "dump.h"

static void
dump_argument(void* argument, void* arg)
{
	ctf_type type;
	ctf_id id;

	(void) arg;

	ctf_argument_get_type(argument, &type);
	ctf_type_get_id(type, &id);

	printf("%d ", id);
}

static void
dump_function(void* function, void* arg)
{
	char* name;
	ctf_type return_type;
	ctf_id return_id;

	(void) arg;

	ctf_function_get_name(function, &name);
	ctf_function_get_return_type(function, &return_type);
	ctf_type_get_id(return_type, &return_id);

	printf("     Name: %s\n", name);
	printf("   Return: %d\n", return_id);
	printf("Arguments: ");
	ctf_function_foreach_argument(function, NULL, dump_argument);

	printf("\n\n");
}

void
dump_functions(ctf_file file)
{
	ctf_count function_count;

	ctf_file_get_function_count(file, &function_count);
	if (function_count == 0)
		printf("No functions.");
	else
		ctf_file_foreach_function(file, NULL, dump_function);
}

