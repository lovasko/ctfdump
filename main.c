#include "../libctf/src/libctf.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Print the usage information.
 */
void
usage ()
{
	printf("Usage: bsdctfdump file\n");
}

/**
 * Print all labels.
 * 
 * @param file file containing the labels
 */
void
dump_labels (struct ctf_file *file)
{
	struct ctf_label *label = NULL;
	int retval; 

	printf("-- Labels ------\n");

	while ((retval = ctf_file_get_next_label(file, label, &label)) == CTF_OK)
	{
		printf(" Name: %s\n", ctf_label_get_name(label));	
		printf("Index: %d\n", ctf_label_get_index(label));	
	}

	if (retval == CTF_EMPTY)
	{
		printf("No labels.");
		return;
	}

	if (retval != CTF_END)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
	}

	printf("\n");
}

/**
 * Print general information about the file.
 *
 * @param file the file in question
 */
void
dump_general_information (struct ctf_file *file)
{
	printf("-- General Information ------\n");
	printf("   Version: %d\n", ctf_file_get_version(file));
	printf("Compressed: %s\n", ctf_file_is_compressed(file) ? "yes" : "no");
	printf("\n");
}

/**
 * Print all available information stored inside the CTF section.
 * 
 * @param argc argument count
 * @param argv array of string arguments
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int
main (int argc, char **argv)
{
	if (argc < 2)
	{
		usage();
		return EXIT_FAILURE;
	}

	struct ctf_file *file;
	int retval;

	if ((retval = ctf_read_file(argv[1], &file)) != CTF_OK)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
		return EXIT_FAILURE;
	}

	dump_general_information(file);
	dump_labels(file);

	return EXIT_SUCCESS;
}

