#include <ctf/ctf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dump.h"

/**
 * Print the usage information.
 */
static void
usage()
{
	printf("Usage: ctfdump [-dhfglt] <file>\n");
}

/**
 * Print the help string.
 */
static void
help()
{	
	printf(" -h\tprint this help message\n");
	printf(" -g\tdump general format information\n");
	printf(" -d\tdump data objects\n");
	printf(" -f\tdump function objects\n");
	printf(" -l\tdump labels\n");
	printf(" -t\tdump types\n");
}

/**
 * Print all available information stored inside the CTF section.
 * 
 * @param argc argument count
 * @param argv array of string arguments
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int
main(int argc, char* argv[])
{
	ctf_file file;
	int data_objects_flag;
	int functions_flag;
	int general_information_flag;
	int labels_flag;
	int option;
	int retval;
	int types_flag;

	general_information_flag = labels_flag = types_flag = data_objects_flag = 
	    functions_flag = 0;

	while ((option = getopt(argc, argv, "hgltdf")) != -1) {
		switch(option) {
			case 'g': 
				general_information_flag = 1;
			break;

			case 'l': 
				labels_flag = 1;
			break;

			case 't': 
				types_flag = 1;
			break;

			case 'd': 
				data_objects_flag = 1;
			break;

			case 'f': 
				functions_flag = 1;
			break;

			case 'h':
				usage();
				help();
			return EXIT_FAILURE;

			case '?':
				fprintf(stderr, "ERROR: invalid option %c\n", optopt);	
				usage();
			return EXIT_FAILURE;

			default: 
				fprintf(stderr, "ERROR: unknown error during option parsing\n");	
			return EXIT_FAILURE;
		}
	}

	if (argc - optind < 1) {
		usage();
		return EXIT_FAILURE;
	}

	if ((retval = ctf_file_read(argv[optind], &file)) != CTF_OK) {
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
		return EXIT_FAILURE;
	}

	if (general_information_flag == 0
	 && labels_flag == 0
	 && types_flag == 0
	 && data_objects_flag == 0
	 && functions_flag == 0) {
		general_information_flag = labels_flag = types_flag = data_objects_flag = 
		    functions_flag = 1;
	}

	if (general_information_flag == 1) {
		printf("-- General information ------\n");
		dump_general_information(file);
	}

	if (labels_flag == 1) {
		printf("-- Labels ------\n");
		dump_labels(file);
	}

	if (types_flag == 1) {
		printf("-- Types ------\n");
		dump_types(file);
	}

	if (data_objects_flag == 1) {
		printf("-- Data objects ------\n");
		dump_data_objects(file);
	}

	if (functions_flag == 1) {
		printf("-- Functions ------\n");
		dump_functions(file);
	}

	return EXIT_SUCCESS;
}

