#include <stdio.h>

#include "functions.h"

static void
dump_argument(ctf_argument argument, void* arg)
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
dump_function(ctf_function function, void* arg)
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
}

void
dump_functions(ctf_file file)
{
	/* char* argument_type_string; */
	/* char* name; */
	/* char* return_type_string; */
	/* ctf_argument argument; */
	/* ctf_function function; */
	/* ctf_type argument_type; */
	/* ctf_type return_type; */
	/* int inner_retval; */
	/* int retval; */ 

	ctf_file_foreach_function(file, NULL, dump_function);

	/* function = NULL; */
	/* while ((retval = ctf_file_get_next_function(file, function, &function)) */ 
	/*     == CTF_OK) */
	/* { */
	/* 	ctf_function_get_name(function, &name); */
	/* 	ctf_function_get_return_type(function, &return_type); */
	/* 	ctf_type_to_string(return_type, &return_type_string); */

	/* 	printf("   Name: %s\n", name); */
	/* 	printf("Returns: %s\n", return_type_string); */

	/* 	free(return_type_string); */

	/* 	argument = NULL; */
	/* 	while ((inner_retval = ctf_function_get_next_argument(function, */
	/* 			argument, &argument)) == CTF_OK) */
	/* 	{ */
	/* 		ctf_argument_get_type(argument, &argument_type); */
	/* 		ctf_type_to_string(argument_type, &argument_type_string); */

	/* 		printf("     %s\n", argument_type_string); */
	/* 		free(argument_type_string); */
	/* 	} */

	/* 	if (inner_retval == CTF_EMPTY) */
	/* 	{ */
	/* 		printf(" No arguments.\n"); */
	/* 		continue; */
	/* 	} */

		/* if (inner_retval != CTF_END) */
		/* { */
		/* 	printf("ERROR: %s\n", ctf_get_error_string(inner_retval)); */
		/* } */

		/* printf("\n"); */
	/* } */

	/* if (retval == CTF_EMPTY) */
	/* { */
		/* printf("No functions."); */
		/* return; */
	/* } */

	/* if (retval != CTF_END) */
	/* { */
		/* fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval)); */
	/* } */
}

