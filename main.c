#include "../libctf/src/libctf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
		char *name;
		(void) ctf_label_get_name(label, &name);

		uint32_t index;
		(void) ctf_label_get_index(label, &index);

		printf(" Name: %s\n", name);	
		printf("Index: %d\n", index);	
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
	int version;
	(void) ctf_file_get_version(file, &version);

	int is_compressed;
	(void) ctf_file_is_compressed(file, &is_compressed);

	printf("-- General Information ------\n");
	printf("   Version: %d\n", version);
	printf("Compressed: %s\n", is_compressed ? "yes" : "no");
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

static char*
type_to_string (struct ctf_type *type)
{
	char result[1024];
	memset(result, '\0', 1024);

	uint8_t kind = ctf_type_get_kind(type);
	const char *kind_string = kind_to_string(kind);
	char *name_string = ctf_type_get_name(type);

	if (ctf_kind_is_complex(kind))
	{
		snprintf(result, 1024, "%s %s", kind_string, name_string);
		return strdup(result);
	}

	if (kind == CTF_KIND_FWD_DECL)
	{
		const char *type_reference_string = kind_to_string(type->type_reference);
		snprintf(result, 1024, "forward declaration of %s %s", type_reference_string,
		    name_string);
		return strdup(result);
	}

	if (ctf_kind_is_pure_reference(kind))
	{
		struct ctf_type *reference_type = type->data;
		char *reference_string = type_to_string(ctf_type_get_data(type));
		snprintf(result, 1024, "%s", reference_string);
		free(reference_string);
		return strdup(result);
	}

	return NULL;
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
	int inner_retval;

	printf("-- Types ------\n");

	struct ctf_enum_entry *enum_entry = NULL;
	struct ctf_member *member = NULL;
	char *type_string = NULL;
	struct ctf_array *array;
	struct ctf_function *function;
	struct ctf_argument *argument = NULL;
	while ((retval = ctf_file_get_next_type(file, type, &type)) == CTF_OK)
	{
		char *name = ctf_type_get_name(type);

		printf("       ID: %d\n", ctf_type_get_id(type));	
		printf("     Name: %s\n", (name && name[0] != '\0') ? name : "N/A");	
		printf("     Kind: %s\n", kind_to_string(ctf_type_get_kind(type)));
		printf("     Root: %s\n", ctf_type_is_root(type) ? "yes" : "no");

		switch (ctf_type_get_kind(type))
		{
			case CTF_KIND_INT:
				printf("     Size: %d\n", ctf_int_float_get_size(type->data));
				printf("   Offset: %d\n", ctf_int_float_get_offset(type->data));
				printf("   Signed: %s\n", ctf_int_is_signed(type->data) ? "yes" : "no");
				printf("  Content: %s\n", 
				    ctf_int_is_varargs(type->data) ? "varargs" : 
						(ctf_int_is_boolean(type->data) ? "boolean" : 
						(ctf_int_is_char(type->data) ? "char" : "number")));
			break;

			case CTF_KIND_FLOAT:
				printf("     Size: %d\n", ctf_int_float_get_size(type->data));
				printf("   Offset: %d\n", ctf_int_float_get_offset(type->data));
				printf(" Encoding: %s\n", float_encoding_to_string(
				    ctf_float_get_encoding(type->data)));
			break;

			case CTF_KIND_ARRAY:
				array = ctf_type_get_data(type);
				type_string = type_to_string(ctf_array_get_type(array));
				printf("  Content: %s\n", type_string);
				free(type_string);
				printf("    Count: %d\n", ctf_array_get_element_count(array));
			break;

			case CTF_KIND_FUNC:
				function = ctf_type_get_data(type);
				type_string = type_to_string(ctf_function_get_return_type(function));
				printf("  Returns: %s\n", type_string);
				free(type_string);

				while ((inner_retval = ctf_function_get_next_argument(function,
				    argument, &argument)) == CTF_OK)
				{
					type_string = type_to_string(ctf_argument_get_type(argument));
					printf("     Argument: %s\n", type_string);
					free(type_string);
				}

				if (inner_retval == CTF_EMPTY)
				{
					printf ("      No arguments.\n");
					break;
				}

				if (inner_retval != CTF_END)
				{
					printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
				}

				argument = NULL;
				inner_retval = CTF_OK;
			break;

			case CTF_KIND_STRUCT:
			case CTF_KIND_UNION:
				while ((inner_retval = ctf_type_get_next_member(type, member,
				    &member)) == CTF_OK)
				{
					printf("         Name: %s\n", ctf_member_get_name(member));

					type_string = type_to_string(ctf_member_get_type(member));
					printf("         Type: %s\n", type_string);
					free(type_string);

					if (ctf_type_get_kind(type) == CTF_KIND_STRUCT)
						printf("       Offset: %llu\n", ctf_member_get_offset(member));
				}

				if (inner_retval == CTF_EMPTY)
				{
					printf ("      No members.\n");
					break;
				}

				if (inner_retval != CTF_END)
				{
					printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
				}

				member = NULL;
				inner_retval = CTF_OK;
			break;

			case CTF_KIND_ENUM:
				while ((inner_retval = ctf_type_get_next_enum_entry(type, enum_entry,
				    &enum_entry)) == CTF_OK)
				{
					printf("       %s = %d\n", ctf_enum_entry_get_name(enum_entry),
					    ctf_enum_entry_get_value(enum_entry));
				}

				if (inner_retval == CTF_EMPTY)
				{
					printf ("      No enum entries.\n");
					break;
				}

				if (inner_retval != CTF_END)
				{
					printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
				}

				enum_entry = NULL;
				inner_retval = CTF_OK;
			break;

			case CTF_KIND_FWD_DECL:
			case CTF_KIND_TYPEDEF:
			case CTF_KIND_POINTER:
			case CTF_KIND_VOLATILE:
			case CTF_KIND_CONST:
			case CTF_KIND_RESTRICT:
				type_string = type_to_string(type);
				printf("Reference: %s\n", type_string);
				free(type_string);
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
 * Print all data objects.
 * 
 * @param file file containing the data objects 
 */
static void
dump_data_objects (struct ctf_file *file)
{
	struct ctf_data_object *data_object = NULL;
	int retval; 

	printf("-- Data Objects ------\n");

	while ((retval = ctf_file_get_next_data_object(file, data_object, 
	    &data_object)) == CTF_OK)
	{
		printf(" Name: %s\n", ctf_data_object_get_name(data_object));	

		char *type_string = type_to_string(ctf_data_object_get_type(data_object));
		printf(" Type: %s\n", type_string);	
		free(type_string);

		printf("\n");
	}

	if (retval == CTF_EMPTY)
	{
		printf("No data objects.");
		return;
	}

	if (retval != CTF_END)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
	}

	printf("\n");
}

/**
 * Print all functions.
 * 
 * @param file file containing the functions 
 */
static void
dump_functions (struct ctf_file *file)
{
	struct ctf_function *function = NULL;
	int retval; 
	int inner_retval;

	printf("-- Functions ------\n");

	while ((retval = ctf_file_get_next_function(file, function, &function)) 
	    == CTF_OK)
	{
		printf("    Name: %s\n", ctf_function_get_name(function));	

		char *return_type_string = type_to_string(ctf_function_get_return_type(
		    function));
		printf(" Returns: %s\n", return_type_string);	
		free(return_type_string);

		struct ctf_argument *argument = NULL;
		while ((inner_retval = ctf_function_get_next_argument(function,
				argument, &argument)) == CTF_OK)
		{
			char *type_string = type_to_string(ctf_argument_get_type(argument));
			printf("   Argument: %s\n", type_string);
			free(type_string);
		}

		if (inner_retval == CTF_EMPTY)
		{
			printf("   No arguments.\n");
			break;
		}

		if (inner_retval != CTF_END)
		{
			printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
		}

		printf("\n");
	}

	if (retval == CTF_EMPTY)
	{
		printf("No functions.");
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
	dump_data_objects(file);
	dump_functions(file);

	return EXIT_SUCCESS;
}

