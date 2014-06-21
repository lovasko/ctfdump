#include "../libctf/src/libctf.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Print the usage information.
 */
static void
usage ()
{
	printf("Usage: bsdctfdump file\n");
}

/**
 * Print all labels.
 * 
 * @param file file containing the labels
 */
static void
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
static void
dump_general_information (struct ctf_file *file)
{
	printf("-- General Information ------\n");
	printf("   Version: %d\n", ctf_file_get_version(file));
	printf("Compressed: %s\n", ctf_file_is_compressed(file) ? "yes" : "no");
	/*TODO print parent file information (None if not available, basename
	 * otherwise).  */
	printf("\n");
}

static const char*
kind_to_string (uint8_t kind)
{
	const char *translation_table[] = 
	{
		"none", 
		"int", 
		"float", 
		"pointer",
		"array",
		"function", 
		"struct", 
		"union", 
		"enum", 
		"forward", 
		"typedef",
		"volatile",
		"const", 
		"restrict"
	};

	if (kind <= CTF_KIND_MAX && kind >= 0)
		return translation_table[kind];
	else
		return "unresolvable";
}

static const char*
float_encoding_to_string (uint8_t encoding)
{
	const char *translation_table[] = 
	{
		"none", 
		"single", 
		"double", 
		"complex",
		"double complex",
		"long double complex", 
		"long double", 
		"interval", 
		"double interval", 
		"long double interval", 
		"imaginary",
		"double imaginary",
		"long double imaginary"
	};

	if (encoding <= CTF_FLOAT_MAX && encoding >= 0)
		return translation_table[encoding];
	else
		return "unresolvable";
}

/**
 * Print all types.
 * 
 * @param file file containing the types
 */
static void
dump_types (struct ctf_file *file)
{
	struct ctf_type *type = NULL;
	int retval; 

	printf("-- Types ------\n");

	while ((retval = ctf_file_get_next_type(file, type, &type)) == CTF_OK)
	{
		char *name = ctf_type_get_name(type);

		printf("      ID: %d\n", ctf_type_get_id(type));	
		printf("    Name: %s\n", (name && name[0] != '\0') ? name : "N/A");	
		printf("    Kind: %s\n", kind_to_string(ctf_type_get_kind(type)));

		switch (ctf_type_get_kind(type))
		{
			case CTF_KIND_INT:
				printf("    Size: %d\n", ctf_int_float_get_size(type->data));
				printf("  Offset: %d\n", ctf_int_float_get_offset(type->data));
				printf("  Signed: %s\n", ctf_int_is_signed(type->data) ? "yes" : "no");
				printf(" Content: %s\n", 
				    ctf_int_is_varargs(type->data) ? "varargs" : 
						(ctf_int_is_boolean(type->data) ? "boolean" : 
						(ctf_int_is_char(type->data) ? "char" : "number")));
			break;

			case CTF_KIND_FLOAT:
				printf("    Size: %d\n", ctf_int_float_get_size(type->data));
				printf("  Offset: %d\n", ctf_int_float_get_offset(type->data));
				printf("Encoding: %s\n", float_encoding_to_string(
				    ctf_float_get_encoding(type->data)));
			break;

			case CTF_KIND_ARRAY:
			break;

			case CTF_KIND_FUNC:
			break;

			case CTF_KIND_STRUCT:
			break;

			case CTF_KIND_UNION:
			break;

			case CTF_KIND_ENUM:
			break;

			case CTF_KIND_FWD_DECL:
			break;

			case CTF_KIND_TYPEDEF:
			break;

			case CTF_KIND_POINTER:
			break;

			case CTF_KIND_VOLATILE:
			break;

			case CTF_KIND_CONST:
			break;

			case CTF_KIND_RESTRICT:
			break;
		}

		printf("\n");
	}

	if (retval == CTF_EMPTY)
	{
		printf("No types.");
		return;
	}

	if (retval != CTF_END)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
	}

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
	dump_types(file);

	return EXIT_SUCCESS;
}

