#include <stdio.h>

#include "dump.h"

static void
dump_argument(void* argument, void* arg)
{
	ctf_type argument_type;
	char* argument_type_name;

	(void) arg;

	ctf_argument_get_type(argument, &argument_type);
	ctf_type_to_string(argument_type, &argument_type_name);

	printf("     %s\n", argument_type_name);
	free(argument_type_name);
}

static void
dump_function(void* function, void* arg)
{
	char* name;
	ctf_type return_type;
	char* return_type_name;

	(void) arg;

	ctf_function_get_name(function, &name);
	ctf_function_get_return_type(function, &return_type);
	ctf_type_to_string(return_type, &return_type_name);

	printf("   Name: %s\n", name);
	printf("Returns: %s\n", return_type_name);
	free(return_type_name);

	ctf_function_foreach_argument(function, NULL, dump_argument);
	printf("\n");
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

